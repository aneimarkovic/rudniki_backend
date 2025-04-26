// Created by Anei Markovič 26.4.2025
#include <iostream>
#include "HttpServer.hpp"

int main() {
    try{
        HttpServer server("127.0.0.1", "8080");
        server.runServer();
    }catch (const std::exception& ex){
        std::cerr << "ERROR: " << ex.what() << "\n";
    }
}