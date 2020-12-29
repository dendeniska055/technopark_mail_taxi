#pragma once
#include "OrderManagement.grpc.pb.h"
#include "iostream"
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>

//struct OrderManagementRpcHandler{
//public:
//    virtual void Handle(bool ok) = 0;
//    virtual ~OrderManagementRpcHandler() = default;
//};
//
//class CreatingDriverWorkRpcClient final : OrderManagementRpcHandler{
//public:
//    MailTaxi::CreatingDriverWorkResponse response;
//    grpc::ClientContext context;
//    grpc::Status status;
//    std::unique_ptr<grpc::ClientAsyncResponseReader<MailTaxi::CreatingDriverWorkResponse>> responseReader;
//public:
//    void Handle(bool ok) override{
//
//    };
//    ~CreatingDriverWorkRpcClient() override = default;
//};
//
//class CloseDriverWorkRpcClient final : OrderManagementRpcHandler{
//public:
//    MailTaxi::CloseDriverWorkResponse response;
//    grpc::ClientContext context;
//    grpc::Status status;
//    std::unique_ptr<grpc::ClientAsyncResponseReader<MailTaxi::CloseDriverWorkResponse>> responseReader;
//public:
//    void Handle(bool ok) override{
//
//    };
//    ~CloseDriverWorkRpcClient() override = default;
//};
//
//class CreateOrderRpcClient final : OrderManagementRpcHandler{
//public:
//    MailTaxi::CreateOrderResponse response;
//    grpc::ClientContext context;
//    grpc::Status status;
//    std::unique_ptr<grpc::ClientAsyncResponseReader<MailTaxi::CreateOrderResponse>> responseReader;
//public:
//    void Handle(bool ok) override{
//
//    };
//    ~CreateOrderRpcClient() override = default;
//};
//
//class ProposeOrderRpcClient final : OrderManagementRpcHandler{
//public:
//    MailTaxi::ProposeOrderResponse response;
//    grpc::ClientContext context;
//    grpc::Status status;
//    std::unique_ptr<grpc::ClientAsyncResponseReader<MailTaxi::ProposeOrderResponse>> responseReader;
//public:
//    void Handle(bool ok) override{
//
//    };
//    ~ProposeOrderRpcClient() override = default;
//};
//
//class OrderManagerClient {
//private:
//    std::unique_ptr<MailTaxi::OrderManager::Stub> stub;
//    grpc::CompletionQueue completionQueue;
//public:
//    OrderManagerClient(std::shared_ptr<grpc::Channel> channel);
//    void CreatingDriverWork(uint64_t driverId, const std::string& driverToken);
//    void  CloseDriverWork(uint64_t driverId);
//    void CreateOrder(uint64_t orderNum, uint64_t driverId,const  std::string& clientToken);
//    void ProposeOrder(uint64_t driverId, const MailTaxi::OrderLocationData& locationData);
//    void CompleteRpcRun();
//};

class OrderManagerClient {
private:
    std::unique_ptr<MailTaxi::OrderManager::Stub> stub;
public:
    explicit OrderManagerClient(std::shared_ptr<grpc::Channel> channel);
    MailTaxi::CreatingDriverWorkResponse CreatingDriverWork(uint64_t driverId, const std::string& driverToken);
    MailTaxi::CloseDriverWorkResponse CloseDriverWork(uint64_t driverId);
    MailTaxi::CreateOrderResponse CreateOrder(uint64_t orderNum, uint64_t driverId,const  std::string& clientToken);
    MailTaxi::ProposeOrderResponse
    ProposeOrder(uint64_t driverId, MailTaxi::OrderLocationData &locationData, uint64_t orderNum);
    MailTaxi::RemoveOrderResponse RemoveOrder(uint64_t orderNum);
};

