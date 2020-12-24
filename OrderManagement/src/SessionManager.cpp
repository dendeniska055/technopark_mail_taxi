//
// Created by anton on 22.12.2020.
//

#include "SessionManager.h"
#include "InitClientCashTable.h"
#include <utility>
#include "ConnectionManagementCash.h"
#include "CDSHelper.h"
SessionManager::SessionManager(int threadNum, const tcp::endpoint& endpoint, GeoIndexClientPtr geoIndexClient,
                               std::shared_ptr<ConnectionManagementCash> a, std::shared_ptr<InitClientCashTable> b)
: context{threadNum}, acceptor(context), connectionManagementCashTable(std::move(a)), initClientCashTable(std::move(b)) {
    this->geoIndexClient   = std::move(geoIndexClient);
    this->threadNum = threadNum;
    beast::error_code ec;
    acceptor.open(endpoint.protocol(), ec);
    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    acceptor.bind(endpoint, ec);
    acceptor.listen(net::socket_base::max_listen_connections, ec);
}

void SessionManager::Run() {
    Accept();
    for (size_t i = 0; i < threadNum; i++){
     workers.emplace_back(std::thread([this](){
         CDSHelper::CDSThreadInitializer threadInitializer;
         context.run();
     }));
    }
}

void SessionManager::Accept() {
    acceptor.async_accept(net::make_strand(context), [this](beast::error_code ec, tcp::socket socket){
        InitSession::CreateNewSession(std::move(socket), connectionManagementCashTable, initClientCashTable,[this] (SessionPtr session) {
            std::function<void(const MailTaxi::Coordinate &coordinate)> withoutPartner;
            std::function<void(const MailTaxi::Coordinate &coordinate)> withPartner;
            std::function<void()> disConnect;
            if (session->IsDriver()) {
                withoutPartner = [this, session](const MailTaxi::Coordinate &coordinate) {
                    HandlingDriverWithoutClient(session, coordinate);
                };
                withPartner = [this, session](const MailTaxi::Coordinate &coordinate) {
                    auto clientSession = connectionManagementCashTable->GetClient(session->GetId());
                    HandlingDriverWithClient(session, clientSession, coordinate);
                };
                disConnect = [this, session]() {
                    connectionManagementCashTable->DriverDisConnect(session->GetId());
                };
            } else {
                withoutPartner = [this, session](const MailTaxi::Coordinate &coordinate) {
                    HandlingClientWithoutDriver(session, coordinate);
                };
                withPartner = [this, session](const MailTaxi::Coordinate &coordinate) {
                    auto driverSession = connectionManagementCashTable->GetDriver(session->GetId());
                    HandlingClientWithDriver(session, driverSession, coordinate);
                };
                disConnect = [this, session]() {
                    connectionManagementCashTable->ClientDisConnect(session->GetId());
                };
            }
            session->ReceiveCoordinate(
                    [this, session, withoutPartner, withPartner, disConnect](MailTaxi::Coordinate &coordinate,
                                                                             beast::error_code err, bool successParse) {
                        if (!err && successParse) {
                            if (!session->PartnerOnLine()) {
                                withoutPartner(coordinate);
                            } else {
                                withPartner(coordinate);
                            }
                        } else {
                            disConnect();
                        }
                    });
        });
        Accept();
    });
}


void SessionManager::HandlingDriverWithoutClient(SessionPtr session, const MailTaxi::Coordinate & coordinate) {
    std::cout << "Координаты водителя(без клмента): "<<coordinate.latitude() << ' ' << coordinate.longitude() << std::endl;
    geoIndexClient->MoveDriver(coordinate, session->IsDriver());
    session->ReceiveCoordinate([this, session](const MailTaxi::Coordinate& coord, beast::error_code err, bool successParse){
        if (!err && successParse){
            if (!session->PartnerOnLine())
                HandlingDriverWithoutClient(session, coord);
            else{
                auto clientSession = connectionManagementCashTable->GetClient(session->GetId());
                HandlingDriverWithClient(session, clientSession, coord);
            }
        }
        else{
            connectionManagementCashTable->DriverDisConnect(session->GetId());
        }
    });
}

void SessionManager::HandlingDriverWithClient(SessionPtr driverSession, SessionPtr clientSession,
                                              const MailTaxi::Coordinate &coordinate) {
    std::cout << "Координаты водителя(с клиентом): "<<coordinate.latitude() << ' ' << coordinate.longitude() << std::endl;
    geoIndexClient->MoveDriver(coordinate, driverSession->IsDriver());
    clientSession->SendCoordinate(coordinate, [this, driverSession, clientSession](beast::error_code err){
        if (!err){
            driverSession->ReceiveCoordinate([this, driverSession, clientSession](const MailTaxi::Coordinate& coord, beast::error_code err, bool successParse){
                if (!err && successParse){
                    if (!driverSession->PartnerOnLine()) {
                        connectionManagementCashTable->ClientDisConnect(clientSession->GetId());
                        HandlingDriverWithoutClient(driverSession, coord);
                    }
                    else{
                        HandlingDriverWithClient(driverSession, clientSession, coord);
                    }
                }
                else{
                    connectionManagementCashTable->DriverDisConnect(driverSession->GetId());
                }
            });
        }
        else{
            connectionManagementCashTable->ClientDisConnect(clientSession->GetId());
        }
    });

}

void SessionManager::HandlingClientWithoutDriver(SessionPtr session, const MailTaxi::Coordinate &coordinate) {
    std::cout << "Координаты клмента(без водителя): "<<coordinate.latitude() << ' ' << coordinate.longitude() << ' '
              << session->PartnerOnLine() <<std::endl;
    session->ReceiveCoordinate([this, session]( const MailTaxi::Coordinate &coord, beast::error_code err,bool successParse){
        if (!err && successParse){
            if (!session->PartnerOnLine())
                HandlingClientWithoutDriver(session, coord);
            else{
                auto driverSession = connectionManagementCashTable->GetDriver(session->GetId());
                HandlingClientWithDriver(session, driverSession, coord);
            }
        }
        else{
            connectionManagementCashTable->ClientDisConnect(session->GetId());
        }
    });
}

void SessionManager::HandlingClientWithDriver(SessionPtr clientSession, SessionPtr driverSession,
                                              const MailTaxi::Coordinate &coordinate) {
    std::cout << "Координаты клмента(с водителем): "<<coordinate.latitude() << ' ' << coordinate.longitude() << ' '
    << clientSession->PartnerOnLine() <<std::endl;
    driverSession->SendCoordinate(coordinate, [this, clientSession, driverSession](beast::error_code err){
        if (!err){
            clientSession->ReceiveCoordinate([this, clientSession, driverSession]( const MailTaxi::Coordinate &coord, beast::error_code err,bool successParse){
                if (!err && successParse){
                    if (!clientSession->PartnerOnLine()){
                        connectionManagementCashTable->DriverDisConnect(driverSession->GetId());
                        HandlingClientWithoutDriver(clientSession, coord);
                    }
                    else{
                        HandlingClientWithDriver(clientSession, driverSession, coord);
                    }
                }
                else{
                    connectionManagementCashTable->ClientDisConnect(clientSession->GetId());
                }
            });
        }
        else{
            connectionManagementCashTable->DriverDisConnect(driverSession->GetId());
        }
    });
}

SessionManager::~SessionManager() {
    for (size_t i = 0; i< threadNum; i++){
        workers[i].join();
    }
}

void SessionManager::Stop() {
    context.stop();
}


