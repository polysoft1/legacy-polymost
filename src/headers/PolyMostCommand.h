#ifndef LOGIN_COMMAND_H
#define LOGIN_COMMAND_H

#include "include/ICommand.h"
#include <string>
#include <vector>
#include "PolyMost.h"
#include "polymost.pb.h"

class PolyMostCommand : public ICommand {
public:
	PolyMostCommand(PolyMost& main);
	~PolyMostCommand();
	bool onCommand(std::vector<std::string>);
	std::vector<std::string>* onTabCompletion(std::vector<std::string>, int);
private:
	PolyMost& main;
	
	polymost::user user;
	polymost::server server;

	bool loginCommand(std::vector<std::string>);
	bool listTeamsCommand(std::vector<std::string>);
	bool listChannelsCommand(std::vector<std::string>);
	bool selectTeamCommand(std::vector<std::string>);
	bool selectChannelCommand(std::vector<std::string>);
	bool sendMessageCommand(std::vector<std::string>);
};

#endif
