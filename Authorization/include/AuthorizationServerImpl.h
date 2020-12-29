#pragma once
#include "AuthorizationServerImpl.h"
#include "GeoIndexClient.h"
#include "OrderManagerClient.h"
#include <iostream>
#include <thread>
#include <vector>
#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include "Authorization.grpc.pb.h"
#include "CashTable.h"
#include "GeoIndexClientSync.h"
class RpcHandler{
public:
    virtual void Handle(bool ok) = 0;
    virtual ~RpcHandler() = default;
};

class RegistrationDriverRpc : public RpcHandler{
    std::shared_ptr<MailTaxi::Authorization::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::RegistrationDriverRequest request;
    MailTaxi::RegistrationDriverResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::RegistrationDriverResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble;
public:
    RegistrationDriverRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                          std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                          std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTable);
    void Handle(bool ok) override;
    ~RegistrationDriverRpc() override = default;
};

class RegistrationClientRpc : public RpcHandler{
    std::shared_ptr<MailTaxi::Authorization::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::RegistrationClientRequest request;
    MailTaxi::RegistrationClientResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::RegistrationClientResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble;
public:
    RegistrationClientRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService> service,
                          std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                          std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTable);
    void Handle(bool ok) override;
    ~RegistrationClientRpc() override = default;
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
    std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble;
public:
    AuthorizationRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService>  service,
                    std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue,
                     std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTable);
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
    std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble;
    std::shared_ptr<GeoIndexClientSync> geoIndexClient;
    std::shared_ptr<OrderManagerClient> orderManagerClient;
public:
    StartDriverWorkRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService>  service,
                     std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue,
                       std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble,
                       std::shared_ptr<GeoIndexClientSync> geoIndexClient,
                       std::shared_ptr<OrderManagerClient> orderManagerClient
                       );
    void Handle(bool ok) override;
    ~StartDriverWorkRpc() override = default;
};

class EndDriverWorkRpc final: public RpcHandler{
private:
    std::shared_ptr<MailTaxi::Authorization::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::EndDriverWorkRequest request;
    MailTaxi::EndDriverWorkResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::EndDriverWorkResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble;
    std::shared_ptr<GeoIndexClientSync> geoIndexClient;
    std::shared_ptr<OrderManagerClient> orderManagerClient;
public:
    EndDriverWorkRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService>  service,
                       std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue,
                       std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble,
                       std::shared_ptr<GeoIndexClientSync> geoIndexClient,
                       std::shared_ptr<OrderManagerClient> orderManagerClient
    );
    void Handle(bool ok) override;
    ~EndDriverWorkRpc() override = default;
};

class MakeOrderRpc final: public RpcHandler{
private:
    std::shared_ptr<MailTaxi::Authorization::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::MakeOrderRequest request;
    MailTaxi::MakeOrderResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::MakeOrderResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble;
    std::shared_ptr<GeoIndexClientSync> geoIndexClient;
    std::shared_ptr<OrderManagerClient> orderManagerClient;
public:
    MakeOrderRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService>  service,
                     std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue,
                     std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble,
                     std::shared_ptr<GeoIndexClientSync> geoIndexClient,
                     std::shared_ptr<OrderManagerClient> orderManagerClient
    );
    void Handle(bool ok) override;
    ~MakeOrderRpc() override = default;
};

class OrderCompletedRpc final: public RpcHandler{
private:
    std::shared_ptr<MailTaxi::Authorization::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::OrderCompletedRequest request;
    MailTaxi::OrderCompletedResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::OrderCompletedResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble;
    std::shared_ptr<GeoIndexClientSync> geoIndexClient;
    std::shared_ptr<OrderManagerClient> orderManagerClient;
public:
    OrderCompletedRpc(std::shared_ptr<MailTaxi::Authorization::AsyncService>  service,
                 std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue,
                 std::shared_ptr<ICashTable<int64_t, std::string>> tokenCashTAble,
                 std::shared_ptr<GeoIndexClientSync> geoIndexClient,
                 std::shared_ptr<OrderManagerClient> orderManagerClient
    );
    void Handle(bool ok) override;
    ~OrderCompletedRpc() override = default;
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
    void Run(size_t threadNum, const std::string &serverAddress,
             std::shared_ptr<GeoIndexClientSync> geoIndexCLient,
             std::shared_ptr<OrderManagerClient> orderManagerClient);
    void Stop();
};
