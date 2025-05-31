/*
	Created by Žan Misja 31/05/2025
*/

#include "WebhookController.hpp"

#include <bsoncxx/json.hpp>

std::map<std::string, std::unordered_set<response*>> clientMap{};
std::shared_mutex WebhookController::webhookMutex{};

/*
	Funkcija ustvari funkcijo za novi webhook ter jo vrne
*/
routeFunction WebhookController::createInternalWebhookFunction(const std::string& webHookName)
{
	return [webHookName](const request& req, response& res, Router* r) mutable {
		std::unique_lock<std::shared_mutex> lock(WebhookController::webhookMutex);
		WebhookController::clientMap[webHookName].insert(&res);
	};
}

/*
	Funkcija preko Routerja ustvari novi webhook glede na URL.
*/
void WebhookController::createWebhook(const std::string webhookURL, const std::string webhookName)
{
	routeFunction savingFunction = createInternalWebhookFunction(webhookName);
	Router::createPostRoute(webhookURL, savingFunction);
}

/*
	Funkcija, ki pošlje broadcast na vse povezane odjemalce za specifični [webhookName]
*/
void WebhookController::sendBroadcast(const std::string webhookName, const bsoncxx::document::value payload)
{
	std::unique_lock<std::shared_mutex> lock(WebhookController::webhookMutex);

	auto map_iterator = WebhookController::clientMap.find(webhookName);
	if (map_iterator == WebhookController::clientMap.end()) {
		return; 
	}

	std::unordered_set<response*>& clients_set = map_iterator->second; 

	for (auto it_client = clients_set.begin(); it_client != clients_set.end();) {
		response* res_ptr = *it_client;

		if (res_ptr) { 
			bool sent_successfully_or_still_seems_valid = true; 
			try {
				res_ptr->set(http::field::server, "Rudnik http server");
				res_ptr->set(http::field::content_type, "application/json");
				res_ptr->set(http::field::access_control_allow_credentials, "true");
				res_ptr->set(http::field::access_control_allow_headers, "Content-Type, Authorization, X-Requested-With, Accept");
				res_ptr->set(http::field::access_control_allow_origin, "http://localhost:3000");
				res_ptr->body() = bsoncxx::to_json(payload); 
				res_ptr->prepare_payload();
			}
			catch (const std::exception& e) {
				sent_successfully_or_still_seems_valid = false;
			}

			if (sent_successfully_or_still_seems_valid) {
				++it_client;
			}
			else {
				it_client = clients_set.erase(it_client);
			}
		}
		else {
			it_client = clients_set.erase(it_client);
		}
	}
}