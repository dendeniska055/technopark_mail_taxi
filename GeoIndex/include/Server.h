#pragma once
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
class Session{
private:
    tcp::socket socket;
private:
    std::string receive();
    void send(std::string message);
public:
    Session(boost::asio::io_context& context);
    tcp::socket& GetSocket();
    void HandleSession();
};
class Server {
private:
    boost::asio::io_service io_service;
    tcp::acceptor acceptor;
public:
    Server(short int portNum);
    bool Run(short threadNum, short port);
    bool Stop();
};

