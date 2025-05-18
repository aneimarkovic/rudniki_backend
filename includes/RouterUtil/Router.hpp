#pragma once
#include <map>
#include <string>
#include <functional>
#include <boost/beast/http.hpp>

// Pripravi vse nove tipe, ki jih rabimo
namespace http = boost::beast::http;
using request = http::request<http::string_body>;
using response = http::response<http::string_body>;
using routeFunction = std::function<void(const request&, response&)>;

/*
	Created by Žan Misja 18/05/2025

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

	// map za shranjevanje URL argumentov. Primer /:id shrani id 
	std::map<std::string, std::string> UrlArguments;

	static std::map<std::string, std::string> getParametersFromUrl(std::string& URL, std::string& urlRegex);
	static std::string convertUrlToRegexForm(std::string& originalUrl);
	static void routeSelector(http::verb method, std::string& URL, const request& req, response& res);

	// Univerzalna funkcija, da se izogibamo ponavljanju znotraj create funkcij
	static void createRoute(requestType type, std::string& URL, routeFunction );

public:

	// Funkcije za dodajanje novih poti
	static void createGetRoute(std::string& URL, routeFunction function) ;
	static void createPostRoute(std::string& URL, routeFunction function);
	static void createPutRoute(std::string& URL, routeFunction function);
	static void createDeleteRoute(std::string& URL, routeFunction function);

	void handleRequest(const request& req, response& res);
};
