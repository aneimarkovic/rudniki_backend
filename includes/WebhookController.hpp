#pragma once

#include <map>
#include <unordered_set>
#include <string>
#include <functional>
#include <mutex>          
#include <shared_mutex>

#include <boost/beast/http.hpp>

#include "RouterUtil/Router.hpp"

namespace http = boost::beast::http;
using request = http::request<http::string_body>;
using response = http::response<http::string_body>;
using routeFunction = std::function<void(const request&, response&, Router* r)>;

/*
	Created by Žan Misja 31/05/2025

	Razred WebhookController je namenjen za shranjevanje ter odstranjevanje odjemalcev prikloplenih na webhook
	Razred shranjuje map vseh odjemalcel glede na ime webhook-a. Primer clientMap["rudniki"] bi dal vse odjemalce, ki so povezani na webhook rudniki
	Upravlje kreiranje novih webhook-ov
*/

class WebhookController
{
private:

	static std::map<std::string, std::unordered_set<response*>> clientMap;
	static std::shared_mutex webhookMutex;

	static routeFunction createInternalWebhookFunction(const std::string& webhookURL);

public:

	void createWebhook(const std::string webhookURL, const std::string webhookName);
	void sendBroadcast();

	// Tukaj se dodajo nove webhook implementacije
};