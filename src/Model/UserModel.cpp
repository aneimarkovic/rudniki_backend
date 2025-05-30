// Created by �an Misja 09/05/2025
#include "Model/UserModel.hpp"
#include "DatabaseHandler.hpp"

#include <iostream>
#include <sodium.h>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/document/view.hpp>

/*
    Funkcija zgradi UserModel objekt iz pridobljenega BSON dokumenta
*/
void UserModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        this->id = docView["_id"].get_oid().value;
        this->username = extractStringFromBSON(docView, "username");
        this->email = extractStringFromBSON(docView, "email");
        this->password = extractStringFromBSON(docView, "password");
        this->birthDate = extractDateFromBSON(docView, "BirthDate");

        this->created = extractDateFromBSON(docView, "created");
        this->modified = extractDateFromBSON(docView, "modified");

        this->validateUserData(REGISTRATION);
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "BSON Deserialization Error for UserModel: " << e.what() << std::endl;
    }
}

/*
    Funkcija zgradi BSON dokument iz trenutnega UserModel objekta
*/
bsoncxx::document::value UserModel::convertToBsonDocument()
{
    bsoncxx::builder::basic::document builder{};

    builder.append(bsoncxx::builder::basic::kvp("_id", id));
    builder.append(bsoncxx::builder::basic::kvp("username", username));
    builder.append(bsoncxx::builder::basic::kvp("email", email));
    builder.append(bsoncxx::builder::basic::kvp("password_hash", password));
    builder.append(bsoncxx::builder::basic::kvp("created", bsoncxx::types::b_date{created}));
    builder.append(bsoncxx::builder::basic::kvp("modified", bsoncxx::types::b_date{modified}));
    builder.append(bsoncxx::builder::basic::kvp("birthDate", bsoncxx::types::b_date{birthDate}));

    return builder.extract();
}

/*
    Funkcija, ki preveri oz validira podatke uporabnika
*/
bool UserModel::validateUserData(validationType type) const
{

    // USERNAME
    std::regex usernameRegex("^(?:[a-zA-Z0-9_-]){3,}$");
    if (!std::regex_match(this->username, usernameRegex))
    {
        // std::cout << "Username ni ok!\n";
        return false;
    }

    // PASSWORD
    std::regex passwordRegex("^(?=.*[A-Z])(?=.*\\d)(?=.*[@#$%^&*!]).{8,}$");
    if (!std::regex_match(this->password, passwordRegex))
    {
        // std::cout << "Geslo ni ok!\n";
        return false;
    }

    // MAIL
    std::regex emailRegex(R"(^([a-zA-Z0-9_\-\.\+]+)@([a-zA-Z0-9_\-\.\+]+\.[a-zA-Z]{2,})$)");
    if ((type == REGISTRATION || type == UPDATE_EMAIL))
    {
        if (this->email == "" || !std::regex_match(this->email, emailRegex))
        {
            // std::cout << "Mail ni ok!\n";
            return false;
        }
    }

    // BIRTH DATE
    if (type == REGISTRATION && this->birthDate == timeStamp::zero())
    {
        // std::cout << "Birth date ni definiran";
        return false;
    }

    std::string birthDateTime = UserModel::getDateFromMS(this->birthDate);
    std::regex birthDateRegex("^\\d{4}-\\d{1,2}-\\d{1,2}$");
    if (type == REGISTRATION && !std::regex_match(birthDateTime, birthDateRegex))
    {
        // std::cout << "Birth date ni ok!\n";
        return false;
    }

    return true;
}

/*
    Funkcija pridobi iz milisekund datum v obliki yyyy-mm-dd
*/
std::string UserModel::getDateFromMS(timeStamp time)
{
    std::chrono::system_clock::time_point point = std::chrono::system_clock::time_point{time};
    std::time_t timeTemp = std::chrono::system_clock::to_time_t(point);
    std::tm *localTime = std::localtime(&timeTemp);

    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;

    return std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
}

/*
    Funkcija, ki preveri oz avtenticira podatke uporabnika
*/
std::optional<bsoncxx::oid> UserModel::authUserData() const
{
    try {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    bsoncxx::document::value filter = this->username != "" ? make_document(kvp("username", this->username),kvp("password", this->password)) : make_document(
            kvp("email", this->email),
            kvp("password", this->password));

    auto result = DatabaseHandler::fetchSingleDocument("users", filter);

    bsoncxx::document::view resultView = result->view();
        return resultView["_id"].get_oid().value;
    } catch (const std::exception &e){
        return std::nullopt;
    }
}

/*
    Funkcija, ki preveri ali je uporabnik prijavljen
*/
bool UserModel::checkIfUserIsLoggedIn(std::string &jwt)
{
    return HttpServer::verifyJWT(jwt);
}
/*
    Funkcija, ki vrne user id iz jwt
*/
bsoncxx::oid UserModel::getUserIdFromJWT(std::string &jwt)
{
    auto decoded = jwt::decode(jwt);
    if (decoded.has_payload_claim("user"))
    {
        std::string user = decoded.get_payload_claim("user").as_string();
        return bsoncxx::oid(user);
    }
}

std::string UserModel::toString() const{
    return this->username + "\n" +
            this->password + "\n" +
            this->email + "\n";
}

void UserModel::getFromBsonDocumentLogin(const bsoncxx::document::view &docView)
{
    try
    {
        this->username = extractStringFromBSON(docView, "username");
        this->email = extractStringFromBSON(docView, "email");
        this->password = extractStringFromBSON(docView, "password");
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "BSON Deserialization Error for UserModel in LOGIN: " << e.what() << std::endl;
    }
}


/*
    Trenutno geslo zakodira po algoritmu Argon2id
*/
void UserModel::hashPassword()
{
    char hashed_password_cstr[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str_alg(
        hashed_password_cstr,
        this->password.c_str(),
        this->password.length(),
        crypto_pwhash_OPSLIMIT_INTERACTIVE, 
        crypto_pwhash_MEMLIMIT_INTERACTIVE, 
        crypto_pwhash_ALG_DEFAULT           
    ) != 0) {
        throw std::runtime_error("Failed to hash password.");
    }

    this->password = std::string(hashed_password_cstr);
}

/*
    Vzame plain password kot input in ga primerja z zakodiranim
*/
bool UserModel::verifyPassword(const std::string& plainPassword, const std::string& hashedPassword)
{
    if (plainPassword.empty() || hashedPassword.empty()) {
        return false; 
    }

    if (crypto_pwhash_str_verify(
        hashedPassword.c_str(),
        plainPassword.c_str(),
        plainPassword.length()
    ) == 0) {
        return true; 
    }

    return false; 
}