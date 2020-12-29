#include "AuthorizationServerImpl.h"
#include "DataBase.h"
#include "CDSHelper.h"
#include "GeoIndexClientSync.h"
int main(){
    CDSHelper::CDSInitializer<rcu_gpb>::initialize();
    CDSHelper::CDSThreadInitializer threadInitializer;
    AuthorizationServerImpl server;
    std::shared_ptr<GeoIndexClientSync> geoIndexClient(new GeoIndexClientSync(grpc::CreateChannel(
            "127.0.0.1:8001", grpc::InsecureChannelCredentials())));
    std::shared_ptr<OrderManagerClient> orderManagementCLient(new OrderManagerClient(grpc::CreateChannel(
            "127.0.0.1:8002", grpc::InsecureChannelCredentials())));
    server.Run(4, std::string("127.0.0.1:8003"), geoIndexClient, orderManagementCLient);
    std::string command;
    std::cin >> command;
    server.Stop();
    return 0;
}