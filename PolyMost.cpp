#ifndef POLYMOST
#define POLYMOST

#include "PolyMost.h"
#include "Core.h"
#include "ConnectCommand.h"
#include <memory>

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
