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

void InitSession::CreateSession(std::function<void(std::shared_ptr<Session>)> handler, size_t byteLen) {
    auto code = static_cast<char*>( readBuffer.data().data())[0];
    MailTaxi::IdentificationData identificationData;
    bool successfulParse = identificationData.ParseFromArray(static_cast<char*>( readBuffer.data().data()) + 1, byteLen - 1);
    SessionPtr session;
    if (successfulParse) {
        if (code== DRIVER_CODE) {
            SessionPtr driverSession(new Session(identificationData.id(), std::move(ws), true));
            if (connectionManagementCash->DriverConnect(identificationData.id(), driverSession, identificationData.token())) {
                std::cout << "Водитель с id " << identificationData.id() << " успешно залогинился." << std::endl;
                handler(driverSession);
            }
        }
        else if (code == CLIENT_CODE) {
            auto confirmInfo = initClientCash->Confirm(identificationData.id(), identificationData.token());
            if (confirmInfo.first){
                SessionPtr clientSession(new Session(confirmInfo.second, std::move(ws), false));
                if (connectionManagementCash->ClientConnect(clientSession->GetId(), clientSession)) {
                    handler(clientSession);
                    std::cout << "Клиент с id " << identificationData.id() << " успешно залогинился." << std::endl;
                }
            }
        }
    }
    delete this;
}

void InitSession::CreateNewSession(tcp::socket &&socket,std::shared_ptr<ConnectionManagementCash> a,
                                   std::shared_ptr<InitClientCashTable> b, std::function<void(std::shared_ptr<Session>)> handler) {
   new InitSession(std::move(socket), std::move(a), std::move(b), std::move(handler));
}