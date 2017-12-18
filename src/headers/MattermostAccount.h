#ifndef MATTERMOST_ACCOUNT
#define MATTERMOST_ACCOUNT

#include <string>
#include "include/IAccount.h"

class MattermostAccount : public IAccount {
public:
	MattermostAccount() {

	}

	std::string getUsername() {
		return "NOT IMPLEMENTED";
	}
};

#endif // !MATTERMOST_ACCOUNT
