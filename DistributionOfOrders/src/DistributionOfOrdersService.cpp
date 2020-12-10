#include "DistributionOfOrdersService.h"

#include <utility>

grpc::Status
DistributionOfOrdersService::MakeOrder(::grpc::ServerContext *context, const ::MailTaxiAPI::MakeOrderRequest *request,
                                       ::MailTaxiAPI::MakeOrderResponse *response) {
    //оавторизовать
    auto drivers = orderClient.GetDrivers(request->orderdata().clientaddress(), request->type(),
                                           request->children(), request->animals());
    if (drivers.empty())
        return grpc::Status::CANCELLED;
    else{
        std::string token = filterDrivers.create_order(drivers, request->orderdata().clientaddress());
        response->set_drivertoken(token);
        if (orderClient.NewOrderForDriver(token, request->orderdata())){
            return grpc::Status::OK;
        }
        else
            return grpc::Status::CANCELLED;
    }

}

grpc::Status DistributionOfOrdersService::RegistrationUser(::grpc::ServerContext *context,
                                                           const ::MailTaxiAPI::RegistrationUserRequest *request,
                                                           ::MailTaxiAPI::RegDriverResponse *response) {
    return Service::RegistrationUser(context, request, response);
}

grpc::Status DistributionOfOrdersService::UserAuthorization(::grpc::ServerContext *context,
                                                              const ::MailTaxiAPI::UserAuthorizationRequest *request,
                                                              ::MailTaxiAPI::UserAuthorizationResponse *response) {

    return Service::UserAuthorization(context, request, response);
}

grpc::Status DistributionOfOrdersService::StartChangingDriver(::grpc::ServerContext *context,
                                                              const ::MailTaxiAPI::StartChangingDriverRequest *request,
                                                              ::MailTaxiAPI::StartChangingDriverResponse *response) {
    MailTaxiAPI::Driver driver;
    orderClient.RegDriver(request->token(), driver, request->coordinate());
    return Service::StartChangingDriver(context, request, response);
}

DistributionOfOrdersService::DistributionOfOrdersService(std::shared_ptr<grpc::Channel> channel) : orderClient(std::move(channel)){

}


GrpcDistributionOfOrdersService::GrpcDistributionOfOrdersService(std::shared_ptr<grpc::Channel> orderClient,
                                                                 const std::string &addr) : orderService(std::move(orderClient)){
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&orderService);
    server = std::move(std::unique_ptr<grpc::Server>(builder.BuildAndStart()));
}

GrpcDistributionOfOrdersService::~GrpcDistributionOfOrdersService() {
    server->Wait();
}
