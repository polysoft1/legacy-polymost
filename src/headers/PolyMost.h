#ifndef POLYMOST_H
#define POLYMOST_H

#include "include/IProtocolPlugin.h"
#include "include/IPlugin.h"
#include "include/Core.h"
#include <string>

class PolyMost : public IProtocolPlugin {
public:
	~PolyMost();

	std::string getName();

	bool initialize(Core* core);

	std::string getDatabaseName();

	bool sendMessage(Message msg);
	std::string token;// TODO: this is temporary! Do not leave it like this!
	std::string team;// TODO: this is temporary! Do not leave it like this!
	std::string user;// TODO: this is temporary! Do not leave it like this!
	std::string channel;// TODO: this is temporary! Do not leave it like this!

private:
	Core* core = nullptr;
};
#endif
