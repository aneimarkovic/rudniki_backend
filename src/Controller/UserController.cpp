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

void UserController::loginUser(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    UserModel temp;
    temp.getFromBsonDocumentLogin(view);

    std::cout << temp.toString() << std::endl;


    std::optional<bsoncxx::oid> id = temp.authUserData();
    std::string status = "Napaka ob prijavi!";
    if(id){
        status = id->to_string();
    }
    bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << status << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view viewTemp = documentTemp.view();
    std::string jsonStr = bsoncxx::to_json(viewTemp);

    response.body() = jsonStr;
}