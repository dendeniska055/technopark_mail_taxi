#include <SingleContext.h>
#include "Connaction.h"
Cone::Web::Connection::Connection() : socket(Cone::Web::Context::SingleContext::GetSingleContext()->GetContext()){
}

std::shared_ptr<Cone::Web::Connection> Cone::Web::Connection::NewConnection() {
    return std::shared_ptr<Connection>(new Connection);
}

boost::asio::ip::tcp::socket &Cone::Web::Connection::Socket() {
    return socket;
}

void Cone::Web::Connection::Receive(std::shared_ptr<std::string> buffer,
                         std::function<void(const boost::system::error_code &, size_t)> handler) {
    socket.async_read_some(boost::asio::buffer(*buffer, buffer->size()), handler);
}

void Cone::Web::Connection::Send(std::shared_ptr<std::string> buffer,
                      std::function<void(const boost::system::error_code &, size_t)> handler) {
    socket.async_write_some(boost::asio::buffer(*buffer, buffer->length()), handler);
}

Cone::Web::Connection::~Connection() {
    if (socket.is_open())
        socket.close();
}

void Cone::Web::Connection::Close() {
    socket.close();
}
