#pragma once
#include "GeoIndex.grpc.pb.h"
#include "GeoIndex.pb.h"
#include "CashTable.h"
#include "GeoIndexer.h"
#include "OrderDataStructs.h"
#include "Web.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "CDSHelper.h"
class OrderService final : public MailTaxiAPI::OrderService::Service{
private:
    std::shared_ptr<GeoIndexer> geoIndexer;
    std::shared_ptr<ICashTable<std::string, std::shared_ptr<OrderData>>> cashTable;
    std::shared_ptr<Cone::Web::EventLoop> eventLoop;
public:
    OrderService(std::shared_ptr<GeoIndexer>,
            std::shared_ptr<ICashTable<std::string, std::shared_ptr<OrderData>>>, std::shared_ptr<Cone::Web::EventLoop>);
    grpc::Status GetDrivers(::grpc::ServerContext* context, const ::MailTaxiAPI::GetDriversRequest* request,
                            ::MailTaxiAPI::GetDriversResponse* response) override;
    grpc::Status RegDriver(::grpc::ServerContext* context, const ::MailTaxiAPI::RegDriverRequest* request,
                           ::MailTaxiAPI::RegDriverResponse* response) override;
    grpc::Status NewOrderForDriver(::grpc::ServerContext* context, const MailTaxiAPI::NewOrderForDriverRequest* request,
                                   MailTaxiAPI::NewOrderForDriverResponse* response) override;
    ~OrderService() override = default;
};

class GrpcOrderService{
private:
    OrderService orderService;
    grpc::ServerBuilder builder;
    std::unique_ptr<grpc::Server> server;
public:
    GrpcOrderService(std::shared_ptr<GeoIndexer>,
                     std::shared_ptr<ICashTable<std::string, std::shared_ptr<OrderData>>>,
                             std::shared_ptr<Cone::Web::EventLoop>, std::string addr);
    ~GrpcOrderService();
};
