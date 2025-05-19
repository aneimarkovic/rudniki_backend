// Created by Anei Markovič 10.5.2025
#include "Controller/UserController.hpp"
#include "DatabaseHandler.hpp"

/*
	Funckija pokliče metode DatabaseHandler in shrani uporabnika
*/
bool UserController::saveUser(UserModel user)
{

	return DatabaseHandler::insertDocument("users", user.convertToBsonDocument());
}
