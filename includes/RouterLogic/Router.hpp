#pragma once
#include <map>
#include <string>

// Pripravi vse nove tipe, ki jih rabimo
namespace http = beast::http;
using request = http::request<http::string_body>;
using response = http::response<http::string_body>;
using routeFunction = std::function<void(const StoredRequest&, StoredResponse&)>;

/*
	Created by Žan Misja 18/05/2025

	Router preusmeri URL na klic pravilne funkcije glede na naprej nastavljeno logiko
	Shranjuje narejene poti in jih uporabi, ko je to potrebno
*/

class Router
{
private:
	// Router spomin za narejene route
	static std::map<std::string, routeFunction> routesPost = NULL;
	static std::map<std::string, routeFunction> routesGet = NULL;
	static std::map<std::string, routeFunction> routesPut = NULL;
	static std::map<std::string, routeFunction> routesDelete = NULL;

	// map za shranjevanje URL argumentov. Primer /:id shrani id 
	std::map<std::string, std::string> UrlArguments;

	static std::map<std::string, std::string> urlParser(std::string& URL);
	static void routeSelector(http::verb method, std::string& URL, const request& req, response& res);

	// Univerzalna funkcija, da se izogibamo ponavljanju znotraj create funkcij
	static void createRoute(std::string& URL, routeFunction);

public:

	// Funkcije za dodajanje novih poti
	static void createGetRoute(std::string& URL, routeFunction);
	static void createPostRoute(std::string& URL, routeFunction);
	static void createPutRoute(std::string& URL, routeFunction);
	static void createDeleteRoute(std::string& URL, routeFunction);

	void handleRequest(const request& req, cresponse& res);
};
