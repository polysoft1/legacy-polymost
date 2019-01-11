#include "include/IProtocolPlugin.h"
#include "include/IPlugin.h"
#include "include/ICore.h"
#include "Poco/ClassLibrary.h"
#include <string>
#include <vector>
#include <set>

using namespace Polychat;

class MattermostAccountSession;

class PolyMost : public IProtocolPlugin {
public:
	PolyMost();

	~PolyMost();

	virtual std::string getPluginName() const;
	virtual std::string getProtocolName() const;

	virtual bool initialize(ICore* core);

	virtual std::string getDatabaseName() const;

	virtual AUTH_RESULT login(std::map<std::string, std::string> fields, IAccount& login);

	virtual const std::vector<LoginField>& loginFields() const { return loginFieldsList; };

	virtual bool connectionsActive() {
		return true;
	}

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
	std::set<std::shared_ptr<MattermostAccountSession>> sessions;
};
