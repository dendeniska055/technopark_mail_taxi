#pragma once
#include <variant>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <memory>
#include "OrderManagement.grpc.pb.h"
#include "SessionCodes.h"
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Session;

typedef std::shared_ptr<Session> SessionPtr;

;

class Session {
private:
    websocket::stream<beast::tcp_stream> ws;
    beast::flat_buffer readBuffer;
    std::shared_ptr<std::atomic_bool> partnerOnLine;
    uint64_t id;
    bool isDriver;
public:
    explicit Session(uint64_t id, websocket::stream<beast::tcp_stream> &&ws, bool isDriver);
    ~Session() = default;
    void SendCoordinate(const MailTaxi::Coordinate &coordinate, std::function<void(beast::error_code)> handler);
    void SendOrderInformation(MailTaxi::ProposeOrderRequest &locationData, std::function<void(beast::error_code)> handler);
    void SendString(const std::string &string, std::function<void(beast::error_code)> handler);
    void ReceiveCoordinate(std::function<void(MailTaxi::Coordinate&, beast::error_code err, bool)> handler);
    void ChangePartnerOnline(bool);
    void SetPartnerOnline( std::shared_ptr<std::atomic_bool> partnerOnLine);
    void Close();
    void Close(std::function<void(beast::error_code)>);
    [[nodiscard]] bool IsDriver() const;
    [[nodiscard]] uint64_t GetId() const;
    [[nodiscard]] bool PartnerOnLine() const;
};
