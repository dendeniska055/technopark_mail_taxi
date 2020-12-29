#include "OrderManagerClient.h"

//void OrderManagerClient::CloseDriverWork(uint64_t driverId) {
//    MailTaxi::CloseDriverWorkRequest request;
//    auto closeDriverWorkRpc = new CloseDriverWorkRpcClient;
//    closeDriverWorkRpc->responseReader = stub->PrepareAsyncCloseDriverWork(
//            &closeDriverWorkRpc->context, request, &completionQueue
//    );
//    request.set_driverid(driverId);
//    closeDriverWorkRpc->responseReader->StartCall();
//    closeDriverWorkRpc->responseReader->Finish(&closeDriverWorkRpc->response, &closeDriverWorkRpc->status,
//                                               static_cast<void*>(closeDriverWorkRpc));
//}
//
//void OrderManagerClient::CreatingDriverWork(uint64_t driverId, const std::string &driverToken) {
//    MailTaxi::CreatingDriverWorkRequest request;
//    auto createDriverWorkRpc = new CreatingDriverWorkRpcClient;
//    request.set_driverid(driverId);
//    request.set_drivertoken(driverToken);
//    createDriverWorkRpc->responseReader = stub->PrepareAsyncCreatingDriverWork(
//            &createDriverWorkRpc->context, request, &completionQueue
//            );
//    createDriverWorkRpc->responseReader->StartCall();
//    createDriverWorkRpc->responseReader->Finish(&createDriverWorkRpc->response, &createDriverWorkRpc->status,
//                                          static_cast<void*>(createDriverWorkRpc));
//}
//
//void OrderManagerClient::CreateOrder(uint64_t orderNum, uint64_t driverId, const std::string &clientToken) {
//    MailTaxi::CreateOrderRequest request;
//    auto CreateOrderRPc = new CreateOrderRpcClient;
//    CreateOrderRPc->responseReader = stub->PrepareAsyncCreateOrder(
//            &CreateOrderRPc->context, request, &completionQueue
//    );
//    request.set_driverid(driverId);
//    request.set_ordernum(orderNum);
//    request.set_clienttoken(clientToken);
//    CreateOrderRPc->responseReader->StartCall();
//    CreateOrderRPc->responseReader->Finish(&CreateOrderRPc->response, &CreateOrderRPc->status,
//                                           static_cast<void*>(CreateOrderRPc));
//}
//
//void OrderManagerClient::ProposeOrder(uint64_t driverId, const MailTaxi::OrderLocationData &locationData) {
//    MailTaxi::ProposeOrderRequest request;
//    auto proposeOrderRpc = new ProposeOrderRpcClient;
//    proposeOrderRpc->responseReader = stub->PrepareAsyncProposeOrder(
//            &proposeOrderRpc->context, request, &completionQueue
//    );
//    request.set_driverid(driverId);
//    request.set_allocated_orderlocationdata(new MailTaxi::OrderLocationData(locationData));
//    proposeOrderRpc->responseReader->StartCall();
//    proposeOrderRpc->responseReader->Finish(&proposeOrderRpc->response, &proposeOrderRpc->status,
//                                            static_cast<void*>(proposeOrderRpc));
//}
//
//OrderManagerClient::OrderManagerClient(std::shared_ptr<grpc::Channel> channel) : stub(MailTaxi::OrderManager::NewStub(channel)){
//
//}
//
//void OrderManagerClient::CompleteRpcRun() {
//    void* got_tag;
//    bool ok = false;
//    while (completionQueue.Next(&got_tag, &ok)) {
//        auto handler = static_cast<OrderManagementRpcHandler*>(got_tag);
//        handler->Handle(ok);
//    }
//}
OrderManagerClient::OrderManagerClient(std::shared_ptr<grpc::Channel> channel) : stub(MailTaxi::OrderManager::NewStub(channel)){

}

MailTaxi::CloseDriverWorkResponse OrderManagerClient::CloseDriverWork(uint64_t driverId) {
    MailTaxi::CloseDriverWorkRequest request;
    request.set_driverid(driverId);
    grpc::ClientContext context;
    MailTaxi::CloseDriverWorkResponse response;
    stub->CloseDriverWork(&context, request, &response);
    return response;
}

MailTaxi::CreatingDriverWorkResponse
OrderManagerClient::CreatingDriverWork(uint64_t driverId, const std::string &driverToken) {
    MailTaxi::CreatingDriverWorkRequest request;
    grpc::ClientContext context;
    MailTaxi::CreatingDriverWorkResponse response;
    request.set_driverid(driverId);
    request.set_drivertoken(driverToken);
    stub->CreatingDriverWork(&context, request, &response);
    return response;
}


MailTaxi::CreateOrderResponse
OrderManagerClient::CreateOrder(uint64_t orderNum, uint64_t driverId, const std::string &clientToken) {
    MailTaxi::CreateOrderRequest request;
    grpc::ClientContext context;
    MailTaxi::CreateOrderResponse response;
    request.set_driverid(driverId);
    request.set_ordernum(orderNum);
    request.set_clienttoken(clientToken);
    stub->CreateOrder(&context, request, &response);
    return response;
}

MailTaxi::ProposeOrderResponse
OrderManagerClient::ProposeOrder(uint64_t driverId, MailTaxi::OrderLocationData &locationData, uint64_t orderNum) {
    MailTaxi::ProposeOrderRequest request;
    grpc::ClientContext context;
    MailTaxi::ProposeOrderResponse response;
    request.set_driverid(driverId);
    request.set_allocated_orderlocationdata(new MailTaxi::OrderLocationData(locationData));
    request.set_ordernum(orderNum);
    stub->ProposeOrder(&context, request, &response);
    return response;
}

MailTaxi::RemoveOrderResponse OrderManagerClient::RemoveOrder(uint64_t orderNum) {
    MailTaxi::RemoveOrderRequest request;
    request.set_ordernum(orderNum);
    grpc::ClientContext context;
    MailTaxi::RemoveOrderResponse response;
    stub->RemoveOrder(&context, request, &response);
    return response;
}
