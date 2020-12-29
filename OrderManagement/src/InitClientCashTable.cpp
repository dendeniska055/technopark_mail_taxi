#include "InitClientCashTable.h"

InitClientCashTable::InitClientCashTable() : table(new CashTableLockFree<uint64_t, std::shared_ptr<InitClientData>>){

}

std::shared_ptr<InitClientCashTable> InitClientCashTable::GetInitCashTableSingleton() {
    std::shared_ptr<InitClientCashTable> initClientCashTableSingleton(new InitClientCashTable);
    return initClientCashTableSingleton;
}

bool InitClientCashTable::CreateInitClientData(uint64_t key, const InitClientData& data) {
    std::shared_ptr<InitClientData> clientData(new InitClientData(data));
    return table->Insert(key, clientData);
}

bool InitClientCashTable::DeleteClientData(uint64_t key) {
    return table->Erase(key);
}

std::pair<bool, uint64_t> InitClientCashTable::Confirm(uint64_t key, std::string token) {
    std::pair<bool, uint64_t> result(false, 0);
    table->Find(key, [&result, &token](const uint64_t&, std::shared_ptr<InitClientData>& data){
        if (data->token == token) {
            result.first = true;
            result.second = data->driverId;
        }
    });
    return result;
}

uint64_t InitClientCashTable::GetDriverId(uint64_t key) {
    uint64_t driverId = 0;
    table->Find(key, [&driverId](const uint64_t&, std::shared_ptr<InitClientData>& data){
        driverId = data->driverId;
    });
    return driverId;
}
