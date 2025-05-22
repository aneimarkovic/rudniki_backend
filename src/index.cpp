// Created by Anei Markovič 26.4.2025
#include <bsoncxx/builder/stream/document.hpp>
#include <iostream>
#include <chrono>
#include <ctime>
#include "HttpServer.hpp"
#include "DatabaseHandler.hpp"

#include "RouterUtil/Router.hpp"
#include "Controller/MineController.hpp"

void testDatabaseInsert()
{
    bsoncxx::builder::stream::document builder;
    auto now = std::chrono::system_clock::now();
    auto timeISO = bsoncxx::types::b_date(now);

    builder << "username" << "Anei"
            << "password" << "Test123"
            << "email" << "test@gmail.com"
            << "birthDate" << "7-6-2004"
            << "created" << timeISO
            << "modified" << timeISO;

    bsoncxx::document::value document = builder.extract();
    std::cout << (DatabaseHandler::insertDocument("user", document) ? "uspešno!" : "neuspešno!") << std::endl;
}

void getAndPrintAllScrapperData()
{
    std::cout << "Pridobivam podatke od scrapperja" << std::endl;
    bsoncxx::document::view filters{};
    std::string collName = "mines";
    std::vector<bsoncxx::document::value> scrapperVec = DatabaseHandler::fetchMultipleDocuments(collName, filters);
    std::string temp = "";
    for (auto &&i : scrapperVec)
    {
        // std::cout << bsoncxx::to_json(i) << std::endl;
        temp += bsoncxx::to_json(i);
    }

    std::cout << temp;
}

int main()
{
    try
    {
        // testDatabaseInsert();
        // getAndPrintAllScrapperData();
        std::string url = "/mine/save";
        Router::createPostRoute(url, MineController::saveMine);

        HttpServer server("127.0.0.1", "8080");
        server.runServer();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "ERROR: " << ex.what() << "\n";
    }
}