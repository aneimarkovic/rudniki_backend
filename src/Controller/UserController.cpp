// Created by Anei Markovič 10.5.2025
#include "Controller/UserController.hpp"
#include "DatabaseHandler.hpp"

/*
	Funckija pokliče metode DatabaseHandler in shrani uporabnika
*/
bool UserController::saveUser(UserModel user)
{
	DatabaseHandler handler(dbUri, dbName); // TODO ZAMENJAJ S PRAVO IMPLEMENTACIJO

	return handler.insertDocument("users", user.convertToBsonDocument())
}
