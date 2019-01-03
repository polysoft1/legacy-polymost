#ifndef MATTERMOST_ACCOUNT
#define MATTERMOST_ACCOUNT

#include <string>
#include "include/IAccount.h"
#include "include/IProtocolPlugin.h"
#include "include/ICore.h"
#include <json.hpp>

class PolyMost;
using namespace Polychat;

/**
 * Represents a user on Mattermost. Can be the logged in user
 * or just another team member.
 */
class MattermostAccount : public Polychat::IAccount {
private:
	// Required by all
	std::string uid, email, username, first_name, last_name, nickname, locale;
	// For logged in accounts
	std::string token = "";
	// The server this account is on.
	std::string host;
	bool ssl;
	unsigned int port;

	Polychat::ICore& core;
	Polychat::IProtocolPlugin& protocol;

	// The websocket that is used for realtime updates.
	// NOTE: This is per account because the servers
	// only support 1 account per connection.
	std::shared_ptr<Polychat::IWebSocket> webSocketConnection;

public:
	MattermostAccount(std::string uid, std::string email, std::string username, std::string first_name,
		std::string last_name, std::string nickname, std::string locale, std::string host,
		unsigned int port, bool ssl, Polychat::ICore& core, Polychat::IProtocolPlugin& protocol) :
		uid(uid), email(email), username(username), first_name(first_name),
		nickname(nickname), locale(locale), core(core), protocol(protocol)
	{

	}

	void setToken(std::string token) {
		this->token = token;
	}

	std::string getToken() {
		return token;
	}

	virtual std::string getUID() {
		return uid;
	}
	virtual std::string getEmail() {
		return email;
	}
	virtual std::string getUsername() {
		return username;
	}
	virtual std::string getFirstName() {
		return first_name;
	}
	virtual std::string getNickName() {
		return nickname;
	}
	virtual std::string getLocale() {
		return locale;
	}

	virtual Polychat::IProtocolPlugin& getProtocol() {
		return protocol;
	}

	virtual void requestUpdates();
};

#endif // !MATTERMOST_ACCOUNT
