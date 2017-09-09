#ifndef POLYMOST_H
#define POLYMOST_H

#include "IProtocolPlugin.h"

class PolyMost : public IProtocolPlugin {
public:
	string getName();

	bool initialize();

	string getDatabaseName();

	bool sendMessage(Message msg);
};

POCO_BEGIN_MANIFEST(IPlugin)
	POCO_EXPORT_CLASS(PolyMost)
POCO_END_MANIFEST

#endif
