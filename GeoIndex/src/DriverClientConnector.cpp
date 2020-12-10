#include "DriverClientConnector.h"


DriverClientConnector::DriverClientConnector(std::shared_ptr<GeoIndexer>& _geoIndexer, std::shared_ptr<Cone::Web::EventLoop>& _eventLoop,
                                             std::shared_ptr<ICashTable<std::string, std::shared_ptr<OrderData>>>& _cashTable, uint16_t port) :
        geoIndexer(_geoIndexer), eventLoop(_eventLoop), cashTable(_cashTable),
        acceptor(port){

}

void DriverClientConnector::operator()() {
    std::shared_ptr<Cone::Web::Connection> connection = Cone::Web::Connection::NewConnection();
    acceptor.Accept(connection, [connection, this](const boost::system::error_code &err){
        if (!err)
            SuccessfulAcceptHandler(connection);
        else
            BadAcceptHandler();
    });
}


void DriverClientConnector::SuccessfulAcceptHandler(const std::shared_ptr<Cone::Web::Connection>& connection) {
    std::shared_ptr<std::string> buffer = std::make_shared<std::string>();
    buffer->resize(newAcceptBufferSize);
    connection->Receive(buffer, [buffer, connection, this](const boost::system::error_code& err, size_t size){
        std::cout << *buffer << std::endl;
        if (!err){
            std::shared_ptr<MailTaxiAPI::LocationData> locationData = std::make_shared<MailTaxiAPI::LocationData>();
            if (locationData->ParseFromArray(buffer->c_str(), size)){
                if (locationData->isdriver()){
                    eventLoop->PutCallBack([connection, locationData, this](){
                        StartHandleDriverConnection(connection, locationData);
                    });
                }
                else{
                    eventLoop->PutCallBack([connection, locationData, this](){
                        HandleClientConnection(connection, locationData);
                    });
                }
            }
        }
        else{
            connection->Close();
        }
    });
    std::shared_ptr<Cone::Web::Connection> newConnection = Cone::Web::Connection::NewConnection();
    acceptor.Accept(newConnection, [newConnection, this](const boost::system::error_code &err){
        if (!err)
            SuccessfulAcceptHandler(newConnection);
        else {
            BadAcceptHandler();
        }
    });
}

void DriverClientConnector::StartHandleDriverConnection(std::shared_ptr<Cone::Web::Connection> connection,
                                                        std::shared_ptr<MailTaxiAPI::LocationData> locationData) {
//    std::cout << "type: " << " LocationData " << std::endl;
//    std::cout << "token: " << locationData->token() << std::endl;
//    std::cout << "coordinates " << locationData->coordinate().latitude() << ' '<< locationData->coordinate().longitude() << std::endl;
    bool registeredDriver = cashTable->Find(locationData->token(), [&connection, &locationData]
     (const std::string& token, std::shared_ptr<OrderData>& orderData){
         orderData->driverConnection = connection;
         orderData->driveCoordinate = locationData->coordinate();
         orderData->driverLastPingTime = std::chrono::system_clock::now();
         orderData->driverBusy = false;
         orderData->driverConnect = true;
     });

    if (registeredDriver) {
//        std::cout << "Водитель с токеном " << locationData->token() << " существует в базе" << std::endl;
        eventLoop->PutCallBack([locationData, connection, this](){
            GeoMover mover = geoIndexer->Update(locationData->coordinate(), locationData->token(), [&locationData](DriverData& data){
                data.busy = false;
                data.active = true;
                data.coordinate = locationData->coordinate();
            });
            std::shared_ptr<std::string> buffer = std::make_shared<std::string>();
            buffer->resize(driverHandleReadBufferSize);
            std::shared_ptr<GeoMover> geoMover = std::make_shared<GeoMover>(std::move(mover));
            connection->Receive(buffer, [this, buffer, geoMover, connection, locationData](const boost::system::error_code& err, size_t size){
                if (!err){
                    HandleDriverConnection(connection, std::make_shared<std::string>(locationData->token()),
                                           buffer, geoMover, false, size);
                }
                else
                    TempCloseDriverConnection(locationData->token(), geoMover);
            });
        });
    }
    else
        connection->Close();
}

void DriverClientConnector::HandleDriverConnection(std::shared_ptr<Cone::Web::Connection> connection,
                                                   std::shared_ptr<std::string> token,
                                                   std::shared_ptr<std::string> buffer,
                                                   std::shared_ptr<GeoMover> geoMover, bool busy, size_t msgSize) {
    MailTaxiAPI::DriverLocationData locationData;
    if (locationData.ParseFromArray(buffer->c_str(), msgSize)){
        std::cout << *token << std::endl << locationData.coordinate().latitude() << ' ' << locationData.coordinate().longitude() << std::endl;
        if (locationData.busy() != busy){
            cashTable->Find(*token, [&locationData, busy](const std::string& key, std::shared_ptr<OrderData>& data){
                data->driverBusy = busy;
                data->driveCoordinate = locationData.coordinate();
                data->driverLastPingTime = std::chrono::system_clock::now();
            });
            busy = locationData.busy();
            geoMover->GetData().busy = busy;
        }
        else{
            cashTable->Find(*token, [&locationData](const std::string& key, std::shared_ptr<OrderData>& data){
                data->driveCoordinate = locationData.coordinate();
                data->driverLastPingTime = std::chrono::system_clock::now();
            });
        }
        geoMover->operator()(locationData.coordinate());
    }
    connection->Receive(buffer, [this, buffer, geoMover, connection, token, &locationData](const boost::system::error_code& err, size_t size){
        if (!err)
            HandleDriverConnection(connection, token, buffer, geoMover, locationData.busy(), size);
        else
            TempCloseDriverConnection(*token, geoMover);
    });
}

