/*
	Created by Žan Misja 31/05/2025
*/

#include "WebhookController.hpp"

std::map<std::string, std::unordered_set<response*>> clientMap{};
std::shared_mutex WebhookController::webhookMutex{};

/*
	Funkcija ustvari funkcijo za novi webhook ter jo vrne
*/
routeFunction WebhookController::createInternalWebhookFunction(const std::string& webHookName)
{
	return [webHookName](const request& req, response& res, Router* r) mutable {
		WebhookController::clientMap[webHookName].insert(&res);
	};
}

/*
	Funkcija preko Routerja ustvari novi webhook glede na URL.
*/
void WebhookController::createWebhook(const std::string webhookURL, const std::string webhookName)
{
	routeFunction savingFunction = createInternalWebhookFunction(webhookName);
	Router.createPostRoute(webhookURL, savingFunction);
}