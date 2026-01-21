

#include "model/HelmetDataModel.hpp"

HelmetDataModel::HelmetDataModel(int totalPersons, int helmetsOn, bool missingHelmet, long long timestamp)
    : totalPersons(totalPersons), helmetsOn(helmetsOn), missingHelmet(missingHelmet), timestamp(timestamp) {
}

HelmetDataModel::HelmetDataModel()
    : totalPersons(0), helmetsOn(0), missingHelmet(false), timestamp(0) {
}

int HelmetDataModel::getTotalPersons() const { return this->totalPersons; }
int HelmetDataModel::getHelmetsOn() const { return this->helmetsOn; }
bool HelmetDataModel::getMissingHelmet() const { return this->missingHelmet; }
long long HelmetDataModel::getTimestamp() const { return this->timestamp; }

std::string HelmetDataModel::toString() {
    return "{\"totalPersons\":" + std::to_string(totalPersons) +
        ", \"helmetsOn\":" + std::to_string(helmetsOn) +
        ", \"missingHelmet\":" + (missingHelmet ? "true" : "false") +
        ", \"timestamp\":" + std::to_string(timestamp) + "}";
}

void HelmetDataModel::getFromBsonDocument(const bsoncxx::document::view& docView) {
    try
    {
        this->totalPersons = extractIntFromBSON(docView, "totalPersons");
        this->helmetsOn = extractIntFromBSON(docView, "helmetsOn");
        this->missingHelmet = extractBoolFromBSON(docView, "missingHelmet");
        this->timestamp = extractDateFromBSON(docView, "timestamp");
    }
    catch (const bsoncxx::exception& exception)
    {
        std::cerr << "BSON Deserialization Error for HelmetDataModel: " << exception.what() << std::endl;
    }
}

bsoncxx::document::value HelmetDataModel::convertToBsonDocument() {
    bsoncxx::builder::basic::document builder{};

    using bsoncxx::builder::basic::kvp;

    builder.append(kvp("totalPersons", this->totalPersons));
    builder.append(kvp("helmetsOn", this->helmetsOn));
    builder.append(kvp("missingHelmet", this->missingHelmet));

    std::chrono::milliseconds ms(this->timestamp);
    std::chrono::system_clock::time_point tp(ms);
    builder.append(kvp("timestamp", bsoncxx::types::b_date{ tp }));

    return builder.extract();
}

int HelmetDataModel::extractIntFromBSON(const bsoncxx::document::view& docView, const char* key) {
    try {
        bsoncxx::document::element element = docView[key];
        if (element) {
            if (element.type() == bsoncxx::type::k_int32) {
                return element.get_int32().value;
            }
            else if (element.type() == bsoncxx::type::k_int64) {
 
                return static_cast<int>(element.get_int64().value);
            }
        }
    }
    catch (...) {}
    return 0;
}

bool HelmetDataModel::extractBoolFromBSON(const bsoncxx::document::view& docView, const char* key) {
    try {
        bsoncxx::document::element element = docView[key];
        if (element && element.type() == bsoncxx::type::k_bool) {
            return element.get_bool().value;
        }
    }
    catch (...) {}
    return false; 
}

long long HelmetDataModel::extractDateFromBSON(const bsoncxx::document::view& docView, const char* key) {
    try {
        bsoncxx::document::element element = docView[key];
        if (element && element.type() == bsoncxx::type::k_date) {
            return element.get_date().value.count();
        }
    }
    catch (...) {}
    return 0; 
}