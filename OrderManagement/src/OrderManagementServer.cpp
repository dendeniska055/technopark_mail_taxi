#include "OrderManagementServer.h"

#include <utility>
#include "CDSHelper.h"

OrderManagementServerImp::OrderManagementServerImp(std::shared_ptr<ConnectionManagementCash> connectionManagementCash,
                                                   std::shared_ptr<InitClientCashTable> initClientCash)
                                                   :service(new MailTaxi::OrderManager::AsyncService),
                                                   connectionManagementCash(std::move(connectionManagementCash)),
                                                   initClientCash(std::move(initClientCash)){
}

void OrderManagementServerImp::Run(size_t threadNum, const std::string &serverAddress) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&*service);
    completionQueue = builder.AddCompletionQueue();
    server = builder.BuildAndStart();

    new CreatingDriverWorkRpc(service, completionQueue, connectionManagementCash);
    new CloseDriverWorkRpc(service, completionQueue, connectionManagementCash);
    new ProposeOrderRpc(service, completionQueue, connectionManagementCash);
    new CreateOrderRpc(service, completionQueue, initClientCash);
    new RemoveOrderRpc(service, completionQueue, initClientCash,
                       connectionManagementCash);

    void* tag;
    bool ok;
    while (true){
        completionQueue->Next(&tag, &ok);
        static_cast<RpcHandler*>(tag)->Handle(ok);
    }
//    for(size_t i = 0; i < threadNum; i++){
//        CDSHelper::CDSThreadInitializer threadInitializer;
//        workers.emplace_back(std::thread([this](){
//            void* tag;
//            bool ok;
//            while (completionQueue->Next(&tag, &ok)){
//                static_cast<RpcHandler *>(tag)->Handle(ok);
//            }
//        }));
//    }
}

OrderManagementServerImp::~OrderManagementServerImp() {
    for (auto & worker : workers)
        worker.join();
}

void OrderManagementServerImp::Stop() {
    server->Shutdown();
    completionQueue->Shutdown();
}


CreatingDriverWorkRpc::CreatingDriverWorkRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
                                             std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                             std::shared_ptr<ConnectionManagementCash> connManagementCash) :
        service(std::move(service)),completionQueue(std::move(completionQueue)),
        responseWriter(&context), status(CREATE), connectionManagementCash(std::move(connManagementCash)){
    Handle(false);
}

void CreatingDriverWorkRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestCreatingDriverWork(&context, &request, &responseWriter, completionQueue.get(),
                                   completionQueue.get(), this);
    }
    else if(status == PROCESS){
        new CreatingDriverWorkRpc(service, completionQueue, connectionManagementCash);
        bool success =
                connectionManagementCash->CreateEmptyConnectionManagementData(request.driverid(),
                                                                              request.drivertoken());
        response.set_success(success);
        status = FINISH;
        responseWriter.Finish(response, grpc::Status::OK, this);
    }
    else{
        delete this;
    }
}

CloseDriverWorkRpc::CloseDriverWorkRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
                                       std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                       std::shared_ptr<ConnectionManagementCash> connManagementCash) :
        service(std::move(service)),completionQueue(std::move(completionQueue)),
        responseWriter(&context), status(CREATE), connectionManagementCash(std::move(connManagementCash)){
    Handle(false);
}

void CloseDriverWorkRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestCloseDriverWork(&context, &request, &responseWriter, completionQueue.get(),
                                           completionQueue.get(), this);
    }
    else if(status == PROCESS){
        new CloseDriverWorkRpc(service, completionQueue, connectionManagementCash);
        bool success = connectionManagementCash->DeleteConnectionData(request.driverid());
        response.set_success(success);
        status = FINISH;
        responseWriter.Finish(response, grpc::Status::OK, this);
    }
    else{
        delete this;
    }
}

CreateOrderRpc::CreateOrderRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
                               std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                               std::shared_ptr<InitClientCashTable> clientCashTable) :
        service(std::move(service)),completionQueue(std::move(completionQueue)),
        responseWriter(&context), status(CREATE), clientCashTable(std::move(clientCashTable))
{
    Handle(false);
}

void CreateOrderRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestCreateOrder(&context, &request, &responseWriter, completionQueue.get(),
                                        completionQueue.get(), this);
    }
    else if(status == PROCESS){
        new CreateOrderRpc(service, completionQueue, clientCashTable);
        InitClientData data;
        data.driverId = request.driverid();
        data.token = request.clienttoken();
        bool success = clientCashTable->CreateInitClientData(request.ordernum(), data);
        response.set_success(success);
        status = FINISH;
        responseWriter.Finish(response, grpc::Status::OK, this);
    }
    else{
        delete this;
    }
}

RemoveOrderRpc::RemoveOrderRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
                               std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                               std::shared_ptr<InitClientCashTable> clientCashTable,
                               std::shared_ptr<ConnectionManagementCash> connectionManagementCash) :
        service(std::move(service)),completionQueue(std::move(completionQueue)),
        responseWriter(&context), status(CREATE), clientCashTable(std::move(clientCashTable)),
        connectionManagementCash(std::move(connectionManagementCash)){
    Handle(false);
}

void RemoveOrderRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestRemoveOrder(&context, &request, &responseWriter, completionQueue.get(),
                                    completionQueue.get(), this);
    }
    else if(status == PROCESS){
        new RemoveOrderRpc(service, completionQueue, clientCashTable, connectionManagementCash);
        bool success = clientCashTable->DeleteClientData(request.ordernum());
        connectionManagementCash->DeleteClientSession(clientCashTable->GetDriverId(request.ordernum()));
        response.set_success(success);
        status = FINISH;
        responseWriter.Finish(response, grpc::Status::OK, this);
    }
    else{
        delete this;
    }
}

ProposeOrderRpc::ProposeOrderRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
                                 std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                 std::shared_ptr<ConnectionManagementCash> connectionManagementCash) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), connectionManagementCash(std::move(connectionManagementCash)){
    finishIndicator.store(false);
    driverMsgSended = false;
    Handle(true);
}

void ProposeOrderRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestProposeOrder(&context, &request, &responder, completionQueue.get(),
                                     completionQueue.get(), this);
    } else if (status == PROCESS) {
        if (!driverMsgSended) {
            new ProposeOrderRpc(service, completionQueue, connectionManagementCash);
            status = FINISH;
            response.set_driverid(request.driverid());
            response.set_success(false);
            auto driverInfo = connectionManagementCash->GetAndCheckDriverSession(request.driverid());
            driverMsgSended = true;
            if (driverInfo.first) {
                driverInfo.second->SendOrderInformation(request.orderlocationdata(), [this](beast::error_code err) {
                    if (!err)
                        response.set_success(true);
                    else
                        connectionManagementCash->DriverDisConnect(request.driverid());
                    finishIndicator.store(true);
                });
            } else
                finishIndicator.store(true);
        } else {
            if (finishIndicator)
                responder.Finish(response, grpc::Status::OK, this);
        }
    } else {
        delete this;
    }
}
//    if (status == CREATE) {
//        status = PROCESS;
//        service->RequestProposeOrder(&context, &responder, completionQueue.get(),
//                                    completionQueue.get(), this);
//    }
//    else if(status == PROCESS){
//        if (!proposeOrderRpcCreated) {
//            new ProposeOrderRpc(service, completionQueue, connectionManagementCash);
//            this->proposeOrderRpcCreated = true;
//        }
//        if (ok){
//            RequiredForCompletionDriverCount++;
//            response.add_driverid(request.driverid());
//            response.add_success(false);
//            auto index = response.driverid_size();
//            auto driverInfo = connectionManagementCash->GetAndCheckDriverSession(request.driverid());
//            if (driverInfo.first){
//                std::cout << "удачный запрос!" << std::endl;
//                auto driverSession = driverInfo.second;
//                if (!driverSession->PartnerOnLine()){
//                    driverSession->SendOrderInformation(request.orderlocationdata(), [this, index]
//                    (beast::error_code err){
//                        if (!err)
//                            response.set_success(index, true);
//                        else
//                            connectionManagementCash->DriverDisConnect(response.driverid(index));
//                        driverCount++;
//                    });
//                }
//                else{
//                    driverCount++;
//                }
//            }
//            else{
//                driverCount++;
//            }
//            this->responder.Read(&request, static_cast<void*>(this));
//        }
//        else{
//            if (RequiredForCompletionDriverCount == driverCount.load()) {
//                std::cout << "Отправил ответ!" << std::endl;
//                status = FINISH;
//                responder.Finish(response, grpc::Status(), this);
//            }
//        }
//    }
//    else{
//        std::cout << "удалился" << std::endl;
//        delete this;
//    }

