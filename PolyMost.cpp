#ifndef POLYMOST
#define POLYMOST

#include "IProtocolPlugin.h"
#include "PolyMost.h"

string PolyMost::getName() {
	return "PolyMost";
}

bool PolyMost::initialize() {
	return false;
}

string PolyMost::getDatabaseName() {
	return "polymost";
}

bool PolyMost::sendMessage(Message msg) {
	return false;
}

#endif
