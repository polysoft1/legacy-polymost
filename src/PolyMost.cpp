#include "PolyMost.h"
#include "MattermostAccountSession.h"

#include "include/IWebHelper.h"
#include "include/IAccountManager.h"

#include <nlohmann/json.hpp>

#include <memory>
#include <string>
#include <iostream>

using namespace Polychat;

std::string PolyMost::getPluginName() const {
	return "PolyMost";
}
std::string PolyMost::getProtocolName() const {
	return "mattermost";
}

PolyMost::PolyMost() {
	loginFieldsList.push_back(LoginField("address", true, true, false));
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

AuthStatus PolyMost::login(std::map<std::string, std::string> fields, IAccount& account) {
	nlohmann::json json;
	
	{ // shrink scope to delete password sooner
		auto itr = fields.find("email");
		if (itr == fields.end())
			return AuthStatus::FAIL_MISSING_FIELDS;
		json["login_id"] = itr->second;
		account.setEmail(itr->second);

		itr = fields.find("password");
		if (itr == fields.end())
			return AuthStatus::FAIL_MISSING_FIELDS;
		json["password"] = itr->second;
	}

	std::string contentString = json.dump();

	std::shared_ptr<HTTPStringContent> content = std::make_shared<HTTPStringContent>(contentString);
	content->setContentType("application/json");

	HTTPMessage message(HTTPMethod::POST, "/api/v4/users/login");
	message.setContent(content);

	IWebHelper& web = core->getWebHelper();

	std::string host, uri;
	unsigned int port;
	bool ssl;
	
	if (!IWebHelper::parseAddress(fields["address"], host, port, ssl, uri))
		return AuthStatus::FAIL_INVALID_ADDRESS;
	std::shared_ptr<IHTTPClient> webClient = web.initHTTPClient(host, port, ssl);
	webClient->sendRequest(message, [this, &account, host, port, ssl](std::shared_ptr<HTTPMessage> responsePtr) {
			if (responsePtr->getStatus() == HTTPStatus::HTTP_OK) {
				HTTPMessage& response = *responsePtr.get();
				std::string token = response["Token"];
				auto userObj = nlohmann::json::parse(response.getContent()->getAsString());

				std::string id = userObj["id"];
				std::string username = userObj["username"];
				std::string firstName = userObj["first_name"];
				std::string lastName = userObj["last_name"];
				std::string nickname = userObj["nickname"];
				std::string email = userObj["email"];
				std::string locale = userObj["locale"];

				// TODO: What about the situation where the ID differs from the stored account?
				account.setUID(id);
				account.setUsername(username);
				account.setFirstName(firstName);
				// TODO: Last name
				account.setNickName(nickname);
				account.setEmail(email);
				account.setLocale(locale);

				auto session = sessions.emplace(std::make_shared<MattermostAccountSession>(account, host, port, ssl, token, *core));
				account.setSession(*session.first);

				core->getAccountManager().alertOfSessionChange(account, AuthStatus::AUTHENTICATED);
			} else {
				core->getAccountManager().alertOfSessionChange(account, AuthStatus::FAIL_HTTP_ERROR);
				core->alert(std::to_string((int)responsePtr->getStatus()));
			}
		}
	);
	return AuthStatus::CONNECTING;
}

#ifdef POLYMOST_SHARED
extern "C" {
#ifdef _WIN32
	__declspec(dllexport) PolyMost* create()
	{
		return new PolyMost;
	}

	__declspec(dllexport) void destroy(PolyMost * in)
	{
		delete in;
	}
#else
	PolyMost* create()
	{
		return new PolyMost;
	}

	void destroy(PolyMost* in)
	{
		delete in;
	}
#endif
}
#endif
