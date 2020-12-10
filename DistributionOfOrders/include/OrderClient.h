#pragma once
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "GeoIndex.grpc.pb.h"
#include "GeoIndex.pb.h"

class OrderClient {
private:
    std::unique_ptr<MailTaxiAPI::OrderService::Stub> stub;
public:
    explicit OrderClient(std::shared_ptr<grpc::Channel> channel);
    bool RegDriver(const std::string& token, const MailTaxiAPI::Driver& driver, const MailTaxiAPI::Coordinate& coordinate);
    std::vector<MailTaxiAPI::DriverInformation> GetDrivers(const MailTaxiAPI::Coordinate& coordinate,
                                                           MailTaxiAPI::OrderStatus orderStatus, bool children , bool animals);
    bool NewOrderForDriver(const std::string & token, const MailTaxiAPI::OrderData& orderData);
};