void DriverClientConnector::BadAcceptHandler() {
    std::shared_ptr<Cone::Web::Connection> connection = Cone::Web::Connection::NewConnection();
    acceptor.Accept(connection, [connection, this](const boost::system::error_code &err){
        if (!err)
            SuccessfulAcceptHandler(connection);
        else {
            BadAcceptHandler();
        }
    });
}

void DriverClientConnector::TempCloseDriverConnection(const std::string& token, std::shared_ptr<GeoMover> geoMover) {
    cashTable->Find(token, [](const std::string& key, std::shared_ptr<OrderData>& data){
        data->driverConnect = false;
        data->driverConnection->Close();
    });
    geoMover->GetData().active = false;
}

void DriverClientConnector::HandleClientConnection(std::shared_ptr<Cone::Web::Connection> connection,
                                                   std::shared_ptr<MailTaxiAPI::LocationData> locationData) {
    bool handleClientAccept = false;
    std::shared_ptr<Cone::Web::Connection> driverConnection;
    MailTaxiAPI::Coordinate driverCoordinate;
    cashTable->Find(locationData->token(), [&handleClientAccept, &driverConnection, &driverCoordinate](const std::string& key, std::shared_ptr<OrderData>& data){
        if (data->driverBusy && data->driverConnect) {
            handleClientAccept = true;
            driverConnection = data->driverConnection;
            driverCoordinate = data->driveCoordinate;
        }
    });
    if (handleClientAccept){
        connection->Send(std::make_shared<std::string>(driverCoordinate.SerializeAsString()),
                [connection](const boost::system::error_code& err, size_t size){
            connection->Close();
        });
        driverConnection->Send(std::make_shared<std::string>(locationData->coordinate().SerializeAsString()),
                         [](const boost::system::error_code& err, size_t size){});
    }

}



//
//void DriverClientConnector::SuccessfulAcceptHandler(const std::shared_ptr<Cone::Web::Connection>& driverConnection) {
//    std::shared_ptr<std::string> buffer = std::make_shared<std::string>();
//    buffer->resize(maxReadBufferSize);
//    driverConnection->Receive(buffer, [buffer, driverConnection, this](const boost::system::error_code& err, size_t size){
//        if (!err){
//            std::shared_ptr<MailTaxiAPI::LocationData> locationData = std::make_shared<MailTaxiAPI::LocationData>();
//            if (locationData->ParseFromString(*buffer)){
//                if (locationData->isdriver()){
//                    eventLoop->PutCallBack([driverConnection, locationData, this](){
//                        StartHandleDriverConnection(driverConnection, locationData);
//                    });
//                }
//                else{
//                    //обработка клиентского соедиенения
//                }
//            }
//        }
//        else{
//            std::cout << "err!" << std::endl;
//        }
//    });
//    std::shared_ptr<Cone::Web::Connection> newConnection = Cone::Web::Connection::NewConnection();
//    acceptor.Accept(newConnection, [newConnection, this](const boost::system::error_code &err){
//        if (!err)
//            SuccessfulAcceptHandler(newConnection);
//        else {
//            BadAcceptHandler();
//        }
//    });
//}
//
//void DriverClientConnector::BadAcceptHandler() {
//    std::shared_ptr<Cone::Web::Connection> driverConnection = Cone::Web::Connection::NewConnection();
//    acceptor.Accept(driverConnection, [driverConnection, this](const boost::system::error_code &err){
//        if (!err)
//            SuccessfulAcceptHandler(driverConnection);
//        else {
//            BadAcceptHandler();
//        }
//    });
//}
//
//void DriverClientConnector::StartHandleDriverConnection(std::shared_ptr<Cone::Web::Connection> driverConnection,
//                                                        std::shared_ptr<MailTaxiAPI::LocationData> locationData) {
//
//     bool registeredDriver = cashTable->Find(locationData->token(), [&driverConnection, &locationData]
//     (const std::string& token,OrderData& orderData){
//         orderData.driverConnection = driverConnection;
//         orderData.driveCoordinate = locationData->coordinate();
//         orderData.driverMover(orderData.driveCoordinate);
//         orderData.driverLastPingTime = std::chrono::system_clock::now();
//     });
//     if (registeredDriver){
//         eventLoop->PutCallBack([driverConnection, this](){
//             HandleDriverConnection(driverConnection);
//         });
//     }
//     else{
//         driverConnection->Close();
//     }
//}
//
//void DriverClientConnector::HandleDriverConnection(const std::shared_ptr<Cone::Web::Connection>& driverConnection) {
//    std::shared_ptr<std::string> buffer = std::make_shared<std::string>();
//    buffer->resize(maxReadBufferSize);
//    driverConnection->Receive(buffer, [buffer, driverConnection, this](const boost::system::error_code& err, size_t size) {
//        if (!err){
//            std::shared_ptr<MailTaxiAPI::LocationData> locationData = std::make_shared<MailTaxiAPI::LocationData>();
//            if (locationData->ParseFromString(*buffer)){
//                cashTable->Find(locationData->token(), [&driverConnection, &locationData]
//                    (const std::string& token,OrderData& orderData) {
//                    orderData.driveCoordinate = locationData->coordinate();
//                    orderData.driverMover(orderData.driveCoordinate);
//                    orderData.driverLastPingTime = std::chrono::system_clock::now();
//                });
//
//            }
//
//        }
//        else{
//            driverConnection->Close();
//            //отметить разрыв соединения с водителем
//        }
//    });
//}

