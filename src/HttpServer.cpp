// Created by Anei Markovič 26.4.2025
#include "HttpServer.hpp"
#include "Controller/UserController.hpp"

HttpServer::HttpServer(const std::string &ipAddress, const std::string &port) : acceptor(ioContext, tcp::endpoint(net::ip::make_address(ipAddress), std::stoi(port))),
                                                                                socket(ioContext)
{
    std::cout << "Server listening on: http://" << ipAddress << ":" << port << "\n";
}

/*Neskončna zanka, ki čaka na zahteve, ko zahteva pride ustvari novo nit (metodo getRequest), ki pošlje zahtevo routerju*/
void HttpServer::runServer()
{
    while (1)
    {
        this->acceptor.accept(this->socket);
        std::thread{&HttpServer::getRequest, this, std::move(socket)}.detach();
    }
}

/*Testna metoda, ki je pomagamapri testiranju ali pride zahteva do strežnika in ali se pošlje odgovor nazaj*/
void HttpServer::parseRequest(tcp::socket socket)
{
    try
    {
        boost::beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);
        http::response<http::string_body> res{http::status::ok, req.version()};

        // res.set(http::field::server, "Rudnik http server");
        // res.set(http::field::content_type, "text/plain");
        // res.body() = "Hello rudar from http server";
        // res.prepare_payload();
        // http::write(socket, res);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "ERROR: " << ex.what() << "\n";
    }
}

/*Metoda, ki pošlje zahtevo routerju*/
void HttpServer::getRequest(tcp::socket socket)
{
    std::cout << "Pošiljam zahtevo na router...\n";
    // boost::beast::flat_buffer buffer;
    // http::request<http::string_body> req;
    // http::read(socket, buffer, req);
}

/*Metoda, ki pošlje odgovor nazaj na clientside*/
void HttpServer::send(tcp::socket socket, http::response<http::string_body> res)
{
    std::cout << "Pošiljam odgovor...\n";
    // res.prepare_payload();
    // http::write(socket, res);
}