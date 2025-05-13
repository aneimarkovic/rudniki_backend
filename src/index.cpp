// Created by Anei Markovič 26.4.2025
#include <bsoncxx/builder/stream/document.hpp>
#include <iostream>
#include <chrono>
#include <ctime>
#include "HttpServer.hpp"
#include "DatabaseHandler.hpp"

void testDatabaseInsert(DatabaseHandler &db)
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
    std::cout << (db.insertDocument("user", document) ? "uspešno!" : "neuspešno!") << std::endl;
}

int main()
{
    try
    {
        std::string dbUri = "<INSERT_URI>";
        std::string dbName = "mines";
        DatabaseHandler db(dbUri, dbName);
        testDatabaseInsert(db);
        HttpServer server("127.0.0.1", "8080");
        server.runServer();

    }
    catch (const std::exception &ex)
    {
        std::cerr << "ERROR: " << ex.what() << "\n";
    }
}