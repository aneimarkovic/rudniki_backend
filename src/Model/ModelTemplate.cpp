// Created by Žan Misja 09/05/2025
#include "Model/ModelTemplate.hpp"

#include <iostream>
#include <chrono>

#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/document/view.hpp>

/*
    Način, da naredimo posodobitev private spremenljivke modified
*/
void ModelTemplate::touch()
{
    modified = GET_NOW_IN_MILLISECONDS();
}

/*
    Funkcija vzame ime polja v BSON in ga da v string format za uporabo.
    Če polja ni vrne error
*/
std::string ModelTemplate::extractStringFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_string)
            {
                return std::string(element.get_string().value);
            }
            else
            {
                std::cerr << "Warning: Field '" << key << "' exists but is not a string type (actual type: "
                          << bsoncxx::to_string(element.type()) << ")." << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Field '" << key << "' missing in BSON document." << std::endl;
        }
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }

    return "";
}

/*
    Funkcija vzame ime polja v BSON in ga da v date format za uporabo
    Če polja ni vrne error
*/
timeStamp ModelTemplate::extractDateFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_date)
            {
                return element.get_date().value;
            }
            else if (element.type() == bsoncxx::type::k_int64)
            {
                return std::chrono::milliseconds(element.get_int64().value);
            }
            else
            {
                std::cerr << "Warning: Field '" << key << "' exists but is not a date type (actual type: "
                          << bsoncxx::to_string(element.type()) << ")." << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Field '" << key << "' missing in BSON document." << std::endl;
        }
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }
}
/*
    Funkcija vzame ime polja v BSON in ga da v int format za uporabo
    Če polja ni vrne error
*/
int ModelTemplate::extractIntFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_int32)
            {
                return element.get_int32().value;
            }
            else
            {
                std::cerr << "Warning: Field '" << key << "' exists but is not a integer type (actual type: "
                          << bsoncxx::to_string(element.type()) << ")." << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Field '" << key << "' missing in BSON document." << std::endl;
        }
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }
}

timeStamp ModelTemplate::getCreated() const
{
    return this->created;
}
timeStamp ModelTemplate::getModifiedCreated() const
{
    return this->modified;
}
/*
    Funkcija vzame ime polja v BSON in ga da v double format za uporabo
    Če polja ni vrne error
*/
double ModelTemplate::extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_double)
            {
                return (double)element.get_double().value;
            } else if (element.type() == bsoncxx::type::k_int32){
                return (double)element.get_int32().value;
            }
            else
            {
                std::cerr << "Warning: Field '" << key << "' exists but is not a double type (actual type: "
                          << bsoncxx::to_string(element.type()) << ")." << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Field '" << key << "' missing in BSON document." << std::endl;
        }
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }
}