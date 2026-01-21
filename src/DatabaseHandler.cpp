// Created by Anei Markovič 4.5.2025
#include "DatabaseHandler.hpp"

#include <mongocxx/options/find.hpp>

std::unique_ptr<mongocxx::instance> DatabaseHandler::instance = nullptr;
mongocxx::uri DatabaseHandler::uri("mongodb+srv://darkosever:KeriBurazi69@imerudniki.a8kpflt.mongodb.net/?retryWrites=true&w=majority&appName=ImeRudniki");
mongocxx::options::client DatabaseHandler::clientOptions{};
std::shared_ptr<mongocxx::pool> DatabaseHandler::pool = nullptr;
const std::string DatabaseHandler::dbName = "ImeRudnikiDatabase";

mongocxx::client DatabaseHandler::createClientWithApi(const mongocxx::uri &uri, mongocxx::options::client &options)
{
    mongocxx::options::server_api api_opts{mongocxx::options::server_api::version::k_version_1};
    options.server_api_opts(api_opts);
    return mongocxx::client{uri, options};
}

void DatabaseHandler::initialize() {
    if (instance == nullptr) {
        instance = std::make_unique<mongocxx::instance>();
    }

    pool = std::make_shared<mongocxx::pool>(uri, clientOptions);
}

mongocxx::pool::entry DatabaseHandler::getClient() {
    return pool->acquire();
}

bool DatabaseHandler::insertDocument(const std::string &collectionName, const bsoncxx::document::value document)
{
    try
    {
        const auto client = getClient();
        const auto db = (*client)[dbName];
        auto collection = db[collectionName];

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
//Funkcija, ki vrne insertId namesto bool
std::optional<bsoncxx::oid> DatabaseHandler::insertDocumentGetInsertId(const std::string &collectionName, const bsoncxx::document::value document)
{
    try
    {
        const auto client = getClient();
        const auto db = (*client)[dbName];
        auto collection = db[collectionName];

        auto result = collection.insert_one(document.view());
        std::cout << "Dokument pravilno vstavljen!\n";
        bsoncxx::types::bson_value::view id = result->inserted_id();
        if (id.type() == bsoncxx::type::k_oid) {
            bsoncxx::oid oid = id.get_oid().value;
            return oid;
        }

        return std::nullopt;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Napaka pri vstavljanju dokumenta: " << e.what() << std::endl;
        return std::nullopt;
    }
}
// Funkcija za posodabljanje ene vrednosti v dokumentu
bool DatabaseHandler::updateOneItem(const std::string &collectionName, bsoncxx::document::view filters, bsoncxx::document::view update)
{
    try
    {
        const auto client = getClient();
        const auto db = (*client)[dbName];
        auto collection = db[collectionName];

        auto result = collection.update_one(filters, update);
        std::cout << "Posodobljenih dokumentov: " << result->modified_count() << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Napaka pri posodabljanju dokumenta: " << e.what() << std::endl;
        return false;
    }
}

/*
    Funkcija vzame [collectionName] in [filters] in glede na to vrne en objekt če ga najde
*/
std::optional<bsoncxx::document::value> DatabaseHandler::fetchSingleDocument(const std::string &collectionName, bsoncxx::document::view filters)
{
    try
    {
        const auto client = getClient();
        const auto db = (*client)[dbName];
        auto collection = db[collectionName];


        bsoncxx::stdx::optional<bsoncxx::document::value> driver_find_one_result = collection.find_one(filters);

        std::optional<bsoncxx::document::value> maybe_result;

        if (driver_find_one_result)
        {
            maybe_result = std::move(*driver_find_one_result);
        }

        if (maybe_result)
        {
            return maybe_result;
        }
        else
        {
            return {};
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Napaka pri branju dokumenta: " << e.what() << std::endl;
    }
}

/*
    Funkcija vzame [collectionName] in [filters] in glede na to vrne več objekt če jih najde
*/
std::vector<bsoncxx::document::value> DatabaseHandler::fetchMultipleDocuments(const std::string &collectionName, bsoncxx::document::view filters, bsoncxx::document::view projection)
{
    std::vector<bsoncxx::document::value> documents;
    try
    {
        const auto client = getClient();
        const auto db = (*client)[dbName];
        auto collection = db[collectionName];

        mongocxx::options::find options;

        if (!projection.empty()) {
            options.projection(projection);
        }

        mongocxx::cursor cursor = collection.find(filters, options);

        for (bsoncxx::document::view doc_view : cursor)
        {
            documents.emplace_back(doc_view);
        }

        return documents;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Splošna napaka pri branju več dokumentov iz zbirke '" << collectionName << "': " << e.what() << std::endl;
        return {};
    }
}

//Funkcija, ki uporablja funckionalnost aggregate za računanje min, max, avg...
std::vector<bsoncxx::document::value> DatabaseHandler::fetchMultipleDocumentsAggregate(const std::string &collectionName, const mongocxx::pipeline &pipeline){
    std::vector<bsoncxx::document::value> documents;
    try{
        const auto client = getClient();
        const auto db = (*client)[dbName];
        auto collection = db[collectionName];


        mongocxx::cursor cursor = collection.aggregate(pipeline);

        for (bsoncxx::document::view doc_view : cursor)
        {
            documents.emplace_back(doc_view);
        }

        return documents;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Splošna napaka pri branju več dokumentov iz zbirke (aggregate) '" << collectionName << "': " << e.what() << std::endl;
        return {};
    }
}
// Funkija zbriše dokument na podlagi filtrov
bool DatabaseHandler::deleteDocument(const std::string &collectionName, bsoncxx::document::view filters)
{
    try
    {
        const auto client = getClient();
        const auto db = (*client)[dbName];
        auto collection = db[collectionName];

        auto result = collection.delete_one(filters);
        std::cout << "Izbrisanih dokumentov: " << result->deleted_count() << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Napaka pri brisanju dokumenta: " << e.what() << std::endl;
        return false;
    }
}

//Funkcija, ki pridobi specifičen stolpec iz dokumenta
std::vector<bsoncxx::document::value> DatabaseHandler::getSpecificColumnFromDocument(const std::string &collectionName, const mongocxx::options::find& column, const bsoncxx::document::value& filters){
    try
    {
        std::vector<bsoncxx::document::value> documents;
        const auto client = getClient();
        const auto db = (*client)[dbName];
        auto collection = db[collectionName];

        mongocxx::cursor cursor = collection.find(filters.view(), column);

        for (bsoncxx::document::view doc_view : cursor)
        {
            documents.emplace_back(doc_view);
        }

        return documents;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Napaka pri pridobivanju dokumenta: " << e.what() << std::endl;
    }
}

//Funkcija,ki iz tabele bordersTable ustvari 2dsphere index za geospation queries
bool DatabaseHandler::create2dsphereIndex(const std::string& collectionName, const std::string& fieldName)
{
    try {
        const auto client = getClient();
        const auto db = (*client)[dbName];
        auto collection = db[collectionName];

        bsoncxx::builder::basic::document indexDoc{};
        indexDoc.append(bsoncxx::builder::basic::kvp(fieldName, "2dsphere"));

        auto result = collection.create_index(indexDoc.view());

        return true;
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
        return false;
    }
}