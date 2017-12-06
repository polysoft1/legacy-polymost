#ifndef POLYMOST
#define POLYMOST

#include "headers/PolyMost.h"
#include "headers/ConnectCommand.h"
#include "headers/PolyMostCommand.h"
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

PolyMost::~PolyMost() {
	if (core != nullptr) {
		ICommand* connectCommand = core->unregisterCommand("connect");
		ICommand* mattermostCommand = core->unregisterCommand("mattermost");
		core->unregisterCommand("polymost"); // Don't store to variable due to alias = same thing
		if (connectCommand != nullptr) {
			delete connectCommand;
		}
		if (mattermostCommand != nullptr) {
			delete mattermostCommand;
		}
	}

}

bool PolyMost::initialize(Core* core) {
	core->registerCommand(new ConnectCommand(), "connect");
	core->registerCommand(new PolyMostCommand(*this), "mattermost");
	core->registerCommand(new PolyMostCommand(*this), "polymost"); // alias
	this->core = core;

	return false;
}

std::string PolyMost::getDatabaseName() {
	return "polymost";
}

bool PolyMost::sendMessage(Message msg) {
	return false;
}

#endif
