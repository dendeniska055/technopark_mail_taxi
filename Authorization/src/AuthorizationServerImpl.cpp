#include "AuthorizationServerImpl.h"

#include <utility>
#include <string>
#include "Sha1.h"
#include "DataBase.h"
#include "CDSHelper.h"
AuthorizationServerImpl::AuthorizationServerImpl() : service(new MailTaxi::Authorization::AsyncService){

}

AuthorizationServerImpl::~AuthorizationServerImpl() {
    for (auto & worker : workers)
        worker.join();
}

void AuthorizationServerImpl::Run(size_t threadNum, const std::string &serverAddress,
                                  std::shared_ptr<GeoIndexClientSync> geoIndexCLient,
                                  std::shared_ptr<OrderManagerClient> orderManagerClient) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&*service);
    completionQueue = builder.AddCompletionQueue();
    server = builder.BuildAndStart();
    static std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTable(new CashTableLockFree<int64_t, std::string>);\

    new RegistrationDriverRpc(service, completionQueue, tokenCashTable);
    new RegistrationClientRpc(service, completionQueue, tokenCashTable);
    new AuthorizationRpc(service, completionQueue, tokenCashTable);
    new StartDriverWorkRpc(service, completionQueue, tokenCashTable, geoIndexCLient, orderManagerClient);
    new EndDriverWorkRpc(service, completionQueue,tokenCashTable, geoIndexCLient, orderManagerClient);
    new MakeOrderRpc(service, completionQueue,tokenCashTable, geoIndexCLient, orderManagerClient);
    new OrderCompletedRpc(service, completionQueue,tokenCashTable, geoIndexCLient, orderManagerClient);
    void *tag;
    bool ok;
//    while (completionQueue->Next(&tag, &ok)) {
//        static_cast<RpcHandler *>(tag)->Handle(false);
//    }
    for (size_t i = 0; i < threadNum; i++) {
        workers.emplace_back([this](){
            CDSHelper::CDSThreadInitializer threadInitializer;
            void *tag;
            bool ok;
            while (completionQueue->Next(&tag, &ok)) {
                static_cast<RpcHandler *>(tag)->Handle(ok);
            }
        });
    }
}

void AuthorizationServerImpl::Stop() {
    server->Shutdown();
    completionQueue->Shutdown();
}

RegistrationDriverRpc::RegistrationDriverRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                                             std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                             std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTable) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), tokenCashTAble(std::move(tokenCashTable)){
        Handle(true);
}

void RegistrationDriverRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestRegistrationDriver(&context, &request, &responder, completionQueue.get(),
                                       completionQueue.get(), this);
    }
    else if (status == PROCESS){
        new RegistrationDriverRpc(service, completionQueue, tokenCashTAble);
        auto registrationManager = RegistrationManager::GetRegistrationManagerSingleton();
        registrationManager->RegistrationDriver(request.name(), request.carmark(), request.login(),
                                                request.password(),
                                                request.driverinfo());
        uint64_t driverId = registrationManager->DriverLogin(request.login(), request.password());
        auto token = std::string(sha1(request.login() + request.password() + std::to_string(std::rand()) +
                std::to_string(driverId)));
        tokenCashTAble->Insert(driverId, token);
        response.set_id(driverId);
        response.set_token(token);
        responder.Finish(response, grpc::Status::OK, this);
        status = FINISH;
    }
    else{
        delete this;
    }
}

AuthorizationRpc::AuthorizationRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                                   std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                   std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTable) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), tokenCashTAble(std::move(tokenCashTable)){
    Handle(true);
}

void AuthorizationRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestAuthorization(&context, &request, &responder, completionQueue.get(),
                                     completionQueue.get(), this);
    }
    else if (status == PROCESS){
        new AuthorizationRpc(service, completionQueue, tokenCashTAble);
        auto registrationManager = RegistrationManager::GetRegistrationManagerSingleton();
        uint64_t id = 0;
        std::string token;
        if (request.usertype() == MailTaxi::driver){
           id = registrationManager->DriverLogin(request.login(), request.password());
           tokenCashTAble->Find(id, [&token](const int64_t& key, std::string &tok){
               token = tok;
           });
        }
        else{
            id = registrationManager->ClientLogin(request.login(), request.password());
            tokenCashTAble->Find(-id, [&token](const int64_t& key, std::string &tok){
                token = tok;
            });
        }
        if (id > 0){
            if (token.empty()){
                token = std::string(sha1(request.login() + request.password() + std::to_string(std::rand()) +
                                         std::to_string(id)));
                if (request.usertype() == MailTaxi::driver)
                    tokenCashTAble->Insert(id, token);
                else
                    tokenCashTAble->Insert(-id, token);
            }
            response.set_id(id);
            response.set_token(token);
            responder.Finish(response,grpc::Status::OK, static_cast<void*>(this));
        }
        else{
            responder.Finish(response,grpc::Status::CANCELLED, static_cast<void*>(this));
        }
        status = FINISH;
    }
    else{
        delete this;
    }
}

