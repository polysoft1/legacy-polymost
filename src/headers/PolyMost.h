#ifndef POLYMOST_H
#define POLYMOST_H

#include "headers/MattermostAccount.h"

#include "include/IProtocolPlugin.h"
#include "include/IPlugin.h"
#include "include/ICore.h"
#include "Poco/ClassLibrary.h"
#include <string>
#include <vector>
#include <set>

using namespace Polychat;

class PolyMost : public IProtocolPlugin {
public:
	PolyMost();

	~PolyMost();

	virtual std::string getPluginName() const;
	virtual std::string getProtocolName() const;

	virtual bool initialize(ICore* core);

	virtual std::string getDatabaseName() const;

	virtual std::shared_ptr<IAccount> login(std::map<std::string, std::string> fields);
	virtual const std::vector<LoginField>& loginFields() const { return loginFieldsList; };

	virtual bool startConnections() {
		return false;
	};

	virtual bool stopConnections() {
		return false;
	};

	virtual bool usesTeams() { return true; }

private:
	ICore* core = nullptr;
	std::vector<LoginField> loginFieldsList;
	std::set<std::shared_ptr<MattermostAccount>> accounts;
};
#endif
