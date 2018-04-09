#ifndef POLYMOST
#define POLYMOST

#include "headers/PolyMost.h"
#include "headers/ConnectCommand.h"
#include "headers/PolyMostCommand.h"
#include "headers/MattermostAccount.h"
#include <memory>
#include <string>
#include <iostream>

#ifndef POLYMOST_MANIFEST
#define POLYMOST_MANIFEST
POCO_BEGIN_MANIFEST(IPlugin)
POCO_EXPORT_CLASS(PolyMost)
POCO_END_MANIFEST
#endif

using namespace Polychat;

std::string PolyMost::getName() {
	return "PolyMost";
}

PolyMost::PolyMost() {
	LoginField loginField = LoginField("email", true, true);
	loginFieldsList.push_back(loginField);
}

PolyMost::~PolyMost() {

}

bool PolyMost::initialize(Core* core) {
	core->getCommandHandler().registerCommand(new ConnectCommand(), "connect", this);
	core->getCommandHandler().registerCommand(new PolyMostCommand(*this, *core), "mattermost", this);
	core->getCommandHandler().registerCommand(new PolyMostCommand(*this, *core), "polymost", this); // alias
	this->core = core;
	std::map<std::string, std::string> map_;

	login(map_);

	return false;
}

std::string PolyMost::getDatabaseName() const {
	return "polymost";
}

void PolyMost::login(std::map<std::string, std::string> fields) {
	MattermostAccount account;
	Notification notification = LoginSuccessNotification(&account);

	core->getNotificationHandler().notify(&notification);
}

#endif
