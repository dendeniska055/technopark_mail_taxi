#include "DriverGeoIndexer.h"

void
DriverGeoIndexer::RegistrationDriver(uint64_t driverId, const MailTaxi::Driver &driver, const MailTaxi::Coordinate &coordinate) {
    std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>> temp =
            std::make_shared<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>(driver, coordinate);
    auto tempMover = quadTree.UpdateAndGetMover({coordinate.latitude(), coordinate.longitude()},
                                                [&driver, &driverId, &coordinate, &temp]
                                                (std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver,
                                                        MailTaxi::Coordinate>>>& data,const Geo::Quadrant& quadrant, std::mutex& lock){
        lock.lock();
        data.insert({driverId, std::move(temp)});
        lock.unlock();
    });
    std::shared_ptr<Geo::QuadTreeObjectMover<std::map<uint64_t,
    std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex>> mover =
            std::make_shared<Geo::QuadTreeObjectMover<std::map<uint64_t,
            std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex>>(std::move(tempMover));
    cashTable->Insert(driverId, mover);
}
const size_t optimalDeep = 14;
DriverGeoIndexer::DriverGeoIndexer() : quadTree({90, -180},
                                                {90, 180}, {-90, -180}, {-90, 180}, optimalDeep){
    cashTable = std::make_shared<CashTableLockFree<uint64_t,
            std::shared_ptr<Geo::QuadTreeObjectMover<std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex>>>>();
}

std::shared_ptr<DriverGeoIndexer> DriverGeoIndexer::GetDriverGeoIndexerSingleton() {
    static std::shared_ptr<DriverGeoIndexer> geoIndexer(new DriverGeoIndexer);
    return geoIndexer;
}

bool DriverGeoIndexer::MoveDriver(uint64_t driverId, const MailTaxi::Coordinate &coordinate) {
    return cashTable->Find(driverId, [&coordinate](const uint64_t& key,
            std::shared_ptr<Geo::QuadTreeObjectMover<std::map<uint64_t,
            std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex>>& mover){
        uint64_t id;
        std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>> driverData;
        mover->operator()({coordinate.latitude(), coordinate.longitude()}, [&key, &id, &driverData, &coordinate](std::map<uint64_t,
                std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>& data, const Geo::Quadrant&,
                std::mutex& lock, bool firstQuadrant){
            lock.lock();
            if (firstQuadrant){
                auto it = data.find(key);
                id = it->first;
                driverData = std::move(it->second);
                driverData->second = coordinate;
                data.erase(it);
            }
            else
                data.insert({id, std::move(driverData)});
            lock.unlock();
        });
    });
}

bool DriverGeoIndexer::RemoveDriver(uint64_t driverId) {
    bool found = cashTable->Find(driverId, [&driverId](const uint64_t& key,
                                                       std::shared_ptr<Geo::QuadTreeObjectMover<std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex>>& mover){
        auto data = mover->GetData();
        auto it = data.find(driverId);
        data.erase(it);
    });
    cashTable->Erase(driverId);
    return found;
}

GeoIndexerMover DriverGeoIndexer::SelectDrivers(const MailTaxi::Coordinate &coordinate) {
    auto mover = quadTree.UpdateAndGetMover({coordinate.latitude(), coordinate.longitude()},
                                            [](std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>&, const Geo::Quadrant&,
                                                    std::mutex& ){});
    return GeoIndexerMover(std::move(mover), optimalDeep);
}

GeoIndexerMover::GeoIndexerMover(
        Geo::QuadTreeObjectMover<std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex> mover,
        size_t deep) : mover(std::move(mover)) {
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
}

GeoIndexerMover &GeoIndexerMover::operator++() {
    constexpr static const size_t cornerPointNum = 4;
    constexpr static const size_t increaseInQuadrantsPerLvl = 8;
    static std::array<std::function<Geo::Coordinate(const Geo::Coordinate&)>, cornerPointNum> shift;
    shift[0] = [this](const Geo::Coordinate &point) {
        return Geo::Coordinate(point.first, point.second + longDistance);
    };
    shift[1] = [this](const Geo::Coordinate &point) {
        return Geo::Coordinate(point.first - latDistance, point.second);
    };
    shift[2] = [this](const Geo::Coordinate &point) {
        return Geo::Coordinate(point.first, point.second - longDistance);
    };
    shift[3] = [this](const Geo::Coordinate &point) {
        return Geo::Coordinate(point.first + latDistance, point.second);
    };
    if (coordinates.size() == 1){
        Geo::Coordinate currentCord = coordinates.front();
        std::cout << currentCord.first << ' ' << currentCord.second << std::endl;
        mover(currentCord, [](std::map<uint64_t , std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>& dataContainer, const Geo::Quadrant& quadrant, std::mutex& lock, bool){});
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
        mover(coordinates.front(), [](std::map<uint64_t , std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>& dataContainer,
                const Geo::Quadrant& quadrant, std::mutex& lock, bool) {});
        coordinates.pop();
    }
    return *this;
}

std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>> & GeoIndexerMover::GetData() {
    return mover.GetData();
}

const Geo::Quadrant &GeoIndexerMover::GetQuadrant() {
    return mover.GetQuadrant();
}

GeoIndexerMover::GeoIndexerMover(GeoIndexerMover &&other) noexcept : mover(std::move(other.mover)) {
    this->coordinates = std::move(other.coordinates);
    this->divider = other.divider;
    this->longDistance = other.longDistance;
    this->latDistance = other.latDistance;
    this->lvl = other.lvl;
}
