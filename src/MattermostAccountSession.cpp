#include "MattermostAccountSession.h"
#include "include/ITeam.h"
#include "PolyMost.h"
#include <chrono>

MattermostAccountSession::MattermostAccountSession(Polychat::IAccount& coreAccount, std::string host,
	unsigned int port, bool ssl, std::string token, Polychat::ICore& core) :
	coreAccount(coreAccount), core(core), host(host), ssl(ssl), token(token), port(port)
{
	tokenIsValid = true;

	webSocketConnection = core.getCommunicator().initWebsocket(host, port, ssl, "/api/v4/websocket");
	webSocketConnection->setOnStringReceived(std::bind(&MattermostAccountSession::onWSMessageReceived, this, std::placeholders::_1));
	webSocketConnection->setOnWebsocketOpen(std::bind(&MattermostAccountSession::onWSOpen, this));
	webSocketConnection->open();
}

void MattermostAccountSession::onWSMessageReceived(std::string msg) {
	nlohmann::json json = nlohmann::json::parse(msg);
	nlohmann::json::iterator seqFindResult = json.find("seq");
	if (seqFindResult == json.end()) {
		core.alert("Websocket acknowledgement from server: " + msg);
	} else {
		core.alert("Websocket from server: " + msg);

		//std::string response = "{\"status\": \"OK\", \"seq_reply\": " + std::to_string(seqFindResult->get<int>()) + " }";
		//core.alert("Responding with " + response);
		//webSocketConnection->sendFrame(response.c_str(), response.length(), true);
	}
}

void MattermostAccountSession::onWSOpen() {
	std::string authChallenge = "{ \"seq\": 1, \"action\" : \"authentication_challenge\", \"data\" : { \"token\": \"" + token + "\" } }";
	std::cout << "Sending " << authChallenge << " with length " << authChallenge.length() << std::endl;
	webSocketConnection->send(authChallenge.data(), authChallenge.length(), true);
}

void MattermostAccountSession::onWSClose() {
	// TODO: Try reopening?
}

void MattermostAccountSession::refresh(std::shared_ptr<IConversation> currentlyViewedConversation) {
	updateTeams();
	auto teams = coreAccount.getTeams();
	for (auto const& teamPair : teams)
	{
		updateConversations(*teamPair.second.get());
	}
}

void MattermostAccountSession::updatePosts(IConversation& conversation, int limit) {
	// TODO
}

bool MattermostAccountSession::isValid() {
	return tokenIsValid;
}

void MattermostAccountSession::updateTeams() {
	Polychat::HTTPMessage getTeamsMessage(Polychat::HTTPMethod::GET, "/api/v4/users/me/teams");
	getTeamsMessage.setAuthorization("Bearer", token);
	Polychat::HTTPMessage teamsResponse = core.getCommunicator().sendRequestSync(host,
		port, ssl, getTeamsMessage);

	std::shared_ptr<Polychat::IHTTPContent> teamsResponseContent = teamsResponse.getContent();
	if (teamsResponse.getStatus() == HTTPStatus::HTTP_OK) {
		nlohmann::json teamsJSON = nlohmann::json::parse(teamsResponseContent->getAsString());

		auto existingTeams = coreAccount.getTeams();

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
				std::shared_ptr<ITeam> newTeam = coreAccount.loadTeam(id, displayName, name);
				newTeam->setDescription(description);
			} else {
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
	} else {
		// TODO: More detailed handling.
		core.getLogger().write("Unable to update teams. Token: " + token + ". Response from server: "
			+ std::to_string(static_cast<int>(teamsResponse.getStatus()))
			+ " " + teamsResponseContent->getAsString(), LogLevel::WARNING);
	}
}

