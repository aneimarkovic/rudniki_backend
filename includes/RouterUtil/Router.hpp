#pragma once
#include <map>
#include <string>
#include <functional>
#include <regex>

#include <boost/beast/http.hpp>
class Router;
// Pripravi vse nove tipe, ki jih rabimo
namespace http = boost::beast::http;
using request = http::request<http::string_body>;
using response = http::response<http::string_body>;
using routeFunction = std::function<void(const request&, response&, Router* r)>;

/*
	Created by �an Misja 18/05/2025

	Razred Router je odgovoren za preslikavo dohodnih HTTP zahtevkov
	(na podlagi URL poti in HTTP metode) na ustrezne obdelovalne funkcije.
	Podpira ekstrakcijo parametrov iz poti (npr. /uporabniki/:id).
*/

class Router
{
	enum requestType { GET, POST, PUT, DEL };

	friend class Router_Test_Class;

public:
	// Router spomin za narejene route
	static std::map<std::string, routeFunction> routesPost;
	static std::map<std::string, routeFunction> routesGet;
	static std::map<std::string, routeFunction> routesPut;
	static std::map<std::string, routeFunction> routesDelete;

	// Vector za shranjevanje URL argumentov. Primer /:id shrani id 
	std::vector<std::string> UrlArguments;

	static std::vector<std::string> getParametersFromUrl(const std::string& URL, const std::regex& urlRegex);
	static std::string convertUrlToRegexForm(const std::string& originalUrl);
	void routeSelector(http::verb method, std::string& URL, const request& request, response& response);

	// Univerzalna funkcija, da se izogibamo ponavljanju znotraj create funkcij
	static void createRoute(requestType type, std::string& URL, routeFunction function);

public:

	// Funkcije za dodajanje novih poti
	static void createGetRoute(std::string URL, routeFunction function) ;
	static void createPostRoute(std::string URL, routeFunction function);
	static void createPutRoute(std::string URL, routeFunction function);
	static void createDeleteRoute(std::string URL, routeFunction function);

	void handleRequest(const request& request, response& response);
};
