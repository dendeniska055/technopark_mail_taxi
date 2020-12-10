#pragma once
#include <boost/asio.hpp>
#include "Connaction.h"
namespace Cone::Web {
        class Acceptor {
        private:
            boost::asio::ip::tcp::acceptor acceptor;
        public:
            explicit Acceptor(uint16_t port);
            void
            Accept(std::shared_ptr<Connection> connection, std::function<void(const boost::system::error_code &err)>);
        };
    }
