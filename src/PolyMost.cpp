#ifndef POLYMOST
#define POLYMOST

#include "headers/PolyMost.h"
#include "headers/ConnectCommand.h"
#include "include/Core.h"
#include <memory>

#ifndef POLYMOST_MANIFEST
#define POLYMOST_MANIFEST
POCO_BEGIN_MANIFEST(IPlugin)
	POCO_EXPORT_CLASS(PolyMost)
POCO_END_MANIFEST
#endif

std::string PolyMost::getName() {
	return "PolyMost";
}

bool PolyMost::initialize(Core* core) {
	// This is invalid (at least on Windows) because the DLL has its own memory space
	core->registerCommand(new ConnectCommand(), "connect");

	return false;
}

std::string PolyMost::getDatabaseName() {
	return "polymost";
}

bool PolyMost::sendMessage(Message msg) {
	return false;
}

#endif
