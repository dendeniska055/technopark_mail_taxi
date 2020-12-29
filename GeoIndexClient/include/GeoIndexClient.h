#pragma once
#include "iostream"
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>
#include "OrderManagerClient.h"
#include "GeoIndex.grpc.pb.h"
#include "functional"
struct GeoIndexRpcHandler{
public:
    virtual void Handle(bool ok) = 0;
    virtual ~GeoIndexRpcHandler() = default;
};

struct RegisterDriverRpc final : GeoIndexRpcHandler{
public:
    MailTaxi::RegisterDriverResponse response;
    grpc::ClientContext context;
    grpc::Status status;
    std::unique_ptr<grpc::ClientAsyncResponseReader<MailTaxi::RegisterDriverResponse>> responseReader;
public:
    void Handle(bool ok) override;

    ~RegisterDriverRpc() override = default;
};

struct SelectDriverRpc final : GeoIndexRpcHandler{
public:
    MailTaxi::SelectDriversResponse response;
    grpc::ClientContext context;
    grpc::Status status;
    std::unique_ptr<grpc::ClientAsyncResponseReader<MailTaxi::SelectDriversResponse>> responder;
    enum SelectDriverStatus {CREATE, PROCESS, FINISH};
    SelectDriverStatus stat;
    std::shared_ptr<std::atomic_size_t> driverId;
    std::shared_ptr<OrderManagerClient> orderManagementClient;
public:
    void Handle(bool ok) override;
    ~SelectDriverRpc() override = default;
};

struct MoveDriverRpc final : GeoIndexRpcHandler{
public:
    MailTaxi::MoveDriverResponse response;
    grpc::ClientContext context;
    grpc::Status status;
    std::unique_ptr<grpc::ClientAsyncResponseReader<MailTaxi::MoveDriverResponse>> responseReader;
public:
    void Handle(bool ok) override;
    ~MoveDriverRpc() override = default;
};

struct RemoveDriverRpc final : GeoIndexRpcHandler{
public:
    MailTaxi::RemoveDriverResponse response;
    grpc::ClientContext context;
    grpc::Status status;
    std::unique_ptr<grpc::ClientAsyncResponseReader<MailTaxi::RemoveDriverResponse>> responseReader;
public:
    void Handle(bool ok) override;
    ~RemoveDriverRpc() override = default;
};

class GeoIndexClient {
private:
    std::unique_ptr<MailTaxi::GeoIndex::Stub> stub;
    grpc::CompletionQueue completionQueue;
public:
    explicit GeoIndexClient(std::shared_ptr<grpc::Channel> channel);
    virtual ~GeoIndexClient() = default;
    void CompleteRpcRun();
    void SelectDriver(const MailTaxi::Coordinate &coordinate, bool children, bool animals, uint64_t lvlIter,
                      std::shared_ptr<std::atomic_size_t> driverId,
                      std::shared_ptr<OrderManagerClient> orderManagementClient);
    void RegisterDriver(uint64_t driverId, const MailTaxi::Coordinate &coordinate, MailTaxi::Driver& driver);
    void MoveDriver(const MailTaxi::Coordinate &coordinate, uint64_t driverId);
    void RemoveDriver(uint64_t driverId);
};
typedef std::shared_ptr<GeoIndexClient> GeoIndexClientPtr;