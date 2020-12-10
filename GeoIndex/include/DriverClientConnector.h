#pragma once
#include "GeoIndexer.h"
#include "CashTable.h"
#include "OrderDataStructs.h"
#include <Web.h>



class DriverClientConnector{
private:
    std::shared_ptr<GeoIndexer> geoIndexer;
    std::shared_ptr<Cone::Web::EventLoop> eventLoop;
    std::shared_ptr<ICashTable<std::string, std::shared_ptr<OrderData>>> cashTable;
    Cone::Web::Acceptor acceptor;
    static const size_t newAcceptBufferSize = 36;
    static const size_t driverHandleReadBufferSize = 20;
private:
    void SuccessfulAcceptHandler(const std::shared_ptr<Cone::Web::Connection>& connection);
    void BadAcceptHandler();
    void StartHandleDriverConnection(std::shared_ptr<Cone::Web::Connection> connection,
                                     std::shared_ptr<MailTaxiAPI::LocationData> locationData);
    void HandleDriverConnection(std::shared_ptr<Cone::Web::Connection> connection,
                                std::shared_ptr<std::string> token,
                                std::shared_ptr<std::string> buffer,
                                std::shared_ptr<GeoMover> geoMover, bool busy, size_t msgSize);
    void TempCloseDriverConnection(const std::string& token, std::shared_ptr<GeoMover> geoMover);
    void HandleClientConnection(std::shared_ptr<Cone::Web::Connection> connection,
                                std::shared_ptr<MailTaxiAPI::LocationData> locationData);

public:
    DriverClientConnector(std::shared_ptr<GeoIndexer>&, std::shared_ptr<Cone::Web::EventLoop>&,
            std::shared_ptr<ICashTable<std::string, std::shared_ptr<OrderData>>>&, uint16_t port);
    void operator()();
};

