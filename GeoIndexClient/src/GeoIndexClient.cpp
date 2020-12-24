#include "GeoIndexClient.h"


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

void GeoIndexClient::SelectDriverRpc(const MailTaxi::Coordinate &coordinate, bool children, bool animals) {
    MailTaxi::SelectDriversRequest request;
    request.set_allocated_coordinate(new MailTaxi::Coordinate(coordinate));
    request.set_children(children);
    request.set_animals(animals);
    auto selectDriverRpc = new struct SelectDriverRpc;
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
    delete this;
}

void MoveDriverRpc::Handle(bool ok) {
    if (!response.driverfound()){
        std::cout << "Водитель не найден в GeoIndex" << std::endl;
    }
}

void RegisterDriverRpc::Handle(bool ok) {

}
