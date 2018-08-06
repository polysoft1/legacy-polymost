#ifndef POLYMOST_H
#define POLYMOST_H

#include "include/IProtocolPlugin.h"
#include "include/IPlugin.h"
#include "include/ICore.h"
#include "Poco/ClassLibrary.h"
#include <string>
#include <vector>

using namespace Polychat;

class PolyMost : public IProtocolPlugin {
public:
	PolyMost();

	~PolyMost();

	virtual std::string getName();

	virtual bool initialize(ICore* core);

	virtual std::string getDatabaseName() const;

	virtual std::shared_ptr<IAccount> login(std::map<std::string, std::string> fields);
	virtual const std::vector<LoginField> loginFields() const { return loginFieldsList; };

	virtual bool resumeConnections() {
		return false;
	};

	virtual bool suspendConnections() {
		return false;
	};

private:
	ICore* core = nullptr;
	std::vector<LoginField> loginFieldsList;
};
#endif
