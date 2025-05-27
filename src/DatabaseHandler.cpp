// Created by Anei Markovič 4.5.2025
#include "DatabaseHandler.hpp"

mongocxx::instance DatabaseHandler::instance{};
mongocxx::uri DatabaseHandler::uri("mongodb+srv://darkosever:KeriBurazi69@imerudniki.a8kpflt.mongodb.net/?retryWrites=true&w=majority&appName=ImeRudniki");
mongocxx::options::client DatabaseHandler::clientOptions{};
mongocxx::client DatabaseHandler::connection = DatabaseHandler::createClientWithApi(DatabaseHandler::uri, DatabaseHandler::clientOptions);
mongocxx::database DatabaseHandler::db = DatabaseHandler::connection["ImeRudnikiDatabase"];

mongocxx::client DatabaseHandler::createClientWithApi(const mongocxx::uri &uri, mongocxx::options::client &options)
{
    mongocxx::options::server_api api_opts{mongocxx::options::server_api::version::k_version_1};
    options.server_api_opts(api_opts);
    return mongocxx::client{uri, options};
}

// DatabaseHandler::DatabaseHandler(const std::string &uriStr, const std::string &dbName)
//     : uri(uriStr), connection(createClientWithApi(uri, clientOptions)), db(connection[dbName])
// {
//     const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
//     this->db.run_command(ping_cmd.view());
//     std::cout << "Pinged your deployment." << std::endl;

//     std::cout << "Povezava z podatkovno bazo vzpostavljena!\n";
// }

bool DatabaseHandler::insertDocument(const std::string &collectionName, const bsoncxx::document::value document)
{
    try
    {
        mongocxx::collection collection = DatabaseHandler::db[collectionName];
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
        mongocxx::collection collection = DatabaseHandler::db[collectionName];
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
        mongocxx::collection collection = DatabaseHandler::db[collectionName];
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
        mongocxx::collection collection = DatabaseHandler::db[collectionName];

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
std::vector<bsoncxx::document::value> DatabaseHandler::fetchMultipleDocuments(const std::string &collectionName, bsoncxx::document::view filters)
{
    std::vector<bsoncxx::document::value> documents;
    try
    {
        mongocxx::collection collection = DatabaseHandler::db[collectionName];

        mongocxx::cursor cursor = collection.find(filters);

        for (bsoncxx::document::view doc_view : cursor)
        {
            documents.emplace_back(doc_view);
        }

        return documents;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Splošna napaka pri branju več dokumentov iz zbirke '" << collectionName << "': " << e.what() << std::endl;
        return {};
    }
}
// Funkija zbriše dokument na podlagi filtrov
bool DatabaseHandler::deleteDocument(const std::string &collectionName, bsoncxx::document::view filters)
{
    try
    {
        mongocxx::collection collection = DatabaseHandler::db[collectionName];
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
        mongocxx::collection collection = DatabaseHandler::db[collectionName];
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

bool DatabaseHandler::create2dsphereIndex(const std::string& collectionName, const std::string& fieldName)
{
    try {
        mongocxx::collection collection = DatabaseHandler::db[collectionName];
        bsoncxx::builder::basic::document indexDoc{};
        indexDoc.append(bsoncxx::builder::basic::kvp(fieldName, "2dsphere"));

        auto result = collection.create_index(indexDoc.view());

        return true;
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
        return false;
    }
}