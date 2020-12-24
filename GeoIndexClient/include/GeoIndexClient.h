#pragma once
#include "iostream"
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>

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
public:
    void Handle(bool ok) override{
//        if (stat == PROCESS){
//            static int counter = 0;
//            if (counter < 5){
//                responder->Read(&response, static_cast<void*>(this));
//                std::cout << "прочитал" << std::endl;
//            }
//            else{
//                stat = FINISH;
//                responder->Finish(&status, static_cast<void*>(this));
//            }
//        }
//        else{
//            delete this;
//        }

    };
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
    void Handle(bool ok) override{
        std::cout << response.driverfound() << std::endl;
    };
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
    void SelectDriverRpc(const MailTaxi::Coordinate &coordinate, bool children, bool animals);
//    void RegisterDriver(MailTaxi::RegisterDriverRequest &request);
    void MoveDriver(const MailTaxi::Coordinate &coordinate, uint64_t driverId);
//    void RemoveDriver(MailTaxi::RemoveDriverRequest &request);
};