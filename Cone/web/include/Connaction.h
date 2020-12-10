#pragma once
#include "boost/asio.hpp"
namespace Cone::Web {
        class Connection {
        private:
            boost::asio::ip::tcp::socket socket;
        private:
            Connection();
        public:
            static std::shared_ptr<Connection> NewConnection();
            void Receive(std::shared_ptr<std::string> buffer,
                         std::function<void(const boost::system::error_code &, size_t)>);
            void Send(std::shared_ptr<std::string> buffer,
                      std::function<void(const boost::system::error_code &, size_t)> handler);
            boost::asio::ip::tcp::socket &Socket();
            void Close();
            ~Connection();
        };
    }