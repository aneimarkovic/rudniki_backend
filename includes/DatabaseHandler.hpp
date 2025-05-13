/*
 Created by Anei Markovič 4.5.2025

 Razred DatabaseHandler, skrbi za povezavo z podatkovno bazo mongodb.
 Razred vzpostavi povezavo z podatkovno bazo z konstruktorjem.
 Z metodami vstavljamo in pridobivamo podatke iz baze
*/
#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <iostream>
#include <string>

class DatabaseHandler
{
private:
    mongocxx::instance instance;
    mongocxx::uri uri;
    mongocxx::options::client clientOptions;
    mongocxx::client connection;
    mongocxx::database db;
    static mongocxx::client createClientWithApi(const mongocxx::uri &uri, mongocxx::options::client &options){
        mongocxx::options::server_api api_opts{mongocxx::options::server_api::version::k_version_1};
        options.server_api_opts(api_opts);
        return mongocxx::client{uri, options};
    }

public:
    DatabaseHandler(const std::string &uriStr, const std::string &dbName);
    bool insertDocument(const std::string& collectionName, const bsoncxx::document::value document);
};
#endif