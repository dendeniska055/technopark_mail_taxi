#pragma once
#include "GeoIndex.pb.h"
#include "GeoIndex.grpc.pb.h"
#include "GeoIndexer.h"
#include "Web.h"
#include <chrono>

struct DriverData{
    MailTaxiAPI::Driver driver;
    MailTaxiAPI::Coordinate coordinate;
    bool busy;
    bool active;
};

typedef QuadTreeGeoIndexerMover<DriverData> GeoMover;
typedef QuadTreeGeoIndexerRoundMover<DriverData> GeoRoundMover;
typedef IGeoIndexer<DriverData, GeoMover, GeoRoundMover> GeoIndexer;

struct OrderData{
    std::shared_ptr<Cone::Web::Connection> driverConnection;
    MailTaxiAPI::Coordinate driveCoordinate;
    std::chrono::time_point<std::chrono::_V2::system_clock> driverLastPingTime;
    bool driverConnect;
    bool driverBusy;
};