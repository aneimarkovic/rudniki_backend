/*
 Created by Anei Markovič 22.5.2025

Razred skrbi za delo z Minerali in njihovimi atributi

*/
#ifndef MINERALMODEL_H
#define MINERALMODEL_H

#include <iostream>
#include <string>

#include <bsoncxx/types.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/value.hpp>

enum MineralGrade
{
    LOW,
    MEDIUM,
    HIGH,
    UNDEFINED
};

class MineralModel
{
private:
    std::string name;
    float min;
    float max;
    MineralGrade grade;
public:
    MineralModel(std::string name, float min, float max, MineralGrade grade);
    MineralModel();
    
    void getFromBsonDocument(const bsoncxx::document::view& docView);
    std::string extractStringFromBSON(const bsoncxx::document::view& docView, const char* key);
    float extractFloatFromBSON(const bsoncxx::document::view &docView, const char *key);
    int extractIntFromBSON(const bsoncxx::document::view &docView, const char *key);
    bsoncxx::document::value convertToBsonDocument();

    bool validateMinerals() const;
};
#endif