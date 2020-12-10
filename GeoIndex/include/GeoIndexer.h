#pragma once
#include "QuadTree.h"
#include "GeoIndex.pb.h"
#include <utility>

using namespace Cone::Geo;

template <class DataType, class InsertReturnType = void, class FindReturnType = void>
class IGeoIndexer {
public:
    virtual InsertReturnType Insert(std::pair<std::string, DataType> data, const MailTaxiAPI::Coordinate& coordinate) = 0;
    virtual InsertReturnType Update(const MailTaxiAPI::Coordinate& coordinate, const std::string& key, std::function<void(DataType&)>) = 0;
    virtual FindReturnType Find(const MailTaxiAPI::Coordinate& coordinate) = 0;
    virtual ~IGeoIndexer() = default;
};

template <class DataType>
class QuadTreeGeoIndexerMover{
private:
    typename std::map<std::string, DataType>::iterator it;
    QuadTreeObjectMover<std::map<std::string, DataType>, std::mutex> mover;
public:
    QuadTreeGeoIndexerMover(typename std::map<std::string, DataType>::iterator it,
                            QuadTreeObjectMover<std::map<std::string, DataType>, std::mutex> mover);
    bool operator()(const MailTaxiAPI::Coordinate& coordinate);
    DataType & GetData();
    const Quadrant& GetQuadrant();
};

template<class DataType>
bool QuadTreeGeoIndexerMover<DataType>::operator()(const MailTaxiAPI::Coordinate &coordinate) {
    Coordinate _coordinate(coordinate.latitude(), coordinate.longitude());
    std::string keyBuffer;
    DataType valueBuffer;
    return mover(_coordinate, [this, &keyBuffer, &valueBuffer](std::map<std::string, DataType>& dataContainer, const Quadrant& quadrant,
                                     std::mutex& lock, bool firstQuadrant){
        lock.lock();
        if (firstQuadrant){
            keyBuffer = it->first;
            valueBuffer = it->second;
            dataContainer.erase(it);
        }
        else{
            it = dataContainer.insert({std::move(keyBuffer), std::move(valueBuffer)}).first;
        }
        lock.unlock();
    });
}

template<class DataType>
QuadTreeGeoIndexerMover<DataType>::QuadTreeGeoIndexerMover(typename std::map<std::string, DataType>::iterator it,
                                                           QuadTreeObjectMover<std::map<std::string, DataType>, std::mutex> mover) :
                                                                       it(std::move(it)), mover(mover){

}

template<class DataType>
DataType & QuadTreeGeoIndexerMover<DataType>::GetData() {
    return it->second;
}

template<class DataType>
const Quadrant &QuadTreeGeoIndexerMover<DataType>::GetQuadrant() {
    return mover.GetQuadrant();
}


template <class DataType>
class QuadTreeGeoIndexerRoundMover{
private:
    QuadTreeObjectMover<std::map<std::string, DataType>, std::mutex> mover;
    std::queue<Coordinate> coordinates;
    uint32_t lvl;
    uint32_t divider;
    double latDistance;
    double longDistance;
    static const size_t cornerPointNum = 4;
    static const size_t increaseInQuadrantsPerLvl = 8;
    std::array<std::function<Coordinate(const Coordinate&)>, cornerPointNum> shift;
public:
    explicit QuadTreeGeoIndexerRoundMover( QuadTreeObjectMover<std::map<std::string, DataType>, std::mutex> mover, size_t deep);
    QuadTreeGeoIndexerRoundMover<DataType>& operator++();
    const std::map<std::string, DataType>& GetData();
    const Quadrant& GetQuadrant();
};

template<class DataType>
QuadTreeGeoIndexerRoundMover<DataType> &QuadTreeGeoIndexerRoundMover<DataType>::operator++() {
    if (coordinates.size() == 1){
        Coordinate currentCord = coordinates.front();
        std::cout << currentCord.first << ' ' << currentCord.second << std::endl;
        mover(currentCord, [](std::map<std::string, DataType>& dataContainer, const Quadrant& quadrant, std::mutex& lock, bool){});
        coordinates.pop();
        lvl++;
        divider+=2;
        size_t quadrantInLvl = increaseInQuadrantsPerLvl * lvl;
        currentCord = shift[3](currentCord);
        coordinates.push(currentCord);
        for (size_t i = 1; i < quadrantInLvl; i++){
            currentCord = shift[i / divider](currentCord);
            coordinates.push(currentCord);
        }
    }
    else {
        mover(coordinates.front(), [](std::map<std::string, DataType>& dataContainer, const Quadrant& quadrant, std::mutex& lock, bool) {});
        std::cout << coordinates.front().first << ' ' << coordinates.front().second << std::endl;
        coordinates.pop();
    }
    return *this;
}

template<class DataType>
QuadTreeGeoIndexerRoundMover<DataType>::QuadTreeGeoIndexerRoundMover(
        QuadTreeObjectMover<std::map<std::string, DataType>, std::mutex> mover, size_t deep) : mover(std::move(mover)) {
    lvl = 0;
    divider = 0;
    coordinates.push(mover.GetQuadrant().GetCenter());
    latDistance = 180;
    longDistance = 360;
    static const double reduction = 2;
    for (size_t i = 0; i < deep; i++) {
        latDistance /= reduction;
        longDistance /= reduction;
    }
    shift[0] = [this](const Coordinate &point) {
        return Coordinate(point.first, point.second + longDistance);
    };
    shift[1] = [this](const Coordinate &point) {
        return Coordinate(point.first - latDistance, point.second);
    };
    shift[2] = [this](const Coordinate &point) {
        return Coordinate(point.first, point.second - longDistance);
    };
    shift[3] = [this](const Coordinate &point) {
        return Coordinate(point.first + latDistance, point.second);
    };
}

