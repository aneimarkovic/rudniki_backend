// Created by Anei Markovič 26.4.2025
#include <iostream>
#include "HttpServer.hpp" 
#include "Model/UserModel.hpp"
#include "../test/UserModelTest.cpp"

int main() {
    try{
        testUserValidation();
        HttpServer server("127.0.0.1", "8080");
        server.runServer();
    }catch (const std::exception& ex){
        std::cerr << "ERROR: " << ex.what() << "\n";
    }
}