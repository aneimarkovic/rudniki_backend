// Created by �an Misja 09/05/2025
#include "Model/UserModel.hpp"

#include <iostream>

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

    //USERNAME
    std::regex usernameRegex("^(?:[a-zA-Z0-9_-]){3,}$");
    if (!std::regex_match(this->username, usernameRegex))
    {
        // std::cout << "Username ni ok!\n";
        return false;
    }

    //PASSWORD
    std::regex passwordRegex("^(?=.*[A-Z])(?=.*\\d)(?=.*[@#$%^&*!]).{8,}$");
    if (!std::regex_match(this->password, passwordRegex))
    {
        // std::cout << "Geslo ni ok!\n";
        return false;
    }
    
    //MAIL
    std::regex emailRegex(R"(^([a-zA-Z0-9_\-\.\+]+)@([a-zA-Z0-9_\-\.\+]+\.[a-zA-Z]{2,})$)");
    if ((type == REGISTRATION || type == UPDATE_EMAIL)) {
        if (this->email == "" || !std::regex_match(this->email, emailRegex)) {
            // std::cout << "Mail ni ok!\n";
            return false;
        }
    }

    //BIRTH DATE
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