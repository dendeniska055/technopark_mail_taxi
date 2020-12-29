#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include "DriverGeoIndexer.h"
#include "GeoIndex.grpc.pb.h"

class RpcHandler{
public:
    virtual void Handle(bool ok) = 0;
    virtual ~RpcHandler() = default;
};

class RegisterDriverRpc final : public RpcHandler{
private:
    std::shared_ptr<MailTaxi::GeoIndex::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::RegisterDriverRequest request;
    MailTaxi::RegisterDriverResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::RegisterDriverResponse> responseWriter;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;

public:
    RegisterDriverRpc(std::shared_ptr<MailTaxi::GeoIndex::AsyncService>  service,
                      std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue);
    void Handle(bool ok) override;
    ~RegisterDriverRpc() override = default;
};

class MoveDriverRpc final : public RpcHandler{
private:
    std::shared_ptr<MailTaxi::GeoIndex::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::MoveDriverRequest request;
    MailTaxi::MoveDriverResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::MoveDriverResponse> responseWriter;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;

public:
    MoveDriverRpc(std::shared_ptr<MailTaxi::GeoIndex::AsyncService>  service,
                      std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue);
    void Handle(bool ok) override;
    ~MoveDriverRpc() override = default;
};

class RemoveDriverRpc final : public RpcHandler{
private:
    std::shared_ptr<MailTaxi::GeoIndex::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::RemoveDriverRequest request;
    MailTaxi::RemoveDriverResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::RemoveDriverResponse> responseWriter;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;

public:
    RemoveDriverRpc(std::shared_ptr<MailTaxi::GeoIndex::AsyncService>  service,
                  std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue);
    void Handle(bool ok) override;
    ~RemoveDriverRpc() override = default;
};

class SelectDriverRpc final : public RpcHandler{
private:
    std::shared_ptr<MailTaxi::GeoIndex::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    grpc::ServerContext context;
    MailTaxi::SelectDriversRequest request;
    MailTaxi::SelectDriversResponse response;
    grpc::ServerAsyncResponseWriter<MailTaxi::SelectDriversResponse> responder;
    enum RegisterDriverStatus {CREATE, PROCESS, FINISH};
    RegisterDriverStatus status;
    static const size_t quadrantMultiplierPerLvl = 8;
private:
    void GetQuadrantDriver(GeoIndexerMover &mover, bool children, bool animals);
public:
    SelectDriverRpc(std::shared_ptr<MailTaxi::GeoIndex::AsyncService>  service,
                    std::shared_ptr<grpc::ServerCompletionQueue>  completionQueue);
    void Handle(bool ok) override;
    ~SelectDriverRpc() override = default;
};

class GeoIndexServerImpl final{
private:
    std::shared_ptr<MailTaxi::GeoIndex::AsyncService> service;
    std::shared_ptr<grpc::ServerCompletionQueue> completionQueue;
    std::unique_ptr<grpc::Server> server;
    std::vector<std::thread> workers;
public:
    GeoIndexServerImpl();
    ~GeoIndexServerImpl();
    void Run(size_t threadNum, const std::string& serverAddress);
    void Stop();
};

