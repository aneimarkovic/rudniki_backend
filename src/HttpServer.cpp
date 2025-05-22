// Created by Anei Markovič 26.4.2025
#include "HttpServer.hpp"
#include "Controller/UserController.hpp"
#include "DatabaseHandler.hpp"
#include "RouterUtil/Router.hpp"

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
        http::request<http::string_body> request;
        http::read(socket, buffer, request);
        http::response<http::string_body> response{http::status::ok, request.version()};

        // response.set(http::field::server, "Rudnik http server");
        // response.set(http::field::content_type, "text/plain");
        // response.body() = "Hello rudar from http server";
        // response.prepare_payload();
        // http::write(socket, response);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "ERROR: " << ex.what() << "\n";
    }
}

/*Metoda, ki pošlje zahtevo routerju*/
void HttpServer::getRequest(tcp::socket socket)
{
    // std::cout << "Pošiljam zahtevo na router...\n";

    // bsoncxx::document::view filters{};
    // std::string collName = "mines";
    // std::vector<bsoncxx::document::value> scrapperVec = DatabaseHandler::fetchMultipleDocuments(collName, filters);

    // std::string temp = "{";
    // int counter = 0;
    // for (auto &&i : scrapperVec)
    // {
    //     // std::cout << bsoncxx::to_json(i) << std::endl;
    //     temp += "\"" + std::to_string(counter) + "\":" + bsoncxx::to_json(i);
    //     if(counter < scrapperVec.size()-1){
    //         temp += ",";
    //     }
    //     counter++;
    // }

    // temp += "}";

    // boost::beast::flat_buffer buffer;
    // http::request<http::string_body> req;
    // http::read(socket, buffer, req);

    // http::response<http::string_body> res{http::status::ok, req.version()};
    // res.set(http::field::server, "Rudnik http server");
    // res.set(http::field::content_type, "application/json");
    // res.set(http::field::access_control_allow_origin, "*");
    // res.body() = temp;
    // res.prepare_payload();
    // http::write(socket, res);

    beast::flat_buffer buffer;
    beast::error_code errorCode;
    http::request<http::string_body> req;

    http::read(socket, buffer, req, errorCode);

    if (errorCode == http::error::end_of_stream)
    {
        socket.shutdown(tcp::socket::shutdown_send, errorCode); // Client je zapr connection preden je poslal vse
        return;
    }

    if (errorCode)
    {
        std::cerr << "Napaka pri branju zahteve: " << errorCode.message() << std::endl;
        return;
    }

    http::response<http::string_body> res;
    res.version(req.version());
    res.keep_alive(req.keep_alive());

    // ROUTER CALL
    Router newRoute;
    newRoute.handleRequest(req, res);

    res.set(http::field::server, "Rudnik http server");
    res.set(http::field::content_type, "application/json");
    res.set(http::field::access_control_allow_origin, "*");
    res.prepare_payload();
    http::write(socket, res)
}

/*Metoda, ki pošlje odgovor nazaj na clientside*/
void HttpServer::send(tcp::socket socket, http::response<http::string_body> response)
{
    std::cout << "Pošiljam odgovor...\n";
    // response.prepare_payload();
    // http::write(socket, response);
}