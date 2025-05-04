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