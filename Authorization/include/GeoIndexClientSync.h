#pragma once
#include "iostream"
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>
#include "OrderManagerClient.h"
#include "GeoIndex.grpc.pb.h"
#include "functional"

class GeoIndexClientSync {
private:
    std::unique_ptr <MailTaxi::GeoIndex::Stub> stub;
public:
    explicit GeoIndexClientSync(std::shared_ptr<grpc::Channel>channel);
    ~GeoIndexClientSync() = default;
    MailTaxi::SelectDriversResponse SelectDriver(const MailTaxi::Coordinate &coordinate, bool children, bool animals, uint64_t lvlIter);
    MailTaxi::RegisterDriverResponse RegisterDriver(uint64_t driverId, const MailTaxi::Coordinate &coordinate, MailTaxi::Driver &driver);
    MailTaxi::RemoveDriverResponse RemoveDriver(uint64_t driverId);
};