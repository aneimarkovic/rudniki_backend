/*
 Created by Anei Markovič 4.5.2025

 Razred DatabaseHandler, skrbi za povezavo z podatkovno bazo mongodb.
 Razred vzpostavi povezavo z podatkovno bazo z konstruktorjem.
 Z metodami vstavljamo in pridobivamo podatke iz baze
*/
#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include "Model/PointModel.hpp"

#include <iostream>
#include <string>
#include <optional>
#include <vector>

class DatabaseHandler
{
private:
    static std::unique_ptr<mongocxx::instance> instance;
    static mongocxx::uri uri;
    static mongocxx::options::client clientOptions;
    static std::shared_ptr<mongocxx::pool> pool;
    const static std::string dbName;

    static mongocxx::client createClientWithApi(const mongocxx::uri &uri, mongocxx::options::client &options);

public:
    static void initialize();
    static mongocxx::pool::entry getClient();

    static bool insertDocument(const std::string &collectionName, const bsoncxx::document::value document);
    static std::optional<bsoncxx::oid> insertDocumentGetInsertId(const std::string &collectionName, const bsoncxx::document::value document);

    static bool deleteDocument(const std::string &collectionName, bsoncxx::document::view filters);
    static bool updateOneItem(const std::string &collectionName, bsoncxx::document::view filters, bsoncxx::document::view update);
    static std::optional<bsoncxx::document::value> fetchSingleDocument(const std::string &collectionName, bsoncxx::document::view filters);
    static std::vector<bsoncxx::document::value> fetchMultipleDocuments(const std::string &collectionName, bsoncxx::document::view filters, bsoncxx::document::view projection = bsoncxx::document::view{});
    static std::vector<bsoncxx::document::value> getSpecificColumnFromDocument(const std::string &collectionName, const mongocxx::options::find& column, const bsoncxx::document::value& filters);
    static std::vector<bsoncxx::document::value> fetchMultipleDocumentsAggregate(const std::string &collectionName, const mongocxx::pipeline &pipeline);

    static bool create2dsphereIndex(const std::string& collectionName, const std::string& fieldName);
};
#endif