void MattermostAccountSession::updateConversations(ITeam& team) {
	// This request gets all of the channels for a user in that team. It also includes a "last_post_at"
	// field, making it easier to know if the conversation needs updating.Okay
	Polychat::HTTPMessage getChannelsMessage(Polychat::HTTPMethod::GET, "/api/v4/users/me/teams/" + team.getID() + "/channels");
	getChannelsMessage.setAuthorization("Bearer", token);
	Polychat::HTTPMessage channelsResponse = core.getCommunicator().sendRequestSync(host,
		port, ssl, getChannelsMessage);

	std::shared_ptr<Polychat::IHTTPContent> channelsResponseContent = channelsResponse.getContent();
	if (channelsResponse.getStatus() == HTTPStatus::HTTP_OK) {
		nlohmann::json channelsJSON = nlohmann::json::parse(channelsResponseContent->getAsString());

		auto existingTeamChannels = team.getConversations();
		auto existingUserChannels = coreAccount.getConversations();

		// Set is to store which teams have been processed from the server's
		// JSON, so we know which ones don't exist, allowing us to archive them.
		std::set<std::string> serverChannelIDs;
		/*std::map<std::string, std::shared_ptr<ITeam>> existingTeams =
			std::map<std::string, std::shared_ptr<ITeam>>(existingTeamsRef.cbegin(), existingTeamsRef.cend());*/

			// Goes through each team to ensure that the core has it stored.
			// For each team, goes through all conversations.
		for (auto& element : channelsJSON) {
			std::string id = element.at("id").get<std::string>();
			std::string displayName = element.at("display_name").get<std::string>();
			std::string name = element.at("name").get<std::string>();
			std::string description = element.at("header").get<std::string>();
			std::string type = element.at("type").get<std::string>();
			std::string teamId = element.at("team_id").get<std::string>();
			CONVERSATION_TYPE parsedType = getTypeFromChar(type.at(0));

			serverChannelIDs.insert(id);

			// Add channel if it does not exist.
			bool newChannel;
			std::map<std::string, std::shared_ptr<Polychat::IConversation>>::iterator existingChannelsItr;
			if (!teamId.empty()) {
				existingChannelsItr = existingUserChannels.find(id);
				newChannel = existingChannelsItr == existingUserChannels.end();
			} else {
				existingChannelsItr = existingTeamChannels.find(id);
				newChannel = existingChannelsItr == existingTeamChannels.end();
			}

			if (newChannel) {
				// Adds it
				std::shared_ptr<IConversation> newConversation;
				if (!teamId.empty())
					newConversation = team.addConversation(id, parsedType, displayName);
				else
					newConversation = coreAccount.loadConversation(id, parsedType, displayName);

				newConversation->setDescription(description);
				newConversation->setName(name);
			} else {
				// Gets it to ensure it's up to date.
				std::shared_ptr<IConversation> existingConversation = existingChannelsItr->second;
				if (existingConversation->getName().compare(name) != 0)
					existingConversation->setName(name);
				if (existingConversation->getTitle().compare(displayName) != 0)
					existingConversation->setTitle(displayName);
				if (existingConversation->getDescription().compare(description) != 0)
					existingConversation->setDescription(description);
				if (existingConversation->getType() != parsedType)
					existingConversation->setType(parsedType);
			}
		}
	} else {
		// TODO: More detailed handling.
		core.getLogger().write("Unable to update conversations. Token: " + token + ". Response from server: "
			+ std::to_string(static_cast<int>(channelsResponse.getStatus()))
			+ " " + channelsResponseContent->getAsString(), LogLevel::WARNING);
	}
}

CONVERSATION_TYPE MattermostAccountSession::getTypeFromChar(char input) {
	switch (input) {
	case 'O': // I think this stands for "open channel"
		return CONVERSATION_TYPE::PUBLIC_CHANNEL;
	case 'P': // I think this stands for "private channel"
		return CONVERSATION_TYPE::PRIVATE_CHANNEL;
	case 'D': // I think this stands for "direct message"
		return CONVERSATION_TYPE::DIRECT_MESSAGE;
	case 'G': // I think this stands for "group message"
		return CONVERSATION_TYPE::GROUP_MESSAGE;
	}
}