#pragma once

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "GeoIndex.grpc.pb.h"
#include "GeoIndex.pb.h"
#include "OrderClient.h"
#include "FilterDrivers.h"

class DistributionOfOrdersService final : public MailTaxiAPI::DistributionOfOrders::Service{
private:
    OrderClient orderClient;
    Filter_Drivers filterDrivers;
public:
    explicit DistributionOfOrdersService(std::shared_ptr<grpc::Channel> channel);
    grpc::Status MakeOrder(::grpc::ServerContext* context, const ::MailTaxiAPI::MakeOrderRequest* request,
                           ::MailTaxiAPI::MakeOrderResponse* response) override;
    grpc::Status UserAuthorization(::grpc::ServerContext* context, const ::MailTaxiAPI::UserAuthorizationRequest* request,
                                     ::MailTaxiAPI::UserAuthorizationResponse* response) override;
    grpc::Status StartChangingDriver(::grpc::ServerContext* context, const ::MailTaxiAPI::StartChangingDriverRequest* request,
                                     ::MailTaxiAPI::StartChangingDriverResponse* response) override;
    grpc::Status RegistrationUser(::grpc::ServerContext* context, const ::MailTaxiAPI::RegistrationUserRequest* request,
                                  ::MailTaxiAPI::RegDriverResponse* response) override;
    ~DistributionOfOrdersService() override = default;
};

class GrpcDistributionOfOrdersService{
private:
    DistributionOfOrdersService orderService;
    grpc::ServerBuilder builder;
    std::unique_ptr<grpc::Server> server;
public:
    explicit GrpcDistributionOfOrdersService(std::shared_ptr<grpc::Channel> channel, const std::string &addr);
    ~GrpcDistributionOfOrdersService();
};