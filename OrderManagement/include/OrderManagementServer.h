#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include "DriverGeoIndexer.h"
#include "OrderManagement.grpc.pb.h"
#include "ConnectionManagementCash.h"
#include "InitClientCashTable.h"



class RpcHandler{
public:
    virtual void Handle(bool ok) = 0;
    virtual ~RpcHandler() = default;
};

class CreatingDriverWorkRpc final : public RpcHandler{
private:
    std::shared_ptr<MailTaxi::OrderManager::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::CreatingDriverWorkRequest request;
    MailTaxi::CreatingDriverWorkResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::CreatingDriverWorkResponse> responseWriter;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ConnectionManagementCash> connectionManagementCash;
public:
    CreatingDriverWorkRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
                          std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                          std::shared_ptr<ConnectionManagementCash> connManagementCash);
    void Handle(bool ok) override;
    ~CreatingDriverWorkRpc() override = default;
};

class CloseDriverWorkRpc final : public RpcHandler{
private:
    std::shared_ptr<MailTaxi::OrderManager::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi:: CloseDriverWorkRequest request;
    MailTaxi::CloseDriverWorkResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::CloseDriverWorkResponse> responseWriter;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ConnectionManagementCash> connectionManagementCash;
public:
    CloseDriverWorkRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
                       std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                       std::shared_ptr<ConnectionManagementCash> connManagementCash);
    void Handle(bool ok) override;
    ~CloseDriverWorkRpc() override = default;
};


class CreateOrderRpc final : public RpcHandler{
private:
    std::shared_ptr<MailTaxi::OrderManager::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi:: CreateOrderRequest request;
    MailTaxi::CreateOrderResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::CreateOrderResponse> responseWriter;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<InitClientCashTable> clientCashTable;
public:
    CreateOrderRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
                       std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                   std::shared_ptr<InitClientCashTable> clientCashTable);
    void Handle(bool ok) override;
    ~CreateOrderRpc() override = default;
};

class RemoveOrderRpc final  : public RpcHandler{
private:
    std::shared_ptr<MailTaxi::OrderManager::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::RemoveOrderRequest request;
    MailTaxi::RemoveOrderResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi:: RemoveOrderResponse> responseWriter;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<InitClientCashTable> clientCashTable;
    std::shared_ptr<ConnectionManagementCash> connectionManagementCash;
public:
    RemoveOrderRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
            std::shared_ptr<InitClientCashTable> clientCashTable,
                   std::shared_ptr<ConnectionManagementCash> connectionManagementCash);
    void Handle(bool ok) override;
    ~RemoveOrderRpc() override = default;
};

class ProposeOrderRpc final  : public RpcHandler{
private:
    std::shared_ptr<MailTaxi::OrderManager::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::ProposeOrderRequest request;
    MailTaxi::ProposeOrderResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::ProposeOrderResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    std::shared_ptr<ConnectionManagementCash> connectionManagementCash;
    std::atomic_bool finishIndicator;
    bool driverMsgSended;
public:
    ProposeOrderRpc(std::shared_ptr<MailTaxi::OrderManager::AsyncService> service,
                   std::shared_ptr<grpc::ServerCompletionQueue> completionQueue,
                    std::shared_ptr<ConnectionManagementCash> connectionManagementCash);
    void Handle(bool ok) override;
    ~ProposeOrderRpc() override = default;
};

class OrderManagementServerImp final{
private:
    std::shared_ptr<MailTaxi::OrderManager::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    std::unique_ptr<grpc::Server> server;
    std::vector<std::thread> workers;
    std::shared_ptr<ConnectionManagementCash> connectionManagementCash;
    std::shared_ptr<InitClientCashTable> initClientCash;
public:
    OrderManagementServerImp(std::shared_ptr<ConnectionManagementCash> connectionManagementCash,
                                 std::shared_ptr<InitClientCashTable> initClientCash);
    ~OrderManagementServerImp();
    void Run(size_t threadNum, const std::string& serverAddress);
    void Stop();
};
