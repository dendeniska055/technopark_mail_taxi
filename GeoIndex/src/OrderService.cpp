#include "OrderService.h"

grpc::Status OrderService::GetDrivers(::grpc::ServerContext *context, const ::MailTaxiAPI::GetDriversRequest *request,
                                      ::MailTaxiAPI::GetDriversResponse *response) {
    Cone::CDSHelper::CDSThreadInitializer threadInitializer;
    const size_t minDriverSize = 5;
    const size_t maxQuadrantSearchNum = 64;
    auto roundMover = geoIndexer->Find(request->coordinate());
    bool driverSelectionIndicator = false;
    size_t quadrantCount = 0;
    while (!driverSelectionIndicator){
        for (auto it = roundMover.GetData().begin(); it != roundMover.GetData().end(); it++){
            auto driverInformation = response->add_information();
            if (!it->second.busy && it->second.active &&
            it->second.driver.animals() == request->animals() && it->second.driver.children() == request->children()) {
                driverInformation->set_token(it->first);
                driverInformation->set_allocated_coordinate(new MailTaxiAPI::Coordinate(it->second.coordinate));
                driverInformation->set_allocated_driver(new MailTaxiAPI::Driver(it->second.driver));
            }
        }
        ++roundMover;
        if (quadrantCount == maxQuadrantSearchNum || response->information().size() >= minDriverSize)
            driverSelectionIndicator = true;
        quadrantCount++;
    }

    return grpc::Status::OK;;
}

grpc::Status OrderService::RegDriver(::grpc::ServerContext *context, const ::MailTaxiAPI::RegDriverRequest *request,
                                     ::MailTaxiAPI::RegDriverResponse *response) {
    Cone::CDSHelper::CDSThreadInitializer threadInitializer;
    DriverData driverData;
    driverData.coordinate = request->coordinate();
    driverData.driver = request->driver();
    driverData.busy = false;
    driverData.active = false;
    geoIndexer->Insert({request->token(), driverData}, request->coordinate());
    std::shared_ptr<OrderData> orderData = std::make_shared<OrderData>();
    orderData->driverConnect = false;
    orderData->driveCoordinate = request->coordinate();
    orderData->driverBusy = false;
    cashTable->Insert(request->token(), orderData);
    response->set_success(true);
    return grpc::Status::OK;
}

OrderService::OrderService(std::shared_ptr<GeoIndexer> _geoIndexer,
                           std::shared_ptr<ICashTable<std::string, std::shared_ptr<OrderData>>> _cashTable,
                           std::shared_ptr<Cone::Web::EventLoop> _eventLoop){
    geoIndexer = _geoIndexer;
    cashTable = _cashTable;
    eventLoop = _eventLoop;

}

grpc::Status
OrderService::NewOrderForDriver(::grpc::ServerContext *context, const ::MailTaxiAPI::NewOrderForDriverRequest *request,
                                ::MailTaxiAPI::NewOrderForDriverResponse *response) {
    Cone::CDSHelper::CDSThreadInitializer threadInitializer;
    auto token = request->token();
    auto orderData = request->orderdata();
    grpc::Status status = grpc::Status::OK;
    std::shared_ptr<Cone::Web::Connection> connection;
    cashTable->Find(token, [&status, &connection](const std::string& token, std::shared_ptr<OrderData> data){
        if (data->driverBusy || !data->driverConnect) {
            status = grpc::Status::CANCELLED;
            connection = data->driverConnection;
            data->driverBusy = true;
        }
    });
    if (status.ok()){
        connection->Send(std::make_shared<std::string>(orderData.SerializeAsString()), []
                (const boost::system::error_code& err, size_t size){});
    }
    return status;
}

GrpcOrderService::GrpcOrderService(std::shared_ptr<GeoIndexer> geoIndexer,
                                   std::shared_ptr<ICashTable<std::string, std::shared_ptr<OrderData>>> cashTable,
                                   std::shared_ptr<Cone::Web::EventLoop> eventLoop, std::string addr) :
                                   orderService(geoIndexer, cashTable, eventLoop){
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&orderService);
    server = std::move(std::unique_ptr<grpc::Server>(builder.BuildAndStart()));
}

GrpcOrderService::~GrpcOrderService() {
        server->Wait();
}
