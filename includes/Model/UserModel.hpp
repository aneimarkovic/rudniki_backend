#pragma once

#include <chrono>
#include <string>
#include <regex>
#include <string>

#include <jwt-cpp/jwt.h>
#include "Model/ModelTemplate.hpp"
#include "Model/MineralModel.hpp"
#include "HttpServer.hpp"

/*
	Created by �an Misja 09/05/2025

	Razred UserModel skrbi za shranjevanje uporabni�kih podatkov
	Imamo en konstruktor za ustvarjanje uporabnika znotraj http serverja. 
	Default konstruktor obstaja.

	Override-a funkcije iz ModelTemplate, ki se uporabljajo za preslikavo objekta
*/

enum validationType{
	LOGIN,
	REGISTRATION,
	UPDATE,
	UPDATE_EMAIL,
};

class UserModel : public ModelTemplate
{
public:
	std::string username;
	std::string password;
	std::string email;
	timeStamp birthDate;

	UserModel(std::string username, std::string email, std::string password, timeStamp birthDate, timeStamp createdAt, timeStamp modifiedAt)
		: username(std::move(username)),
		email(std::move(email)),
		password(std::move(password)),
		birthDate(birthDate),
		ModelTemplate(createdAt, modifiedAt) {
	}

	UserModel() : ModelTemplate(GET_NOW_IN_MILLISECONDS(), GET_NOW_IN_MILLISECONDS()) {}

	void getFromBsonDocument(const bsoncxx::document::view& docView) override;
	bsoncxx::document::value convertToBsonDocument() override;

	bool validateUserData(validationType type) const;
	static std::string getDateFromMS(timeStamp time);
	std::optional<bsoncxx::oid> authUserData() const;
	static bool checkIfUserIsLoggedIn(std::string& jwt);
	static bsoncxx::oid getUserIdFromJWT(std::string& jwt);

	std::string toString() const;
	void getFromBsonDocumentLogin(const bsoncxx::document::view& docView);

	void hashPassword();
	static bool verifyPassword(const std::string& plainPassword, const std::string& hashedPassword);
};