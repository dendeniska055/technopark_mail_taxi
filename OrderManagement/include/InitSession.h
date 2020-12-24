#pragma once
#include "Session.h"
#include "ConnectionManagementCash.h"
#include "InitClientCashTable.h"


class InitSession final{
private:
    websocket::stream<beast::tcp_stream> ws;
    beast::flat_buffer readBuffer;
    std::shared_ptr<ConnectionManagementCash> connectionManagementCash;
    std::shared_ptr<InitClientCashTable> initClientCash;
private:
    void CreateSession(std::function<void(std::shared_ptr<Session>)> handler, size_t byteLen);
    InitSession(tcp::socket &&socket, std::shared_ptr<ConnectionManagementCash>, std::shared_ptr<InitClientCashTable>, std::function<void(std::shared_ptr<Session>)> handler);
public:
    static void CreateNewSession(tcp::socket &&socket, std::shared_ptr<ConnectionManagementCash>, std::shared_ptr<InitClientCashTable>,
            std::function<void(std::shared_ptr<Session>)> handler);
};