StartDriverWorkRpc::StartDriverWorkRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                                       std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                       std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble,
                                       std::shared_ptr<GeoIndexClientSync> geoIndexClient,
                                       std::shared_ptr<OrderManagerClient> orderManagerClient) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), tokenCashTAble(std::move(tokenCashTAble)), geoIndexClient(std::move(geoIndexClient)),
        orderManagerClient(std::move(orderManagerClient)){
    Handle(true);
}

void StartDriverWorkRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestStartDriverWork(&context, &request, &responder, completionQueue.get(),
                                      completionQueue.get(), this);
    }
    else if (status == PROCESS){
        new StartDriverWorkRpc(service, completionQueue, tokenCashTAble, geoIndexClient, orderManagerClient);
        bool success = false;
        tokenCashTAble->Find(request.id(), [this, &success](const uint64_t&, std::string& token){
            if (request.token() == token)
                success = true;
        });
        response.set_success(success);
        if (success) {
            auto registrationManager = RegistrationManager::GetRegistrationManagerSingleton();
            MailTaxi::Driver driver;
            registrationManager->GetDriverInformation(request.id(), driver);
            geoIndexClient->RegisterDriver(request.id(), request.coordinate(), driver);
            orderManagerClient->CreatingDriverWork(request.id(), request.token());
            responder.Finish(response, grpc::Status::OK, this);
        }
        else{
            responder.Finish(response, grpc::Status::CANCELLED, this);
        }
        status = FINISH;
    }
    else{
        delete this;
    }
}

RegistrationClientRpc::RegistrationClientRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                                             std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                             std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTable):
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), tokenCashTAble(std::move(tokenCashTable)) {
    Handle(true);
}

void RegistrationClientRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestRegistrationClient(&context, &request, &responder, completionQueue.get(),
                                           completionQueue.get(), this);
    }
    else if (status == PROCESS){
        new RegistrationClientRpc(service, completionQueue, tokenCashTAble);
        auto registrationManager = RegistrationManager::GetRegistrationManagerSingleton();
        registrationManager->RegistrationClient(request.name(),request.login(), request.password());
        uint64_t clientId = registrationManager->ClientLogin(request.login(), request.password());
        auto token = std::string(sha1(request.login() + request.password() + std::to_string(std::rand()) +
                                      std::to_string(clientId)));
        tokenCashTAble->Insert(-clientId, token);
        response.set_id(clientId);
        response.set_token(token);
        responder.Finish(response, grpc::Status::OK, this);
        status = FINISH;
    }
    else{
        delete this;
    }
}

EndDriverWorkRpc::EndDriverWorkRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                                   std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                   std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble,
                                   std::shared_ptr<GeoIndexClientSync> geoIndexClient,
                                   std::shared_ptr<OrderManagerClient> orderManagerClient) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), tokenCashTAble(std::move(tokenCashTAble)), geoIndexClient(std::move(geoIndexClient)),
        orderManagerClient(std::move(orderManagerClient)){
    Handle(true);
}

void EndDriverWorkRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestEndDriverWork(&context, &request, &responder, completionQueue.get(),
                                        completionQueue.get(), this);
    }
    else if (status == PROCESS){
        new EndDriverWorkRpc(service, completionQueue, tokenCashTAble,
                             geoIndexClient, orderManagerClient);
        bool success = false;
        tokenCashTAble->Find(request.id(), [this, &success](const uint64_t&, std::string& token){
            if (request.token() == token)
                success = true;
        });
        if (success) {
            orderManagerClient->CloseDriverWork(request.id());
            geoIndexClient->RemoveDriver(request.id());
            responder.Finish(response, grpc::Status::OK, this);
        }
        else{
            responder.Finish(response, grpc::Status::CANCELLED, this);
        }
        status = FINISH;
    }
    else{
        delete this;
    }
}

