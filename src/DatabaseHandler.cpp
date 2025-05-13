// Created by Anei Markovič 4.5.2025
#include "DatabaseHandler.hpp"

DatabaseHandler::DatabaseHandler(const std::string &uriStr, const std::string &dbName)
    : uri(uriStr), connection(createClientWithApi(uri, clientOptions)), db(connection[dbName])
{
    const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
    this->db.run_command(ping_cmd.view());
    std::cout << "Pinged your deployment." << std::endl;

    std::cout << "Povezava z podatkovno bazo vzpostavljena!\n";
}

bool DatabaseHandler::insertDocument(const std::string &collectionName, const bsoncxx::document::value document)
{
    try
    {
        mongocxx::collection collection = this->db[collectionName];
        collection.insert_one(document.view());
        std::cout << "Dokument pravilno vstavljen!\n";
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Napaka pri vstavljanju dokumenta: " << e.what() << std::endl;
        return false;
    }
}