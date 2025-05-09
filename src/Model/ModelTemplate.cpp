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
std::string ModelTemplate::extractStringFromBSON(const bsoncxx::document::view& docView, const char* key) 
{
    try {
        bsoncxx::document::element element = docView[key];

        if (element) { 
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
    catch (const bsoncxx::exception& e) 
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }

    return ""; 
}


/*
    Funkcija vzame ime polja v BSON in ga da v date format za uporabo
    Če polja ni vrne error
*/
timeStamp ModelTemplate::extractDateFromBSON(const bsoncxx::document::view& docView, const char* key)
{
    try {
        bsoncxx::document::element element = docView[key];

        if (element) {
            if (element.type() == bsoncxx::type::k_date)
            {
                return element.get_date().value;
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
    catch (const bsoncxx::exception& e)
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }
}