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

}

bool PolyMost::initialize(Core* core) {
	core->getCommandHandler().registerCommand(new ConnectCommand(), "connect", this);
	core->getCommandHandler().registerCommand(new PolyMostCommand(*this), "mattermost", this);
	core->getCommandHandler().registerCommand(new PolyMostCommand(*this), "polymost", this); // alias
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
