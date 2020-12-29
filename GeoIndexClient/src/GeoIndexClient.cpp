#include "GeoIndexClient.h"

#include <utility>


GeoIndexClient::GeoIndexClient(std::shared_ptr<grpc::Channel> channel) : stub(MailTaxi::GeoIndex::NewStub(channel))
{
}

void GeoIndexClient::CompleteRpcRun() {
    void* got_tag;
    bool ok = false;
    while (completionQueue.Next(&got_tag, &ok)) {
        auto handler = static_cast<GeoIndexRpcHandler*>(got_tag);
        handler->Handle(ok);
    }
}

void
GeoIndexClient::SelectDriver(const MailTaxi::Coordinate &coordinate, bool children, bool animals, uint64_t lvlIter,
                             std::shared_ptr<std::atomic_size_t> driverId, std::shared_ptr<OrderManagerClient> orderManagementClient) {
    MailTaxi::SelectDriversRequest request;
    request.set_allocated_coordinate(new MailTaxi::Coordinate(coordinate));
    request.set_children(children);
    request.set_animals(animals);
    request.set_lvliter(lvlIter);
    auto selectDriverRpc = new struct SelectDriverRpc;
    selectDriverRpc->driverId = std::move(driverId);
    selectDriverRpc->orderManagementClient = std::move(orderManagementClient);
    selectDriverRpc->responder = stub->PrepareAsyncSelectDrivers(&selectDriverRpc->context,
                                                              request, &completionQueue);
    selectDriverRpc->stat = SelectDriverRpc::PROCESS;
}


void GeoIndexClient::MoveDriver(const MailTaxi::Coordinate &coordinate, uint64_t driverId) {
    MailTaxi::MoveDriverRequest request;
    auto driverCoordinate = coordinate;
    request.set_allocated_coordinate(&driverCoordinate);
    request.set_driverid(driverId);
    auto moveDriverRpc = new MoveDriverRpc;
    moveDriverRpc->responseReader = stub->PrepareAsyncMoveDriver(&moveDriverRpc->context,
                                                                 request, &completionQueue);
    moveDriverRpc->responseReader->StartCall();
    moveDriverRpc->responseReader->Finish(&moveDriverRpc->response, &moveDriverRpc->status,
                                          static_cast<void*>(moveDriverRpc));
    request.release_coordinate();
}

void GeoIndexClient::RegisterDriver(uint64_t driverId, const MailTaxi::Coordinate &coordinate, MailTaxi::Driver& driver) {
    MailTaxi::RegisterDriverRequest request;
    request.set_driverid(driverId);
    request.set_allocated_coordinate(new MailTaxi::Coordinate(coordinate));
    request.set_allocated_driver(&driver);

    auto registerDriverRpc = new RegisterDriverRpc;
    registerDriverRpc->responseReader = stub->PrepareAsyncRegisterDriver(&registerDriverRpc->context,
                                                                 request, &completionQueue);
    registerDriverRpc->responseReader->StartCall();
    registerDriverRpc->responseReader->Finish(&registerDriverRpc->response, &registerDriverRpc->status,
                                          static_cast<void*>(registerDriverRpc));
    request.release_driver();
}

void GeoIndexClient::RemoveDriver(uint64_t driverId) {
    MailTaxi::RemoveDriverRequest request;
    request.set_driverid(driverId);
    auto removeDriverRpc = new RemoveDriverRpc;
    removeDriverRpc->responseReader = stub->PrepareAsyncRemoveDriver(&removeDriverRpc->context,
                                                                       request, &completionQueue);
    removeDriverRpc->responseReader->StartCall();
    removeDriverRpc->responseReader->Finish(&removeDriverRpc->response, &removeDriverRpc->status,
                                              static_cast<void*>(removeDriverRpc));
}

void MoveDriverRpc::Handle(bool ok) {
}

void RegisterDriverRpc::Handle(bool ok) {
    std::cout << "Регстрация: " << response.success() << std::endl;
}

void SelectDriverRpc::Handle(bool ok) {
    if (stat == PROCESS){
        if (!response.driverid().empty()) {
            driverId->store(response.driverid(0));
            std::cout << "Выбран водитель " << response.driverid(0) << std::endl;
        }

        stat = FINISH;
    }
    else{
        delete this;
    }
}

void RemoveDriverRpc::Handle(bool ok) {
    std::cout << "Удаление: " << response.driverfound() << std::endl;
}
