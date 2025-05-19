#include "RouterUtil/Router.hpp"

#include <vector>
#include <utility>
#include <regex>

std::map<std::string, routeFunction> Router::routesPost{};
std::map<std::string, routeFunction> Router::routesGet{};
std::map<std::string, routeFunction> Router::routesPut{};
std::map<std::string, routeFunction> Router::routesDelete{};

/*
	Funkcija vzame nek [originalUrl] in ga pretvori v regex pri čemer zamenja parametre (:id) z ([^/]+)
*/
std::string Router::convertUrlToRegexForm(std::string& originalUrl)
{
	// Pridobi pozicije vseh parametrov
	std::vector<int> parameterPositions;

	for (int i = 0; i < originalUrl.size(); i++)
	{
		if (originalUrl[i] == ':')
		{
			parameterPositions.push_back(i);
		}
	}

	if (parameterPositions.empty())
	{
		return originalUrl; // Ni parametrov v URL vrni original
	}

	// Zamenja parametre v url z ([^/]+)
	
	std::string newUrl = originalUrl;
	for (int position : parameterPositions)
	{
		int nextSlashPosition = originalUrl.find('/', position);
		int distanceToNextSlash = (nextSlashPosition == std::string::npos)
			? originalUrl.size() - position : nextSlashPosition - position;

		newUrl.erase(position, distanceToNextSlash);
		newUrl.insert(position, "([^/]+)");
	}

	return newUrl;
}

/*
	Funkcija vzame kot parameter [URL] in shrani njegove parametre. Primer :id shrani kot ["id", "VREDNOST ID"]
*/
std::map<std::string, std::string> Router::getParametersFromUrl(std::string& URL, std::string& urlRegex)
{
	return {};
}

/*
	Vzame tip requesta, URL pretvori s pomočjo [getParametersFromUrl] in nato shrani
*/
void Router::createRoute(requestType type, std::string& URL, routeFunction function)
{
	std::string formatedUrl = Router::convertUrlToRegexForm(URL);

	switch (type)
	{
	case Router::GET:
		Router::routesGet.insert({ formatedUrl, function });
		break;
	case Router::POST:
		Router::routesPost.insert({ formatedUrl, function });
		break;
	case Router::PUT:
		Router::routesPut.insert({ formatedUrl, function });
		break;
	case Router::DEL:
		Router::routesDelete.insert({ formatedUrl, function });
		break;
	default:
		break;
	}
}

/*
	Naredi route preko [createRoute] za get
*/
void Router::createGetRoute(std::string& URL, routeFunction function)
{
	createRoute(GET, URL, function);
}

/*
	Naredi route preko [createRoute] za post
*/
void Router::createPostRoute(std::string& URL, routeFunction function)
{
	createRoute(POST, URL, function);
}

/*
	Naredi route preko [createRoute] za put
*/
void Router::createPutRoute(std::string& URL, routeFunction function)
{
	createRoute(PUT, URL, function);
}

/*
	Naredi route preko [createRoute] za delete
*/
void Router::createDeleteRoute(std::string& URL, routeFunction function)
{
	createRoute(DEL, URL, function);
}


/*
	Funkcija glede na podan URL zažene ustrezno funkcijo
*/
static void routeSelector(http::verb method, std::string& URL, const request& req, response& res)
{
	std::map<std::string, routeFunction>* currentRoutingTable;

	switch (method)
	{
	case boost::beast::http::verb::delete_:
		currentRoutingTable = &Router::routesDelete;
		break;
	case boost::beast::http::verb::get:
		currentRoutingTable = &Router::routesGet;
		break;
	case boost::beast::http::verb::post:
		currentRoutingTable = &Router::routesPost;
		break;
	case boost::beast::http::verb::put:
		currentRoutingTable = &Router::routesPut;
		break;
	default:
		return;
	}

	for (const auto& pair : *currentRoutingTable) 
	{
		const std::string& urlPattern = pair.first;
		const std::regex urlRegex(urlPattern);
		const routeFunction& function = pair.second; 

		if (regex_match(URL, urlRegex))
		{
			function(req, res);
			return;
		}
	}

	// TODO Naredi tu da gre na 404
}