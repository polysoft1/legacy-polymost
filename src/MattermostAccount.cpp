#include "headers/MattermostAccount.h"
#include "include/ITeam.h"
#include "include/IConversationManager.h"
#include "headers/PolyMost.h"

void MattermostAccount::requestUpdates() {
	Polychat::HTTPMessage getTeamsMessage(Polychat::HTTPMethod::GET, "/api/v4/users/me/teams");
	Polychat::HTTPMessage teamsResponse = core.getCommunicator().sendRequestSync(host,
		port, ssl, getTeamsMessage);

	std::shared_ptr<Polychat::IHTTPContent> teamsResponseContent = teamsResponse.getContent();
	nlohmann::json teamsJSON = nlohmann::json::parse(teamsResponseContent->getAsString());

	IConversationManager& conversationManager = core.getConversationManager();
	auto existingTeams = conversationManager.getTeams(&protocol);

	// Set is to store which teams have been processed from the server's
	// JSON, so we know which ones don't exist, allowing us to archive them.
	std::set<std::string> serverTeamIDs;
	/*std::map<std::string, std::shared_ptr<ITeam>> existingTeams =
		std::map<std::string, std::shared_ptr<ITeam>>(existingTeamsRef.cbegin(), existingTeamsRef.cend());*/

	// Goes through each team to ensure that the core has it stored.
	// For each team, goes through all conversations.
	for (auto& element : teamsJSON) {
		std::string id = element.at("id").get<std::string>();
		std::string displayName = element.at("display_name").get<std::string>();
		std::string name = element.at("name").get<std::string>();
		std::string description = element.at("description").get<std::string>();

		serverTeamIDs.insert(id);

		// Add team if it does not exist.
		auto existingTeamItr = existingTeams.find(id);
		if (existingTeamItr == existingTeams.end()) {
			// Adds it
			std::shared_ptr<ITeam> newTeam = conversationManager.addTeam(id, &protocol, displayName, name);
			newTeam->setDescription(description);
		}
		else {
			// Gets it to ensure it's up to date.
			std::shared_ptr<ITeam> existingTeam = existingTeamItr->second;
			if (existingTeam->getName().compare(name) != 0)
				existingTeam->setName(name);
			if (existingTeam->getDisplayName().compare(displayName) != 0)
				existingTeam->setDisplayName(displayName);
			if (existingTeam->getDescription().compare(description) != 0)
				existingTeam->setDescription(description);
		}
	}

	//HTTPMessage message(HTTPMethod::GET, "/api/v4/users/me/teams/{team_id}/ch");

	//core.getCommunicator().sendRequestSync(host, port, ssl, message);
}