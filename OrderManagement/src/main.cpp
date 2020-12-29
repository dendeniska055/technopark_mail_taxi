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
    std::string serverAddress = "127.0.0.1:8002";
    std::string geoIndexAddress = "127.0.0.1:8001";
    CDSHelper::CDSInitializer<rcu_gpb>::initialize();
    CDSHelper::CDSThreadInitializer threadInitializer;
    auto connectionManagementCash = ConnectionManagementCash::GetConnectionManagementCashSingleton();
    auto initClientCash = InitClientCashTable::GetInitCashTableSingleton();
    OrderManagementServerImp orderManagementServer(connectionManagementCash, initClientCash);
    GeoIndexClientPtr geoIndexClient(
            new GeoIndexClient((grpc::CreateChannel(geoIndexAddress, grpc::InsecureChannelCredentials()))));
    SessionManager sessionManager(1, tcp::endpoint{net::ip::make_address("127.0.0.1"), 8000}, geoIndexClient,
                                  connectionManagementCash, initClientCash);
    std::thread th([&geoIndexClient](){
        CDSHelper::CDSThreadInitializer threadInitializer;
        geoIndexClient->CompleteRpcRun();
    });

    sessionManager.Run();
    orderManagementServer.Run(orderManagerThreadNum, serverAddress);
    std::string command = "start";
    while (command != "close"){
        std::cin >> command;
    }
    sessionManager.Stop();
    orderManagementServer.Stop();
    th.join();
    return 0;
}
