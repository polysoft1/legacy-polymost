#ifndef MATTERMOST_ACCOUNT
#define MATTERMOST_ACCOUNT

#include <string>
#include "include/IAccount.h"

/**
 * Represents a user on Mattermost. Can be the logged in user
 * or just another team member.
 */
class MattermostAccount : public IAccount {
private:
	std::string uid, email, username, first_name, last_name, nickname, locale;

public:
	MattermostAccount(std::string uid, std::string email, std::string username, std::string first_name,
		std::string last_name, std::string nickname, std::string locale) :
		uid(uid), email(email), username(username), first_name(first_name),
		nickname(nickname), locale(locale)
	{

	}

	std::string getUID() {
		return username;
	}
	std::string getEmail() {
		return username;
	}
	std::string getUsername() {
		return username;
	}
	std::string getFirstName() {
		return username;
	}
	std::string getNickName() {
		return username;
	}
	std::string getLocale() {
		return username;
	}
};

#endif // !MATTERMOST_ACCOUNT
