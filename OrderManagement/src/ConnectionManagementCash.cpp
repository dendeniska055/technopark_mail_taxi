//
// Created by anton on 23.12.2020.
//

#include "ConnectionManagementCash.h"

ConnectionManagementCash::ConnectionManagementCash() :table(new CashTableLockFree<uint64_t, std::shared_ptr<ConnectionManagementData>>){

}

std::shared_ptr<ConnectionManagementCash> ConnectionManagementCash::GetConnectionManagementCashSingleton() {
    std::shared_ptr<ConnectionManagementCash> connectionManagementCashSingleton(new ConnectionManagementCash);
    return connectionManagementCashSingleton;
}

bool ConnectionManagementCash::CreateEmptyConnectionManagementData(uint64_t key, const std::string &driverToken) {
    std::shared_ptr<ConnectionManagementData> emptyData = std::make_shared<ConnectionManagementData>();
    emptyData->driverOn = std::make_shared<std::atomic_bool>(false);
    emptyData->clientOn = std::make_shared<std::atomic_bool>(false);
    emptyData->driverToken = driverToken;
    return table->Insert(key, emptyData);
}

bool ConnectionManagementCash::DeleteConnectionData(uint64_t key) {
    table->Find(key, [](const uint64_t& key, std::shared_ptr<ConnectionManagementData>& connectionManagementData){
        connectionManagementData->driverOn->store(false);
        connectionManagementData->clientOn->store(false);
        connectionManagementData->driverSession->Close();
        connectionManagementData->clientSession->Close();
    });
    return table->Erase(key);
}

bool ConnectionManagementCash::DriverConnect(uint64_t key, SessionPtr &driverSession, const std::string &driverToken) {
    bool driverAlreadyOnLine = true;
    table->Find(key, [&driverAlreadyOnLine, driverSession, &driverToken]
    (const uint64_t& key, std::shared_ptr<ConnectionManagementData>& connectionManagementData){
       if (driverToken == connectionManagementData->driverToken) {
           if (!(driverAlreadyOnLine = connectionManagementData->driverOn->load())) {
               connectionManagementData->driverSession = driverSession;
               connectionManagementData->driverOn->store(true);
               driverSession->SetPartnerOnline(connectionManagementData->clientOn);
           }
       }
    });
    return !driverAlreadyOnLine;
}

bool ConnectionManagementCash::ClientConnect(uint64_t key, SessionPtr &clientSession) {
    bool clientAlreadyOnLine = true;
    table->Find(key, [&clientAlreadyOnLine, clientSession](const uint64_t& key, std::shared_ptr<ConnectionManagementData>& connectionManagementData){
        if (!(clientAlreadyOnLine = connectionManagementData->clientOn->load())) {
            connectionManagementData->clientSession = clientSession;
            connectionManagementData->clientOn->store(true);
            clientSession->SetPartnerOnline(connectionManagementData->driverOn);
        }
    });
    return !clientAlreadyOnLine;
}

void ConnectionManagementCash::DriverDisConnect(uint64_t key) {
    table->Find(key, [](const uint64_t& key, std::shared_ptr<ConnectionManagementData>& connectionManagementData){
        connectionManagementData->driverOn->store(false);
    });
}

void ConnectionManagementCash::ClientDisConnect(uint64_t key) {
    table->Find(key, [](const uint64_t& key, std::shared_ptr<ConnectionManagementData>& connectionManagementData){
        connectionManagementData->clientOn->store(false);
    });
}

SessionPtr ConnectionManagementCash::GetDriver(uint64_t key) {
    SessionPtr driverSession;
    table->Find(key, [&driverSession](const uint64_t&, std::shared_ptr<ConnectionManagementData>& data){
        driverSession = data->driverSession;
    });

    return driverSession;
}

SessionPtr ConnectionManagementCash::GetClient(uint64_t key) {
    SessionPtr clientSession;
    table->Find(key, [&clientSession](const uint64_t&, std::shared_ptr<ConnectionManagementData>& data){
        clientSession = data->clientSession;
    });

    return clientSession;
}

std::pair<bool, SessionPtr> ConnectionManagementCash::GetAndCheckDriverSession(uint64_t driverId) {
    std::pair<bool, SessionPtr> res;
    res.first = false;
    table->Find(driverId, [&res](const uint64_t&, std::shared_ptr<ConnectionManagementData>& data){
        if ((res.first = data->driverOn->load())){
            res.second = data->driverSession;
        }
    });
    return res;
}

bool ConnectionManagementCash::DeleteClientSession(uint64_t id) {
    return table->Find(id, [this](const uint64_t&, std::shared_ptr<ConnectionManagementData>& data){
        data->clientOn->store(false);
        data->clientSession->Close([this, driverSession = data->driverSession](beast::error_code err){
            driverSession->SendString(orderCloseString, [this, driverSession](beast::error_code err){
                if(err){
                    DriverDisConnect(driverSession->GetId());
                }
            });
        });
    });
}

