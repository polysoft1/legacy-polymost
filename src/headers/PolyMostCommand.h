#ifndef LOGIN_COMMAND_H
#define LOGIN_COMMAND_H

#include "include/ICommand.h"
#include <string>
#include <vector>
#include "PolyMost.h"

class PolyMostCommand : public ICommand {
public:
	PolyMostCommand(PolyMost& main);
	bool onCommand(std::vector<std::string>);
	std::vector<std::string>* onTabCompletion(std::vector<std::string>, int);
private:
	PolyMost& main;
	bool loginCommand(std::vector<std::string>);
	bool listTeamsCommand(std::vector<std::string>);
	bool listChannelsCommand(std::vector<std::string>);
	bool selectTeamCommand(std::vector<std::string>);
	bool selectChannelCommand(std::vector<std::string>);
	bool sendMessageCommand(std::vector<std::string>);
};

#endif