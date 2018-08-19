#ifndef POLYMOST
#define POLYMOST

#include "headers/PolyMost.h"
#include "headers/MattermostAccount.h"

#include <json.hpp>

#include <memory>
#include <string>
#include <iostream>

#ifndef POLYMOST_MANIFEST
#define POLYMOST_MANIFEST
POCO_BEGIN_MANIFEST(IPlugin)
POCO_EXPORT_CLASS(PolyMost)
POCO_END_MANIFEST
#endif

using namespace Polychat;

std::string PolyMost::getPluginName() const {
	return "PolyMost";
}
std::string PolyMost::getProtocolName() const {
	return "mattermost";
}

PolyMost::PolyMost() {
	loginFieldsList.push_back(LoginField("host", true, true, false));
	loginFieldsList.push_back(LoginField("port", true, true, false));
	loginFieldsList.push_back(LoginField("email", true, true, false));
	loginFieldsList.push_back(LoginField("password", true, false, true));
}

PolyMost::~PolyMost() {

}

bool PolyMost::initialize(ICore* core) {
	this->core = core;

	return true;
}

std::string PolyMost::getDatabaseName() const {
	return "polymost";
}

std::shared_ptr<IAccount> PolyMost::login(std::map<std::string, std::string> fields) {
	// TODO: Validate args.

	nlohmann::json json;
	json["login_id"] = fields["email"];
	json["password"] = fields["password"];

	std::string contentString = json.dump();

	std::shared_ptr<HTTPStringContent> content = std::make_shared<HTTPStringContent>(contentString);
	content->setContentType("application/json");

	HTTPMessage message(HTTPMethod::POST, "/api/v4/users/login");
	message.setContent(content);

	ICommunicator& comm = core->getCommunicator();

	HTTPMessage response = comm.sendRequestSync(fields["host"], std::stoi(fields["port"]), message);
	if (response.getStatus() == HTTPStatus::HTTP_OK) {
		std::string token = response["token"];
		auto userObj = nlohmann::json::parse(response.getContent()->getAsString());

		std::string id = userObj["id"];
		std::string username = userObj["username"];
		std::string first_name = userObj["first_name"];
		std::string last_name = userObj["last_name"];
		std::string nickname = userObj["nickname"];
		std::string email = userObj["email"];
		std::string locale = userObj["locale"];
		std::shared_ptr<MattermostAccount> account =
			std::make_shared<MattermostAccount>(id, email, username, first_name, last_name, nickname, locale);
		return account;
	} else {
		return nullptr;
	}
}

#endif
