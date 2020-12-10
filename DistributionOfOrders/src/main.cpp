#include <iostream>
#include "DistributionOfOrdersService.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

int main(){
    GrpcDistributionOfOrdersService service(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()),
                                            "127.0.0.1:8025");

    return 0;
}