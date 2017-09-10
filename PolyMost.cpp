#ifndef POLYMOST
#define POLYMOST

#include "PolyMost.h"


std::string PolyMost::getName() {
	return "PolyMost";
}

bool PolyMost::initialize() {
	return false;
}

std::string PolyMost::getDatabaseName() {
	return "polymost";
}

bool PolyMost::sendMessage(Message msg) {
	return false;
}

#endif
