#ifndef MATTERMOST_USER_H
#define MATTERMOST_USER_H

#include "mattermost.pb.h"
#include <string>

class MattermostUser {
public:
	MattermostUser(mattermost::User* user) {
		userData = user;
	};

	std::string token = "", team = "", channel = "";
	mattermost::User* userData;
};
#endif