MakeOrderRpc::MakeOrderRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                           std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                           std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble,
                           std::shared_ptr<GeoIndexClientSync> geoIndexClient,
                           std::shared_ptr<OrderManagerClient> orderManagerClient) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), tokenCashTAble(std::move(tokenCashTAble)), geoIndexClient(std::move(geoIndexClient)),
        orderManagerClient(std::move(orderManagerClient)){
    Handle(true);
}

uint64_t CreateOrderNum(){
    static std::mutex mtx;
    static size_t orderNum = 0;
    int res = 0;
    mtx.lock();
    res = ++orderNum;
    mtx.unlock();
    return res;
};

void MakeOrderRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestMakeOrder(&context, &request, &responder, completionQueue.get(),
                                      completionQueue.get(), this);
    }
    else if (status == PROCESS){
        static bool requestMade = false;
        static size_t lvlIter = 4;
        new MakeOrderRpc(service, completionQueue, tokenCashTAble,
                         geoIndexClient, orderManagerClient);
        bool success = false;
        tokenCashTAble->Find(-  request.id(), [this, &success](const uint64_t &, std::string &token) {
            if (request.token() == token)
                success = true;
        });
        auto orderNum_ = CreateOrderNum();
        if (!success){
            responder.Finish(response, grpc::Status::CANCELLED, this);
            status = FINISH;
        }
        else{
            auto selectResponse = geoIndexClient->SelectDriver(request.locationdata().clientaddress(),
                                                               request.children(), request.animal(), 4);
            if (selectResponse.driverid().empty()){
                responder.Finish(response, grpc::Status::CANCELLED, this);
                status = FINISH;
            }
            else{
                size_t driverId = 0;
                MailTaxi::Coordinate coordinate;
                for (size_t i = 0; i < selectResponse.driverid().size(); i++){
                    MailTaxi::OrderLocationData locationData = request.locationdata();
                    auto proposeOrderResponse = orderManagerClient->ProposeOrder(
                            selectResponse.driverid(i),
                            locationData, orderNum_);
                    if (proposeOrderResponse.success()) {
                        driverId = selectResponse.driverid(i);
                        coordinate = selectResponse.drivercoordinates(i);
                        break;
                    }
                }
                if (!driverId){
                    responder.Finish(response, grpc::Status::CANCELLED, this);
                    status = FINISH;
                }
                else{
                    orderManagerClient->CreateOrder(orderNum_, driverId, request.token());
                    response.set_ordernum(orderNum_);
                    response.set_allocated_coordinate(new MailTaxi::Coordinate(coordinate));
                    response.set_price(200);
                    auto registrationManager = RegistrationManager::GetRegistrationManagerSingleton();
                    registrationManager->GetDriverInformationForClientResponse(driverId, response);
                    responder.Finish(response, grpc::Status::OK, this);
                    status = FINISH;
                }
            }
        }
    }
    else{
        delete this;
    }
}

OrderCompletedRpc::OrderCompletedRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                                     std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                     std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble,
                                     std::shared_ptr<GeoIndexClientSync> geoIndexClient,
                                     std::shared_ptr<OrderManagerClient> orderManagerClient) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), tokenCashTAble(std::move(tokenCashTAble)), geoIndexClient(std::move(geoIndexClient)),
        orderManagerClient(std::move(orderManagerClient)){
Handle(true);
}

void OrderCompletedRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestOrderCompleted(&context, &request, &responder, completionQueue.get(),
                                      completionQueue.get(), this);
    }
    else if (status == PROCESS){
        new OrderCompletedRpc(service, completionQueue, tokenCashTAble,
                             geoIndexClient, orderManagerClient);
        bool success = false;
        tokenCashTAble->Find(request.driverid(), [this, &success](const uint64_t&, std::string& token){
            if (request.token() == token)
                success = true;
        });
        if (success) {
            auto resp = orderManagerClient->RemoveOrder(request.ordernum());
            response.set_success(resp.success());
            responder.Finish(response, grpc::Status::OK, this);
        }
        else{
            responder.Finish(response, grpc::Status::CANCELLED, this);
        }
        status = FINISH;
    }
    else{
        delete this;
    }
}
