
#include "GeoIndexClientSync.h"


MailTaxi::RegisterDriverResponse GeoIndexClientSync::RegisterDriver(uint64_t driverId, const MailTaxi::Coordinate &coordinate,
                                   MailTaxi::Driver &driver) {
    grpc::ClientContext context;
    MailTaxi::RegisterDriverRequest request;
    MailTaxi::RegisterDriverResponse response;
    request.set_allocated_coordinate(new MailTaxi::Coordinate(coordinate));
    request.set_driverid(driverId);
    request.set_allocated_driver(new MailTaxi::Driver(driver));
    stub->RegisterDriver(&context, request, &response);
    return response;
}

MailTaxi::RemoveDriverResponse GeoIndexClientSync::RemoveDriver(uint64_t driverId) {
    grpc::ClientContext context;
    MailTaxi::RemoveDriverRequest request;
    request.set_driverid(driverId);
    MailTaxi::RemoveDriverResponse response;
    stub->RemoveDriver(&context, request, &response);
    return response;
}

GeoIndexClientSync::GeoIndexClientSync(std::shared_ptr<grpc::Channel> channel) : stub(MailTaxi::GeoIndex::NewStub(channel)){

}

MailTaxi::SelectDriversResponse
GeoIndexClientSync::SelectDriver(const MailTaxi::Coordinate &coordinate, bool children, bool animals,
                                 uint64_t lvlIter) {
    grpc::ClientContext context;
    MailTaxi::SelectDriversRequest request;
    request.set_allocated_coordinate(new MailTaxi::Coordinate(coordinate));
    request.set_children(children);
    request.set_animals(animals);
    request.set_lvliter(lvlIter);
    MailTaxi::SelectDriversResponse response;
    stub->SelectDrivers(&context, request, &response);
    return response;
}
