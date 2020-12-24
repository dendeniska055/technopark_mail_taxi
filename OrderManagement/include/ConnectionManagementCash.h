#pragma once
#include "atomic"

#include "Session.h"
#include "CashTable.h"

typedef  std::shared_ptr<std::atomic_bool> AtomicBoolPtr;
struct ConnectionManagementData{
    SessionPtr driverSession;
    SessionPtr clientSession;
    AtomicBoolPtr driverOn;
    AtomicBoolPtr clientOn;
    std::string driverToken;
};

class ConnectionManagementCash {
private:
    std::shared_ptr<ICashTable<uint64_t, std::shared_ptr<ConnectionManagementData>>> table;
    ConnectionManagementCash();
public:
    static std::shared_ptr<ConnectionManagementCash> GetConnectionManagementCashSingleton();
    bool CreateEmptyConnectionManagementData(uint64_t key, const std::string &driverToken);
    bool DeleteConnectionData(uint64_t key);
    bool DeleteClientSession(uint64_t key);
    bool DriverConnect(uint64_t key, SessionPtr &driverSession, const std::string &driverToken);
    bool ClientConnect(uint64_t key, SessionPtr &clientSession);
    void DriverDisConnect(uint64_t key);
    void ClientDisConnect(uint64_t key);
    SessionPtr GetDriver(uint64_t key);
    SessionPtr GetClient(uint64_t key);
    std::pair<bool, SessionPtr> GetAndCheckDriverSession(uint64_t driverId);
};

