// Created by Anei Markovič 26.4.2025
#include "HttpServer.hpp"
#include "Controller/UserController.hpp"
#include "DatabaseHandler.hpp"
#include "RouterUtil/Router.hpp"
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <fstream>

std::string HttpServer::accessToken = "";
std::string HttpServer::projectId = "";

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
    beast::tcp_stream stream(std::move(socket));

    beast::flat_buffer buffer;
    beast::error_code errorCode;
    http::request<http::string_body> req;

    http::read(stream, buffer, req, errorCode);

    if (errorCode == http::error::end_of_stream)
    {
        stream.socket().shutdown(tcp::socket::shutdown_send, errorCode); // Client je zapr connection preden je poslal vse
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

    // ROUTER CALL - pass the stream
    Router newRoute;
    try{
        newRoute.handleRequest(req, res, &stream);
    } catch (const std::exception &ex){
        std::cout << "NAPAKA PRI HANDLANJU REQ: " << ex.what() << std::endl;
    }

    if (!websocket::is_upgrade(req)) {
        res.set(http::field::server, "Rudnik http server");
        res.set(http::field::content_type, "application/json");
        res.set(http::field::access_control_allow_credentials, "true");
        res.set(http::field::access_control_allow_origin, "http://127.0.0.1:3000");
        res.set(http::field::access_control_allow_headers, "Content-Type, Authorization, X-Requested-With, Accept");
        res.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
        res.prepare_payload();
        try{
            http::write(stream, res);
        }catch (const std::exception &ex){
            std::cout << "NAPAKA PRI PISANJU RES: " << ex.what() << std::endl;
        }
    }
}

/*Metoda, ki pošlje odgovor nazaj na clientside*/
void HttpServer::send(tcp::socket socket, http::response<http::string_body> response)
{
    std::cout << "Pošiljam odgovor...\n";
    // response.prepare_payload();
    // http::write(socket, response);
}

//Funkcija, ki naredi JWT z podatki uporabnik (ID...)
std::string HttpServer::createJWT(bsoncxx::oid userId)
{
    std::string publicKey = "d8700e1e557dd94d3a5d95f96995cff7367c8dc7fefa766ba262402b4fa9a230ce816d0dedfedb03f2579ae538f79c762ffab3f976d3f3df7ea384d502a93d9c0d46cc70bda25293dc8b4b6e74b87bc8050c6cd169d1f5a735a83ba43db78d3b17bc9ae4f96c3c75c567a559cf1e13dca7b35aeed3bfad15e50d0c192bcdaa059ce553761fa98f025fe7512a25ee18621a4b90dca35f5a7e8492baed76b618115bec637a1e532a4280b100d63e0f758b03af7a1afcbeb6c7c37ceb6d3392b10a217cb09900132d35aa45fed6626bfba629060d72cf33ee74bbda232fe84c85730391a01e047c240d475d482e72550be3d2d43111c97e316ac411f77ecbc000c8";
    std::string privateKey = "c10c8634a206365af5d6e700b8299deacd8f0b0250544ae3381595c91a408c3b8d9517afadf9edd48b5fa362e8f4d518f6f3cf3f68e4e09a0840733fce208ee1d13195aeca563fa8da338abe078006602bbffe2f82f26e2ba1030c822fffd000750efb1a3a2dc8f455958d9b1ad22190aa34bef118c8ded75b97c4be9f8b9ab91771db293a453718acfe5e8acca420efd063ce590f5b7d86a3c066c7de8b078b2ebec65a74335bd4d27760d5d933b42b6512cb7f14e65ea065fded58bb3e9c63df9638a0fdd06c9a225aaa82add19596247475cf7e340be7831c6a6f9038e3c16e957ddd11c7d002d713a2df4df814129d2a4ba4b23fd6c16782948dbe7c803d";
    auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    auto expirationTime = currentTime + std::chrono::seconds{604800};

    std::string token = jwt::create()
            .set_type("JWT")
            .set_issuer("server")
            .set_payload_claim("sub", jwt::claim(std::string(publicKey)))
            .set_payload_claim("issued_time", jwt::claim(picojson::value(currentTime.count())))
            .set_payload_claim("expiration_time", jwt::claim(picojson::value(expirationTime.count())))
            .set_payload_claim("user", jwt::claim(userId.to_string()))
            .sign(jwt::algorithm::hs256{privateKey});
    return token;
}

std::string sendJwtToOAuth(std::string jwt);

