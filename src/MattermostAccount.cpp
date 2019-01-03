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
	auto existingTeamsRef = conversationManager.getTeams(&protocol);
	/*std::map<std::string, std::shared_ptr<ITeam>> existingTeams =
		std::map<std::string, std::shared_ptr<ITeam>>(existingTeamsRef.cbegin(), existingTeamsRef.cend());*/

	// Goes through each team to ensure that the core has it stored.
	// For each team, goes through all conversations.
	for (auto& element : teamsJSON) {
		std::string id = element.at("id").get<std::string>();
		std::string displayName = element.at("display_name").get<std::string>();
		std::string name = element.at("name").get<std::string>();
		std::string description = element.at("description").get<std::string>();
		// TODO: Properly update it.
		/*if (!existingTeams.erase(id)) {
			// Doesn't exist!
			conversationManager.addConversation(id,
				account, CONVERSATION_TYPE type, std::shared_ptr<ITeam> team,
				std::string title = "Conversation");
		}*/
		
	}

	//HTTPMessage message(HTTPMethod::GET, "/api/v4/users/me/teams/{team_id}/ch");

	//core.getCommunicator().sendRequestSync(host, port, ssl, message);
}