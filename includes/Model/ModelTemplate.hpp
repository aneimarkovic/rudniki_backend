#pragma once

#include "chrono"

#include <bsoncxx/types.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/value.hpp>

using timeStamp = std::chrono::milliseconds;

/* Macro da se dobi trenutni čas v milisekundah */
#define GET_NOW_IN_MILLISECONDS() (std::chrono::duration_cast<std::chrono::milliseconds>( \
    std::chrono::system_clock::now().time_since_epoch() \
))


/*
	Created by Žan Misja 09/05/2025

	ModelTemplate je template razred za vse naše Model implementacije.
	Ima atribute, ki jih bo imel vsak model (id, createdAt, modifiedAt).

	Vsebuje dve virtualni funkciji, ki se uporabljata za preslikavo iz objekta v BSON in obratno
	Ima funckijo touch za lahko spreminjanje modifiedAt spremenljivke

	Vsak subClass bo moral implementirati vse prototip funkcije tega razreda

	Ima helper funkcijo za pridobivanje string ter date iz BSON dokumenta
*/
class ModelTemplate
{
protected:
	bsoncxx::oid id;

	timeStamp created;
	timeStamp modified;

public:
	ModelTemplate(timeStamp created, timeStamp modified) : created(created), modified(modified) {}

	virtual void getFromBsonDocument(const bsoncxx::document::view& docView) = 0;
	virtual bsoncxx::document::value convertToBsonDocument() = 0;

	void touch();
	std::string extractStringFromBSON(const bsoncxx::document::view& docView, const char* key);
	timeStamp extractDateFromBSON(const bsoncxx::document::view& docView, const char* key);
	timeStamp getCreated() const;
	timeStamp getModifiedCreated() const;
};