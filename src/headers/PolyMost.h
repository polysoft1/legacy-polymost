#ifndef POLYMOST_H
#define POLYMOST_H

#include "include/IProtocolPlugin.h"
#include "include/IPlugin.h"
#include <string>

class PolyMost : public IProtocolPlugin {
public:
	std::string getName();

	bool initialize(Core* core);

	std::string getDatabaseName();

	bool sendMessage(Message msg);
};
#endif
