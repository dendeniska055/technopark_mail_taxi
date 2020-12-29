#pragma once
#include "Session.h"
#include "InitSession.h"
#include "GeoIndexClient.h"
#include "ConnectionManagementCash.h"
#include "InitClientCashTable.h"
#include <vector>
#include <thread>

class SessionManager {
private:
    boost::asio::io_context context;
    boost::asio::ip::tcp::acceptor acceptor;
    GeoIndexClientPtr geoIndexClient;
    int threadNum;
    std::shared_ptr<ConnectionManagementCash> connectionManagementCashTable;
    std::shared_ptr<InitClientCashTable> initClientCashTable;
    std::vector<std::thread> workers;
private:
    void Accept();
    void HandlingDriverWithoutClient(SessionPtr session, const MailTaxi::Coordinate&);
    void HandlingDriverWithClient(SessionPtr driverSession, SessionPtr clientSession,const MailTaxi::Coordinate&);
    void HandlingClientWithoutDriver(SessionPtr session, const MailTaxi::Coordinate&);
    void HandlingClientWithDriver(SessionPtr clientSession, SessionPtr driverSession, const MailTaxi::Coordinate&);
public:
    SessionManager(int threadNum, const tcp::endpoint& endpoint, GeoIndexClientPtr geoIndexClient,
                   std::shared_ptr<ConnectionManagementCash>, std::shared_ptr<InitClientCashTable>);
    ~SessionManager();
    void Run();
    void Stop();
};
