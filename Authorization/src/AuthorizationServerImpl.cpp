#include "AuthorizationServerImpl.h"

#include <utility>
#include "Sha1.h"
AuthorizationServerImpl::AuthorizationServerImpl() : service(new MailTaxi::Authorization::AsyncService){

}

AuthorizationServerImpl::~AuthorizationServerImpl() {
    for (auto & worker : workers)
        worker.join();
}

void AuthorizationServerImpl::Run(size_t threadNum, const std::string &serverAddress) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&*service);
    completionQueue = builder.AddCompletionQueue();
    server = builder.BuildAndStart();




    void* tag;
    bool ok;
    while (completionQueue->Next(&tag, &ok)){
        static_cast<RpcHandler*>(tag)->Handle(false);
    }

}

void AuthorizationServerImpl::Stop() {
    server->Shutdown();
    completionQueue->Shutdown();
}

RegistrationRpc::RegistrationRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                                 std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                 std::shared_ptr<ICashTable<std::string, uint64_t>> registrCashTable,
                                 std::shared_ptr<std::atomic_uint64_t> idCounter) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), registrCashTable(std::move(registrCashTable)),
        idCounter(std::move(idCounter)){
        Handle(true);
}

void RegistrationRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestRegistration(&context, &request, &responder, completionQueue.get(),
                                       completionQueue.get(), this);
    }
    else if (status == PROCESS){

        new RegistrationRpc(service, completionQueue, registrCashTable, idCounter);
        auto token = std::string(sha1(request.login() + request.password()));
        auto id = idCounter->operator++();
        if (registrCashTable->Insert(token, id)) {
            response.set_token(token);
            response.set_id(id);
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

AuthorizationRpc::AuthorizationRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                                   std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                                   std::shared_ptr<ICashTable<std::string, uint64_t>> registrCashTable) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE), registrCashTable(std::move(registrCashTable)){

}

void AuthorizationRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestAuthorization(&context, &request, &responder, completionQueue.get(),
                                     completionQueue.get(), this);
    }
    else if (status == PROCESS){
        new AuthorizationRpc(service, completionQueue, registrCashTable);
        auto token = std::string(sha1(request.login() + request.password()));
        uint64_t userId = 0;
        if (registrCashTable->Find(token, [&userId](const std::string &key, uint64_t& id){
            userId = id;
        })){
            response.set_token(token);
            response.set_id(userId);
            responder.Finish(response, grpc::Status::OK, this);
        }
        else{
            responder.Finish(response, grpc::Status::OK, this);
        }
        status = FINISH;
    }
    else{
        delete this;
    }
}

StartDriverWorkRpc::StartDriverWorkRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                                       std::shared_ptr<grpc::ServerCompletionQueue> completionQueue) :
        service(std::move(service)), completionQueue(std::move(completionQueue)),
        responder(&context), status(CREATE){

}

void StartDriverWorkRpc::Handle(bool ok) {
    if (status == CREATE) {
        status = PROCESS;
        service->RequestStartDriverWork(&context, &request, &responder, completionQueue.get(),
                                      completionQueue.get(), this);
    }
    else if (status == PROCESS){

        new StartDriverWorkRpc(service, completionQueue);

        status = FINISH;
        responder.Finish(response, grpc::Status::CANCELLED, this);
    }
    else{
        delete this;
    }
}
