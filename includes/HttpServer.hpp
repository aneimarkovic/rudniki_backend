#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <thread>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class HttpServer
{
private:
    net::io_context ioContext;
    tcp::acceptor acceptor;
    tcp::socket socket;

public:
    HttpServer(const std::string &ipAddress, const std::string &port);
    void runServer();
    void parseRequest(tcp::socket socket);
    void getRequest(tcp::socket socket);
};
#endif