//Funkcija, ki bo naredila JWT iz
void HttpServer::createOAuthJWT() {
    auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    auto expirationTime = currentTime + std::chrono::seconds{3600};

    std::ifstream f("../rudnikipora_firebase_key.json");
    if (!f.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
    }

    nlohmann::json data = nlohmann::json::parse(f);
    // std::cout << data << std::endl;
    // std::cout << data["type"] << std::endl;

    std::string privateKey = data["private_key"].get<std::string>();

    HttpServer::projectId = data["project_id"].get<std::string>();

    size_t pos = 0;
    while ((pos = privateKey.find("\\n", pos)) != std::string::npos) {
        privateKey.replace(pos, 2, "\n");
        pos += 1;
    }

    std::string token = jwt::create()
    .set_type("JWT")
    .set_issuer(data["client_email"])
    .set_subject(data["client_email"])
    .set_audience("https://oauth2.googleapis.com/token")
    .set_payload_claim("scope", jwt::claim(std::string("https://www.googleapis.com/auth/firebase.messaging")))
    .set_payload_claim("iat", jwt::claim(picojson::value(currentTime.count())))
    .set_payload_claim("exp", jwt::claim(picojson::value(expirationTime.count())))
    .sign(jwt::algorithm::rs256("", privateKey, ""));

    try {
        HttpServer::accessToken = sendJwtToOAuth(token);
    } catch (std::exception &e) {
        std::cerr << "ERROR OAUTH: " << e.what() << std::endl;
    }
}

std::string sendJwtToOAuth(std::string jwt) {
    std::string accessToken = "";
    try {
        net::io_context ioc;
        net::ssl::context ctx {net::ssl::context::tlsv12_client};
        tcp::resolver resolver{ ioc };

        boost::beast::ssl_stream<beast::tcp_stream> stream{ ioc, ctx };
        beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));

        //Server name indication
        if (!SSL_set_tlsext_host_name(stream.native_handle(), "oauth2.googleapis.com")) {
            throw std::runtime_error("ERROR with server name indication");
        }

        boost::asio::ip::basic_resolver_results<tcp> const results = resolver.resolve("oauth2.googleapis.com", "443");

        boost::beast::get_lowest_layer(stream).connect(results);
        stream.handshake(net::ssl::stream_base::client);

        std::string bodyData = "grant_type=urn:ietf:params:oauth:grant-type:jwt-bearer&assertion=" + jwt;

        http::request<http::string_body> req{http::verb::post, "/token", 11};
        req.set(http::field::host, "oauth2.googleapis.com");
        req.set(http::field::user_agent, "Rudniki/1.0");
        req.set(http::field::content_type, "application/x-www-form-urlencoded");

        req.body() = bodyData;
        req.prepare_payload();
        try {
            http::write(stream, req);
        } catch (std::exception &e) {
            std::cerr << "ERROR sending JWT to OAuth: " << e.what() << std::endl;
            throw std::runtime_error("ERROR sending jwt to OAuth");
        }

        beast::flat_buffer buffer;
        http::response<http::string_body> res;

        try {
            http::read(stream, buffer, res);
        } catch (std::exception &e) {
            std::cerr << "ERROR reading response from OAuth: " << e.what() << std::endl;
            throw std::runtime_error("ERROR reading response from OAuth");
        }

        // std::cout << "OAuth response:  "<< res.body() << std::endl;
        nlohmann::json data = nlohmann::json::parse(res.body());
        // std::cout << "OAUTH RESPONSE: " << data << std::endl;

        accessToken = data["access_token"].get<std::string>();

        beast::error_code ec;
        stream.shutdown(ec);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR]: " << e.what() << std::endl;
        throw std::runtime_error("ERROR while doing OAuth");
    }

    return accessToken;
}

//Funkcija, ki preveri in verifya jwt
bool HttpServer::verifyJWT(std::string &token)
{
    try
    {
        std::string privateKey = "c10c8634a206365af5d6e700b8299deacd8f0b0250544ae3381595c91a408c3b8d9517afadf9edd48b5fa362e8f4d518f6f3cf3f68e4e09a0840733fce208ee1d13195aeca563fa8da338abe078006602bbffe2f82f26e2ba1030c822fffd000750efb1a3a2dc8f455958d9b1ad22190aa34bef118c8ded75b97c4be9f8b9ab91771db293a453718acfe5e8acca420efd063ce590f5b7d86a3c066c7de8b078b2ebec65a74335bd4d27760d5d933b42b6512cb7f14e65ea065fded58bb3e9c63df9638a0fdd06c9a225aaa82add19596247475cf7e340be7831c6a6f9038e3c16e957ddd11c7d002d713a2df4df814129d2a4ba4b23fd6c16782948dbe7c803d";
        auto decoded = jwt::decode(token);

        // std::cout << "Header:\n"
        //           << decoded.get_header() << std::endl;
        // std::cout << "Payload:\n"
        //           << decoded.get_payload() << std::endl;

        // if (decoded.has_payload_claim("user"))
        // {
        //     std::string user = decoded.get_payload_claim("user").as_string();
        //     std::cout << "user: " << user << std::endl;
        // }
        // else
        // {
        //     std::cout << "Claim 'user' ne obstaja." << std::endl;
        // }

        auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{privateKey})
                .with_issuer("server");
        verifier.verify(decoded);
        return true;
    }
    catch (const std::exception &e)
    {
        // std::cout << "Potrebna: " << e.what() << "\n";
        return false;
    }
}