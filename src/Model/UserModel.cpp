// Created by Žan Misja 09/05/2025
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
void UserModel::getFromBsonDocument(const bsoncxx::document::view& docView)
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
    }
    catch (const bsoncxx::exception& e)
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
    builder.append(bsoncxx::builder::basic::kvp("created", bsoncxx::types::b_date{ created }));
    builder.append(bsoncxx::builder::basic::kvp("modified", bsoncxx::types::b_date{ modified }));
    builder.append(bsoncxx::builder::basic::kvp("birthDate", bsoncxx::types::b_date{ birthDate }));

    return builder.extract();
}