template<class DataType>
const std::map<std::string, DataType> &QuadTreeGeoIndexerRoundMover<DataType>::GetData() {
    return mover.GetData();
}

template<class DataType>
const Quadrant& QuadTreeGeoIndexerRoundMover<DataType>::GetQuadrant() {
    return mover.GetQuadrant();
}

template <class DataType>
class QuadTreeGeoIndexer final : public IGeoIndexer<DataType, QuadTreeGeoIndexerMover<DataType>,
        QuadTreeGeoIndexerRoundMover<DataType>>{
private:
    constexpr static const Coordinate _northWest = {90, -180};
    constexpr static const Coordinate _northEast = {90, 180};
    constexpr static const Coordinate _southWest = {-90, -180};
    constexpr static const Coordinate _southEast = {-90, 180};
    static const size_t optimalDeep = 14;
    QuadTree<std::map<std::string, DataType>, std::mutex> quadTree;
public:
    QuadTreeGeoIndexer(const MailTaxiAPI::Coordinate& northWest,const MailTaxiAPI::Coordinate& northEast,
                       const MailTaxiAPI::Coordinate& southWest, const MailTaxiAPI::Coordinate& southEast, size_t deep);
    explicit QuadTreeGeoIndexer(size_t deep = optimalDeep);
    QuadTreeGeoIndexerMover<DataType> Insert(std::pair<std::string, DataType> data,
                                             const MailTaxiAPI::Coordinate& coordinate) override;
    QuadTreeGeoIndexerMover<DataType>
    Update(const MailTaxiAPI::Coordinate& coordinate,const std::string& key, std::function<void(DataType&)>) override;
    QuadTreeGeoIndexerRoundMover<DataType> Find(const MailTaxiAPI::Coordinate& coordinate) override;
    ~QuadTreeGeoIndexer() override = default;
};

template<class DataType>
QuadTreeGeoIndexer<DataType>::QuadTreeGeoIndexer(const MailTaxiAPI::Coordinate& northWest,const MailTaxiAPI::Coordinate& northEast,
                                                 const MailTaxiAPI::Coordinate& southWest,const MailTaxiAPI::Coordinate& southEast,
                                                 size_t deep) : quadTree({northWest.latitude(), northWest.longitude()},
                                                                         {northEast.latitude(), northEast.longitude()},
                                                                         {southWest.latitude(), southWest.longitude()},
                                                                         {southEast.latitude(), southEast.longitude()},
                                                                         deep){
}

template<class DataType>
QuadTreeGeoIndexer<DataType>::QuadTreeGeoIndexer(size_t deep) : quadTree(_northWest, _northEast,
                                                                         _southWest, _southEast, deep){
}


template<class DataType>
QuadTreeGeoIndexerMover<DataType>
QuadTreeGeoIndexer<DataType>::Insert(std::pair<std::string, DataType> data, const MailTaxiAPI::Coordinate &coordinate) {
    Coordinate _coordinate(coordinate.latitude(), coordinate.longitude());
    typename std::map<std::string, DataType>::iterator it;
    auto mover = quadTree.UpdateAndGetMover(_coordinate,
                                            [&data, &it](std::map<std::string, DataType>& dataContainer, const Quadrant& quadrant, std::mutex& lock){
        lock.lock();
        it = dataContainer.insert(data).first;
        lock.unlock();
    });
    return QuadTreeGeoIndexerMover<DataType>(std::move(it), mover);
}

template<class DataType>
QuadTreeGeoIndexerMover<DataType> QuadTreeGeoIndexer<DataType>::Update(const MailTaxiAPI::Coordinate &coordinate,
                                                                       const std::string& key,
                                                                       std::function<void(DataType&)>handler) {
    Coordinate _coordinate(coordinate.latitude(), coordinate.longitude());
    typename std::map<std::string, DataType>::iterator it;
    auto mover = quadTree.UpdateAndGetMover(_coordinate,[handler, &key, &it]
            (std::map<std::string, DataType>& dataContainer, const Quadrant& quadrant, std::mutex& lock){
        lock.lock();
        it = dataContainer.find(key);
        if (it != dataContainer.end())
            handler(it->second);
        lock.unlock();
    });
    return QuadTreeGeoIndexerMover<DataType>(std::move(it), mover);
}

template<class DataType>
QuadTreeGeoIndexerRoundMover<DataType>
QuadTreeGeoIndexer<DataType>::Find(const  MailTaxiAPI::Coordinate &coordinate) {
    Coordinate _coordinate(coordinate.latitude(), coordinate.longitude());
    auto mover = quadTree.UpdateAndGetMover(_coordinate,[](std::map<std::string, DataType>& dataContainer, const Quadrant& quadrant, std::mutex& lock){});
    return QuadTreeGeoIndexerRoundMover<DataType>(mover, optimalDeep);
}

