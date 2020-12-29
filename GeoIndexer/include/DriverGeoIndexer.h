#pragma once
#include <map>
#include <iostream>

#include "QuadTree.h"
#include "CashTable.h"
#include "GeoIndex.grpc.pb.h"

class GeoIndexerMover{
private:
    Geo::QuadTreeObjectMover<std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex> mover;
    std::queue<Geo::Coordinate> coordinates;
    uint32_t lvl;
    uint32_t divider;
    double latDistance;
    double longDistance;
public:
    explicit GeoIndexerMover(
            Geo::QuadTreeObjectMover<std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex> mover,
            size_t deep);
    GeoIndexerMover(GeoIndexerMover&& other) noexcept ;
    GeoIndexerMover& operator++();
    std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>> & GetData();
    const Geo::Quadrant& GetQuadrant();
};

class DriverGeoIndexer{
private:
    Geo::QuadTree<std::map<uint64_t, std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex> quadTree;
    std::shared_ptr<ICashTable<uint64_t,
    std::shared_ptr<Geo::QuadTreeObjectMover<std::map<uint64_t,
        std::shared_ptr<std::pair<MailTaxi::Driver, MailTaxi::Coordinate>>>, std::mutex>>>> cashTable;
    DriverGeoIndexer();
public:
    static std::shared_ptr<DriverGeoIndexer> GetDriverGeoIndexerSingleton();
    bool RegistrationDriver(uint64_t driverId, const MailTaxi::Driver &driver, const MailTaxi::Coordinate &coordinate);
    bool MoveDriver(uint64_t driverId, const MailTaxi::Coordinate &coordinate);
    bool RemoveDriver(uint64_t driverId);
    GeoIndexerMover SelectDrivers(const MailTaxi::Coordinate& coordinate);
};