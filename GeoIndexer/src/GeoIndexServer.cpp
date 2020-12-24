#include "GeoIndexServer.h"
#include "DriverGeoIndexer.h"
#include <utility>
#include "CDSHelper.h"

RegisterDriverRpc::RegisterDriverRpc(std::shared_ptr<MailTaxi::GeoIndex::AsyncService> service,
                                     std::shared_ptr<grpc::ServerCompletionQueue> completionQueue) :
                                     service(std::move(service)), completionQueue(std::move(completionQueue)),
                                     responseWriter(&context), status(CREATE){
    Handle(false);
}

void RegisterDriverRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestRegisterDriver(&context, &request, &responseWriter, completionQueue.get(),
                                       completionQueue.get(), this);
    }
    else if (status == PROCESS){
        auto geoIndexer = DriverGeoIndexer::GetDriverGeoIndexerSingleton();
        new RegisterDriverRpc(service, completionQueue);
        geoIndexer->RegistrationDriver(request.driverid(), request.driver(), request.coordinate());
        response.set_success(true);
        status = FINISH;
        responseWriter.Finish(response, grpc::Status::OK, this);
    }
    else{
        delete this;
    }
}

MoveDriverRpc::MoveDriverRpc(std::shared_ptr<MailTaxi::GeoIndex::AsyncService> service,
                             std::shared_ptr<grpc::ServerCompletionQueue> completionQueue) :service(std::move(service)),
                                                                                            completionQueue(std::move(completionQueue)),responseWriter(&context), status(CREATE){
    Handle(false);
}

void MoveDriverRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestMoveDriver(&context, &request, &responseWriter, completionQueue.get(),
                                   completionQueue.get(), this);
    }
    else if(status == PROCESS){
        auto geoIndexer = DriverGeoIndexer::GetDriverGeoIndexerSingleton();
        new MoveDriverRpc(service, completionQueue);
        bool driverFound = geoIndexer->MoveDriver(request.driverid(), request.coordinate());
        response.set_driverfound(driverFound);
        status = FINISH;
        responseWriter.Finish(response, grpc::Status::OK, this);
    }
    else{
        delete this;
    }

}

RemoveDriverRpc::RemoveDriverRpc(std::shared_ptr<MailTaxi::GeoIndex::AsyncService> service,
                                 std::shared_ptr<grpc::ServerCompletionQueue> completionQueue) :service(std::move(service)),
                                                                                                completionQueue(std::move(completionQueue)),responseWriter(&context), status(CREATE){
    Handle(false);
}

void RemoveDriverRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestRemoveDriver(&context, &request, &responseWriter, completionQueue.get(),
                                   completionQueue.get(), this);
    }
    else if(status == PROCESS){
        auto geoIndexer = DriverGeoIndexer::GetDriverGeoIndexerSingleton();
        new RemoveDriverRpc(service, completionQueue);
        bool driverFound = geoIndexer->RemoveDriver(request.driverid());
        response.set_driverfound(driverFound);
        status = FINISH;
        responseWriter.Finish(response, grpc::Status::OK, this);
    }
    else
        delete this;
}

SelectDriverRpc::SelectDriverRpc(std::shared_ptr<MailTaxi::GeoIndex::AsyncService> service,
                                 std::shared_ptr<grpc::ServerCompletionQueue> completionQueue) :
                                 service(std::move(service)),completionQueue(std::move(completionQueue)),
                                 responder(&context), status(CREATE){
    Handle(false);
}

void SelectDriverRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestSelectDrivers(&context, &request,&responder, completionQueue.get(),
                                      completionQueue.get(), this);
    }
    else if(status == PROCESS) {
        static auto geoIndexer = DriverGeoIndexer::GetDriverGeoIndexerSingleton();
        new SelectDriverRpc(service, completionQueue);
        static auto mover = geoIndexer->SelectDrivers(request.coordinate());
        response.clear_driverid();
        response.clear_drivers();
        response.clear_drivercoordinates();
        for (size_t i = 0; i < request.lvliter() * quadrantMultiplierPerLvl; i++) {
            ++mover;
            for (auto &it : mover.GetData()) {
                GetQuadrantDriver(mover, request.children(), request.animals());
            }
        }
        status = FINISH;
        responder.Finish(response, grpc::Status::OK, this);
    }
    else{
        delete this;
    }
}

void SelectDriverRpc::GetQuadrantDriver(GeoIndexerMover &mover, bool children, bool animals) {
    for (auto &it : mover.GetData()) {
        if (it.second->first.children() == children && it.second->first.animals() == animals) {
            response.add_driverid(it.first);
            auto currentDriver = response.add_drivers();
            currentDriver->set_type(it.second->first.type());
            currentDriver->set_children(it.second->first.children());
            currentDriver->set_animals(it.second->first.animals());
            auto currentDriverCoordinate = response.add_drivercoordinates();
            currentDriverCoordinate->set_latitude(it.second->second.latitude());
            currentDriverCoordinate->set_longitude(it.second->second.longitude());
        }
    }
}

GeoIndexServerImpl::GeoIndexServerImpl() : service(new MailTaxi::GeoIndex::AsyncService)
{
}

GeoIndexServerImpl::~GeoIndexServerImpl() {
    for (auto & worker : workers)
        worker.join();
}

void GeoIndexServerImpl::Run(size_t threadNum, const std::string& serverAddress) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&*service);
    completionQueue = builder.AddCompletionQueue();
    server = builder.BuildAndStart();

    new RegisterDriverRpc(service, completionQueue);
    new MoveDriverRpc(service, completionQueue);
    new SelectDriverRpc(service, completionQueue);
    new RemoveDriverRpc(service, completionQueue);


    void* tag;
    bool ok;
    while (completionQueue->Next(&tag, &ok)){
        static_cast<RpcHandler *>(tag)->Handle(false);
    }

//    for (size_t i = 0; i < threadNum; i++){
//        workers.emplace_back(std::thread([this]{
//            CDSHelper::CDSThreadInitializer threadInitializer;
//            void* tag;
//            bool ok;
//            while (completionQueue->Next(&tag, &ok)){
//                if (ok)
//                    static_cast<RpcHandler*>(tag)->Handle();
//            }
//        }));
//    }
}

void GeoIndexServerImpl::Stop() {
    server->Shutdown();
    completionQueue->Shutdown();
}
