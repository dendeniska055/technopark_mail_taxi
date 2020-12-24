#include "CashTable.h"
#include "CDSHelper.h"
#include "Session.h"
#include "SessionManager.h"
#include "ConnectionManagementCash.h"
#include "InitClientCashTable.h"
#include "OrderManagementServer.h"
int main(){
    size_t sessionManagerThreadNum = 2;
    size_t orderManagerThreadNum = 2;
    std::string serverAddress = "127.0.0.1:8003";
    std::string geoIndexAddress = "localhost:50051";
    CDSHelper::CDSInitializer<rcu_gpb>::initialize();
    CDSHelper::CDSThreadInitializer threadInitializer;
    auto connectionManagementCash = ConnectionManagementCash::GetConnectionManagementCashSingleton();
    auto initClientCash = InitClientCashTable::GetInitCashTableSingleton();
    OrderManagementServerImp orderManagementServer(connectionManagementCash, initClientCash);
    GeoIndexClientPtr geoIndexClient(
            new GeoIndexClient((grpc::CreateChannel(geoIndexAddress, grpc::InsecureChannelCredentials()))));
    SessionManager sessionManager(1, tcp::endpoint{net::ip::make_address("127.0.0.1"), 8000}, geoIndexClient,
                                  connectionManagementCash, initClientCash);

    sessionManager.Run();
    orderManagementServer.Run(orderManagerThreadNum, serverAddress);
    std::string command = "start";
    while (command != "close"){
        std::cin >> command;
    }
    sessionManager.Stop();
    orderManagementServer.Stop();
    return 0;
}
