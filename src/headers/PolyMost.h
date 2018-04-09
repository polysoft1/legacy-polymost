#ifndef POLYMOST_H
#define POLYMOST_H

#include "include/IProtocolPlugin.h"
#include "include/IPlugin.h"
#include "include/Core.h"
#include <string>
#include <vector>

using namespace Polychat;

class PolyMost : public IProtocolPlugin {
public:
	PolyMost();

	~PolyMost();

	virtual std::string getName();

	virtual bool initialize(Core* core);

	virtual std::string getDatabaseName() const;

	std::string token;// TODO: this is temporary! Do not leave it like this!
	std::string team;// TODO: this is temporary! Do not leave it like this!
	std::string user;// TODO: this is temporary! Do not leave it like this!
	std::string channel;// TODO: this is temporary! Do not leave it like this!

	virtual void login(std::map<std::string, std::string> fields);
	virtual const std::vector<LoginField> loginFields() const { return loginFieldsList; };

private:
	Core* core = nullptr;
	std::vector<LoginField> loginFieldsList;
};
#endif
