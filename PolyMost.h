#ifndef POLYMOST_H
#define POLYMOST_H

#include "IProtocolPlugin.h"
#include "IPlugin.h"
#include <string>

class PolyMost : public IProtocolPlugin {
public:
	std::string getName();

	bool initialize(PolyChatCore* core);

	std::string getDatabaseName();

	bool sendMessage(Message msg);
};

POCO_BEGIN_MANIFEST(IPlugin)
	POCO_EXPORT_CLASS(PolyMost)
POCO_END_MANIFEST

#endif
