#include "Acceptor.h"
#include "SingleContext.h"
using  boost::asio::ip::tcp;
Cone::Web::Acceptor::Acceptor(uint16_t port) : acceptor(Cone::Web::Context::SingleContext::GetSingleContext()->GetContext(),
                                             tcp::endpoint(tcp::v6(), port)){
}

void Cone::Web::Acceptor::Accept(std::shared_ptr<Connection> connection, std::function<void(const boost::system::error_code &)> handler) {
    acceptor.async_accept(connection->Socket(), handler);
}
