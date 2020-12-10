
#include <iostream>
#include <mutex>
#include <thread>
#include <Web.h>
#include <CDSHelper.h>
#include "CashTable.h"
#include "GeoIndexer.h"
#include "OrderDataStructs.h"
#include "DriverClientConnector.h"
#include "OrderService.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

int main() {
    const size_t _tempThreadNum = 4;
    Cone::CDSHelper::CDSInitializer<rcu_gpb>::initialize();
    Cone::CDSHelper::CDSThreadInitializer threadInitializer;
    std::shared_ptr<Cone::Web::EventLoop> eventLoop = Cone::Web::EventLoop::GetEventLoop();

    std::shared_ptr<ICashTable<std::string, std::shared_ptr<OrderData>>> cashTable =
            std::make_shared<CashTableLockFree<std::string, std::shared_ptr<OrderData>>>();
    std::shared_ptr<GeoIndexer> geoIndexer = std::make_shared<QuadTreeGeoIndexer<DriverData>>();
    DriverClientConnector requestHandler(geoIndexer, eventLoop, cashTable, 8000);
    requestHandler();
    eventLoop->StartEventLoop(_tempThreadNum);
    eventLoop->EndEventLoop();

    std::string server_address("127.0.0.1:50051");
    GrpcOrderService service(geoIndexer, cashTable, eventLoop, server_address);
    std::string command;
    while (command != "close"){
        std::cin >> command;
    }
    return 0;
}

