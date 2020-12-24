#pragma once
#include "AuthorizationServerImpl.h"
#include <iostream>
#include <thread>
#include <vector>
#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include "Authorization.grpc.pb.h"
#include "CashTable.h"
class RpcHandler{
public:
    virtual void Handle(bool ok) = 0;
    virtual ~RpcHandler() = default;
};
//rpc Registration(RegistrationRequest) returns (RegistrationResponse){}
//rpc Authorization(AuthorizationRequest) returns (AuthorizationResponse){}
//rpc StartDriverWork(StartDriverWorkRequest) returns (StartDriverWorkResponse){}
//rpc EndDriverWork(EndDriverWorkRequest) returns (EndDriverWorkResponse){}
//rpc MakeOrder(MakeOrderRequest) returns (stream MakeOrderResponse){}


class RegistrationRpc : public RpcHandler{
    std::shared_ptr<MailTaxi::Authorization::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::RegistrationRequest request;
    MailTaxi::RegistrationResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::RegistrationResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ICashTable<std::string, uint64_t>> registrCashTable;
    std::shared_ptr<std::atomic_uint64_t> idCounter;
public:
    RegistrationRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService>  service,
                      std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue,
                    std::shared_ptr<ICashTable<std::string, uint64_t>> registrCashTable,
                    std::shared_ptr<std::atomic_uint64_t> idCounter);
    void Handle(bool ok) override;
    ~ RegistrationRpc() override = default;
};

class AuthorizationRpc : public RpcHandler{
    std::shared_ptr<MailTaxi::Authorization::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::AuthorizationRequest request;
    MailTaxi::AuthorizationResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::AuthorizationResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ICashTable<std::string, uint64_t>> registrCashTable;
public:
    AuthorizationRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService>  service,
                    std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue,
                     std::shared_ptr<ICashTable<std::string, uint64_t>> registrCashTable);
    void Handle(bool ok) override;
    ~AuthorizationRpc() override = default;
};

class StartDriverWorkRpc final: public RpcHandler{
    std::shared_ptr<MailTaxi::Authorization::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::StartDriverWorkRequest request;
    MailTaxi::StartDriverWorkResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::StartDriverWorkResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
public:
    StartDriverWorkRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService>  service,
                     std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue);
    void Handle(bool ok) override;
    ~StartDriverWorkRpc() override = default;
};

class AuthorizationServerImpl {
    std::shared_ptr<MailTaxi::Authorization::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    std::unique_ptr<grpc::Server> server;
    std::vector<std::thread> workers;
    std::shared_ptr<ICashTable<std::string, uint64_t>> registrCashTable;
    std::shared_ptr<std::atomic_uint64_t> idCounter;
public:
    AuthorizationServerImpl();
    ~AuthorizationServerImpl();
    void Run(size_t threadNum, const std::string& serverAddress);
    void Stop();

};
