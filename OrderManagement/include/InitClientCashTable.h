#pragma once
#include "atomic"

#include "Session.h"
#include "CashTable.h"

struct InitClientData{
    std::string token;
    uint64_t driverId;
};

class InitClientCashTable {
private:
    std::shared_ptr<ICashTable<uint64_t, std::shared_ptr<InitClientData>>> table;
    InitClientCashTable();
public:
    static std::shared_ptr<InitClientCashTable> GetInitCashTableSingleton();
    bool CreateInitClientData(uint64_t, const InitClientData& data);
    bool DeleteClientData(uint64_t);
    uint64_t GetDriverId(uint64_t);
    std::pair<bool, uint64_t> Confirm(uint64_t, std::string token);
};

