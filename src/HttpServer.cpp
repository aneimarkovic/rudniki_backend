#include "HttpServer.hpp"

HttpServer::HttpServer(const std::string &ipAddress, const std::string &port) : acceptor(ioContext, tcp::endpoint(net::ip::make_address(ipAddress), std::stoi(port))),
                                                                                 socket(ioContext)
{
    std::cout << "Server listening on: http://" << ip_address << ":" << port << "\n";
}

void HttpServer::runServer()
{
    while (1)
    {
        this->acceptor.accept(this->socket);
        std::thread{&HttpServer::getRequest, this, std::move(socket)}.detach();
    }
}

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

void HttpServer::getRequest(tcp::socket socket){
    std::cout << "Pošiljam zahtevo na router...\n";
}