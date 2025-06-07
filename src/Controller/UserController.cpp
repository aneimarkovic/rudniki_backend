// Created by Anei Markovič 10.5.2025
#include "Controller/UserController.hpp"
#include "DatabaseHandler.hpp"

/*
	Funckija pokliče metode DatabaseHandler in shrani uporabnika
*/
void UserController::saveUser(const request& request, response& response, Router* router)
{
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();


    UserModel user;
    user.getFromBsonDocument(view);

    if(!user.validateUserData(validationType::REGISTRATION)){
        bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << "Napaka ob prijavi: Nepravilni podatki" << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view viewTemp = documentTemp.view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);

        response.body() = jsonStr;
        return;
    }

    user.hashPassword();
    bsoncxx::document::value temp = user.convertToBsonDocument();

    std::cout << user.toString() << std::endl;

    std::optional<bsoncxx::oid> id = DatabaseHandler::insertDocumentGetInsertId("users", temp);

    if(id){
        bsoncxx::oid actualId = *id;
        std::string jwt = HttpServer::createJWT(actualId);

        std::string cookie = "jwt=" + jwt + "; Path=/; Max-Age=604800; Secure; SameSite=none";
        response.set(http::field::set_cookie, cookie);

//        std::cout << actualId.to_string() << std::endl;

        bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << actualId.to_string() << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view viewTemp = documentTemp.view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);
        response.body() = jsonStr;
    } else
    {
        response.body() = "Napaka pri dodajanju uporabnika!";
    }
}

//Funkcija, ki prijavi uporabnika in vrne njegov id
void UserController::loginUser(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    std::string token = "";
    auto it = request.find(boost::beast::http::field::cookie);
    if(it != request.end())
    {
        auto cookie_header = std::string(it->value());
        size_t pos = cookie_header.find("jwt=");
        if (pos != std::string::npos) {
            size_t start = pos + 4;
            size_t end = cookie_header.find(";", start);
            token = cookie_header.substr(start, end - start);
        }
    }

    if(token != ""){
        bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << "Napaka ob prijavi!" << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view viewTemp = documentTemp.view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);

        response.body() = jsonStr;
        return;
    }

    UserModel temp;
    temp.getFromBsonDocumentLogin(view);

    if(!temp.validateUserData(validationType::LOGIN)){
        bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << "Napaka ob prijavi: Nepravilni podatki" << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view viewTemp = documentTemp.view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);

        response.body() = jsonStr;
        return;
    }


    mongocxx::options::find opts{};
    opts.projection(bsoncxx::builder::stream::document{} << "password_hash" << 1 << "_id" << 1 << bsoncxx::builder::stream::finalize);

    bsoncxx::builder::stream::document filters;
    if(temp.username != ""){
        filters << "username" << temp.username;
    } else {
        filters << "email" << temp.email;
    }

    bsoncxx::document::value docValue = filters << bsoncxx::builder::stream::finalize;
    std::vector<bsoncxx::document::value> result = DatabaseHandler::getSpecificColumnFromDocument("users", opts, docValue);
    std::string status = "Napaka ob prijavi!";
    if(result.size() > 0){
        bsoncxx::document::element password = result[0].view()["password_hash"];
        if(password && password.type() == bsoncxx::type::k_string){
            std::string passwordHash(password.get_string().value);
            if(UserModel::verifyPassword(temp.password, passwordHash)){
                bsoncxx::oid id = result[0].view()["_id"].get_oid().value;
                std::string jwt = HttpServer::createJWT(id);
                std::string cookie = "jwt=" + jwt + "; Path=/; Max-Age=604800; Secure; SameSite=none";
                response.set(http::field::set_cookie, cookie);
                status = id.to_string();
            }
        }
    }

    bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << status << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view viewTemp = documentTemp.view();
    std::string jsonStr = bsoncxx::to_json(viewTemp);

    response.body() = jsonStr;
}
//Funkcija, ki pridobi user-ja
void UserController::getUser(const request &request, response &response, Router* r){
    auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{r->UrlArguments[0]}));

    std::optional<bsoncxx::document::value> userDoc = DatabaseHandler::fetchSingleDocument("users", filters);
    if (userDoc)
    {
        bsoncxx::document::view viewTemp = userDoc->view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);
        response.body() = jsonStr;
    }
    else
    {
        std::cout << "Neobstaja\n";
    }
}
//Funkcija ki posodobi userja
void UserController::updateUser(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    bsoncxx::builder::stream::document query;
    bsoncxx::document::element id = view["id"];
    auto stringView = id.get_string().value;
    std::string str_val(stringView.data(), stringView.size());
    bsoncxx::oid userID = bsoncxx::oid(str_val);
    query << "_id" << userID;

    bsoncxx::builder::stream::document updateQuery;
    bsoncxx::document::element element = view["username"];
    if(element && element.type() == bsoncxx::type::k_string){
        updateQuery << "username" << element.get_string().value;
    }

    element = view["email"];
    if(element && element.type() == bsoncxx::type::k_string){
        updateQuery << "email" << element.get_string().value;
    }
    bsoncxx::builder::stream::document updateSet;
    updateSet << "$set" << updateQuery.view();

    std::string resString = (DatabaseHandler::updateOneItem("users", query, updateSet) ? ("Uporabnik uspešno posodobljen!") : ("Pri posodabljanju uporabnika je prišlo do napake!"));
    bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
    response.body() = bsoncxx::to_json(documentTemp.view());
}