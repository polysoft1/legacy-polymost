#ifndef MATTERMOST_USER_H
#define MATTERMOST_USER_H

#include <string>

class MattermostUser {
public:
	MattermostUser(std::string email,
		std::string username,
		std::string first_name,
		std::string last_name,
		std::string nickname,
		std::string locale
	);
};
#endif
