#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include "Model/UserModel.hpp"
#include "RouterUtil/Router.hpp"
#include <iostream>
#include <string.h>
#include <regex>

#include <bsoncxx/json.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>

class UserController
{
public:

    static void saveUser(const request& request, response& response, Router* router);
    static void loginUser(const request &request, response &response, Router* r);
    static void getUser(const request &request, response &response, Router* r);
};

#endif