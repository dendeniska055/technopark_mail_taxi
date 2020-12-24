#include "Session.h"

#include <utility>

Session::Session(uint64_t id, websocket::stream<beast::tcp_stream> &&ws, bool isDriver) : ws(std::move(ws)), id(id), isDriver(isDriver)
{
    partnerOnLine = std::make_shared<std::atomic_bool>(false);
}


void Session::SendCoordinate(const MailTaxi::Coordinate &coordinate, std::function<void(beast::error_code)> handler) {
    std::vector<char> message(coordinate.ByteSizeLong() + 1);
    message[0] = coordinateCode;
    coordinate.SerializeToArray(message.data() + 1, message.size() - 1);
    ws.async_write(net::buffer(message),[handler](beast::error_code ec, std::size_t msgLen){
        handler(ec);
    });
}


uint64_t Session::GetId() const {
    return id;
}

void Session::ChangePartnerOnline(bool indicator) {
    partnerOnLine->store(indicator);
}

void Session::SetPartnerOnline(std::shared_ptr<std::atomic_bool> partnerOnLine) {
    this->partnerOnLine = std::move(partnerOnLine);
}

void Session::ReceiveCoordinate(std::function<void(MailTaxi::Coordinate &, beast::error_code, bool)> handler) {
    readBuffer.consume(readBuffer.size());
    ws.async_read(readBuffer, [this, handler](beast::error_code ec, std::size_t msgLen){
        MailTaxi::Coordinate coordinate;
        bool parse = coordinate.ParseFromArray(readBuffer.data().data(), msgLen);
        handler(coordinate, ec, parse);
    });
}

bool Session::IsDriver() const {
    return isDriver;
}

bool Session::PartnerOnLine() const {
    return partnerOnLine->load();
}

void
Session::SendOrderInformation(const MailTaxi::OrderLocationData &locationData, std::function<void(beast::error_code)> handler) {
    std::vector<char> message(locationData.ByteSizeLong() + 1);
    message[0] = orderInformationCode;
    locationData.SerializeToArray(message.data() + 1, message.size() - 1);
    ws.async_write(net::buffer(message),[handler](beast::error_code ec, std::size_t msgLen){
        handler(ec);
    });
}

void Session::SendString(const std::string &str, std::function<void(beast::error_code)> handler) {
    std::string code;
    code.push_back(stringCode);
    ws.async_write(net::buffer(code + str),[handler](beast::error_code ec, std::size_t msgLen){
        handler(ec);
    });
}

void Session::Close() {
    beast::websocket::close_reason closeResponse(beast::websocket::close_code::normal, "Work of!");
    ws.async_close(closeResponse, [](beast::error_code const& err){});
}

void Session::Close(std::function<void(beast::error_code)> handler) {
    beast::websocket::close_reason closeResponse(beast::websocket::close_code::normal, "Work of!");
    ws.async_close(closeResponse, [handler](beast::error_code const& err){
        handler(err);
    });
}


