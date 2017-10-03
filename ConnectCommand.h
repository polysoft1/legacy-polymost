#ifndef CONNECT_COMMAND_H
#define CONNECT_COMMAND_H

#include "ICommand.h"
#include <string>
#include <vector>

class ConnectCommand : public ICommand {
public:
public:
	bool onCommand(std::vector<std::string>);
	std::vector<std::string>* onTabCompletion(std::vector<std::string>, int);
};

#endif