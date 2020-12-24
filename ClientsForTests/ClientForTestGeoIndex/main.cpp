#include "iostream"
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>

#include "GeoIndex.grpc.pb.h"
#include "GeoIndexClient.h"
#include "GeoIndexClient.h"
int main(){
    GeoIndexClient client(grpc::CreateChannel(
            "localhost:50051", grpc::InsecureChannelCredentials()));
    MailTaxi::Coordinate coordinate;
    coordinate.set_latitude(78.2131);
    coordinate.set_longitude(120.123);
    client.SelectDriverRpc(coordinate, true, true);
    client.CompleteRpcRun();
};