#include "InitSession.h"

#include <utility>
#include "ConnectionManagementCash.h"
#include "InitClientCashTable.h"
InitSession::InitSession(tcp::socket &&socket, std::shared_ptr<ConnectionManagementCash> a,
                         std::shared_ptr<InitClientCashTable> b, std::function<void(std::shared_ptr<Session>)> handler)
        : ws(std::move(socket)), connectionManagementCash(std::move(a)), initClientCash(std::move(b)) {
    net::dispatch(ws.get_executor(),[this, handler](){
        ws.set_option(
                websocket::stream_base::timeout::suggested(
                        beast::role_type::server));

        ws.set_option(websocket::stream_base::decorator(
                [](websocket::response_type& res)
                {
                    res.set(http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-server-async");
                }));
        ws.binary(true);
        ws.async_accept([this, handler](beast::error_code ec){
            if (!ec) {
                ws.async_read(readBuffer, [this, handler](beast::error_code ec, std::size_t bSize) {
                    std::cout << bSize << std::endl;
                    if (!ec) {
                        CreateSession(handler, bSize);
                    }
                    else{
                        delete this;
                    }
                });
            }
            else{
                delete this;
            }
        });

    });
}
//                                       122-1261486
//210949505152535455565716-57-21-24-1114 119-689285
//210949505152535455565716-57-21-24-1114  нормальное сообщение клиента
//1101049494949494949494949161            нормальное сообщение водителя
void InitSession::CreateSession(std::function<void(std::shared_ptr<Session>)> handler, size_t byteLen) {
    auto code = static_cast<char*>( readBuffer.data().data())[0];
    std::cout << std::endl;
    std::cout << readBuffer.size() << std::endl;
    MailTaxi::IdentificationData identificationData;
    bool successfulParse = identificationData.ParseFromArray(static_cast<char*>( readBuffer.data().data()) + 1, byteLen - 1);
    SessionPtr session;
    std::cout << "NEW USER" << std::endl;
    if (successfulParse) {
        std::cout << "УСПЕШНО ЗАЛОГИНЕЛСЯ" << std::endl;
        std::cout << "id " << identificationData.id() << std::endl;
        std::cout << identificationData.token() << std::endl;
        if (code== DRIVER_CODE) {
            SessionPtr driverSession(new Session(identificationData.id(), std::move(ws), true));
            if (connectionManagementCash->DriverConnect(identificationData.id(), driverSession, identificationData.token()))
                handler(driverSession);
        }
        else if (code == CLIENT_CODE) {
            auto confirmInfo = initClientCash->Confirm(identificationData.id(), identificationData.token());
            if (confirmInfo.first){
                SessionPtr clientSession(new Session(confirmInfo.second, std::move(ws), false));
                if (connectionManagementCash->ClientConnect(clientSession->GetId(), clientSession))
                    handler(clientSession);
            }
        }
    }
    else{
        std::cout << "НЕУСПЕШНО ЗАЛОГИНЕЛСЯ" << std::endl;
    }
    delete this;
}

void InitSession::CreateNewSession(tcp::socket &&socket,std::shared_ptr<ConnectionManagementCash> a,
                                   std::shared_ptr<InitClientCashTable> b, std::function<void(std::shared_ptr<Session>)> handler) {
   new InitSession(std::move(socket), std::move(a), std::move(b), std::move(handler));
}