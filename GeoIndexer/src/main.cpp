#include "QuadTree.h"
#include "DriverGeoIndexer.h"
#include "CDSHelper.h"
#include "GeoIndexServer.h"
int main(){
    CDSHelper::CDSInitializer<rcu_gpb>::initialize();
    CDSHelper::CDSThreadInitializer threadInitializer;
    GeoIndexServerImpl server;
    server.Run(4, std::string("127.0.0.1:8001"));
    std::string command;
    while (command != "close"){
        std::cin >> command;
    }
    server.Stop();
    return 0;
}