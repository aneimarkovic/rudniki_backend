#include "RouterUtil/Router.hpp"

#include <vector>
#include <utility>
#include <regex>

std::map<std::string, routeFunction> Router::routesPost{};
std::map<std::string, routeFunction> Router::routesGet{};
std::map<std::string, routeFunction> Router::routesPut{};
std::map<std::string, routeFunction> Router::routesDelete{};
std::shared_mutex Router::routesMutex{};

/*
	Funkcija vzame nek [originalUrl] in ga pretvori v regex pri čemer zamenja parametre (:id) z ([^/]+)
*/
std::string Router::convertUrlToRegexForm(const std::string& originalUrl)
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
	Funkcija vzame kot parameter [URL] in shrani njegove parametre.
*/
std::vector<std::string> Router::getParametersFromUrl(const std::string& URL, const std::regex& urlRegex)
{
	std::vector<std::string> capturedValues;
	std::smatch matchResults;

	if (std::regex_match(URL, matchResults, urlRegex)) {
		for (size_t i = 1; i < matchResults.size(); ++i) {
			capturedValues.push_back(matchResults[i].str());
		}
	}

	return capturedValues;
}

/*
	Vzame tip requesta, URL pretvori s pomočjo [getParametersFromUrl] in nato shrani
*/
void Router::createRoute(requestType type, std::string& URL, routeFunction function)
{
	std::string formatedUrl = Router::convertUrlToRegexForm(URL);

	std::unique_lock<std::shared_mutex> lock(routesMutex);
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
void Router::createGetRoute(std::string URL, routeFunction function)
{
	createRoute(GET, URL, function);
}

/*
	Naredi route preko [createRoute] za post
*/
void Router::createPostRoute(std::string URL, routeFunction function)
{
	createRoute(POST, URL, function);
}

/*
	Naredi route preko [createRoute] za put
*/
void Router::createPutRoute(std::string URL, routeFunction function)
{
	createRoute(PUT, URL, function);
}

/*
	Naredi route preko [createRoute] za delete
*/
void Router::createDeleteRoute(std::string URL, routeFunction function)
{
	createRoute(DEL, URL, function);
}


/*
	Funkcija glede na podan URL zažene ustrezno funkcijo
*/
void Router::routeSelector(http::verb method, std::string& URL, const request& request, response& response)
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

	std::shared_lock<std::shared_mutex> routes_lock(routesMutex);
	for (const auto& pair : *currentRoutingTable) 
	{
		const std::string& urlPattern = pair.first;
		const std::regex urlRegex(urlPattern);
		const routeFunction& function = pair.second; 

		if (regex_match(URL, urlRegex))
		{
			if (urlPattern.find("([^/]+)") != urlPattern.npos)
			{
				UrlArguments = getParametersFromUrl(URL, urlRegex);
			}

			routes_lock.unlock();

			function(request, response, this);
			return;
		}
	}

	// TODO Naredi tu da gre na 404
}

void Router::handleRequest(const request& request, response& response)
{
	http::verb requestMethod = request.method();

	boost::string_view target_view = request.target();
	std::string URL(target_view.data(), target_view.length());

    try{
        routeSelector(requestMethod, URL, request, response);
    } catch (const std::exception &e){
        bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << "Prišlo je do napake!" << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view viewTemp = documentTemp.view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);

        response.body() = jsonStr;
    }
}