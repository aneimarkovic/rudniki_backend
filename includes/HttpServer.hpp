/*
Created by Anei Markovič 26.4.2025

Razred HttpServer skrbi za povezavo med client in server side.
Razred ustvarimo z konstruktorjem, tako da mu podamo ip naslov in port kjer naj posluša.
Zaženemo ga z metodo runServer, ki čaka na zahteve, ko dobi zahtevo jo sprejme in pošlje routerju,
po obdelavi zahteve dobi v funkcijo sen objekt odgovor, ki ga pošlje na client side

*/
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
    void send(tcp::socket socket, http::response<http::string_body> res);
};
#endif