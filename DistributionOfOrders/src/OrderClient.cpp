#include "OrderClient.h"

OrderClient::OrderClient(std::shared_ptr<grpc::Channel> channel) : stub(MailTaxiAPI::OrderService::NewStub(channel)){
}

bool OrderClient::RegDriver(const std::string &token, const MailTaxiAPI::Driver &driver,
                            const MailTaxiAPI::Coordinate &coordinate) {
    MailTaxiAPI::RegDriverRequest req;
    req.set_token(token);
    req.set_allocated_driver(new MailTaxiAPI::Driver(driver));
    req.set_allocated_coordinate(new MailTaxiAPI::Coordinate(coordinate));
    MailTaxiAPI::RegDriverResponse response;

    grpc::ClientContext context;
    grpc::Status status = stub->RegDriver(&context, req, &response);
    if (status.ok())
        return response.success();
    else{
        return false;
    }
}

std::vector<MailTaxiAPI::DriverInformation>
OrderClient::GetDrivers(const MailTaxiAPI::Coordinate &coordinate, MailTaxiAPI::OrderStatus orderStatus,
                        bool children, bool animals) {
    MailTaxiAPI::GetDriversRequest req;
    req.set_allocated_coordinate(new MailTaxiAPI::Coordinate(coordinate));
    req.set_status(orderStatus);
    req.set_children(children);
    req.set_animals(animals);
    MailTaxiAPI::GetDriversResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub->GetDrivers(&context, req, &response);
    if (status.ok()) {
        std::vector<MailTaxiAPI::DriverInformation> result(response.information_size());
        for (size_t i = 0; i < response.information_size(); i++){
            result.emplace_back(response.information()[i]);
        }
        return result;
    }
    else{
        return   std::vector<MailTaxiAPI::DriverInformation>();
    }
}

bool OrderClient::NewOrderForDriver(const std::string &token, const MailTaxiAPI::OrderData& orderData) {
    MailTaxiAPI::NewOrderForDriverRequest req;
    req.set_token(token);
    req.set_allocated_orderdata(new MailTaxiAPI::OrderData(orderData));
    MailTaxiAPI::NewOrderForDriverResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub->NewOrderForDriver(&context, req, &response);
    if (status.ok())
        return response.success();
    else{
        return false;
    }
}
