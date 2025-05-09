#pragma once

#include <chrono>
#include <string>

#include "Model/ModelTemplate.hpp"

/*
	Created by Žan Misja 09/05/2025

	Razred UserModel skrbi za shranjevanje uporabniških podatkov
	Imamo en konstruktor za ustvarjanje uporabnika znotraj http serverja. 
	Default konstruktor obstaja.

	Override-a funkcije iz ModelTemplate, ki se uporabljajo za preslikavo objekta
*/
class UserModel : public ModelTemplate
{
public:
	std::string username;
	std::string password;
	std::string email;
	timeStamp birthDate;

	UserModel(std::string username, std::string email, std::string password, timeStamp createdAt, timeStamp modifiedAt )
		: username(std::move(username)),
		email(std::move(email)),
		password(std::move(password)),
		ModelTemplate(createdAt, modifiedAt) {
	} 

	UserModel() : ModelTemplate(GET_NOW_IN_MILLISECONDS(), GET_NOW_IN_MILLISECONDS()) {}

	void getFromBsonDocument(const bsoncxx::document::view& docView) override;
	bsoncxx::document::value convertToBsonDocument() override;
};