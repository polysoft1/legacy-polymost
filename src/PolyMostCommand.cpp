#include "headers/PolyMostCommand.h"
#include "include/ICommunicator.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>
#include "mattermost.pb.h"

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/String.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Net/Context.h"
//#include <curlpp/cURLpp.hpp>
//#include <curlpp/Easy.hpp>
//#include <curlpp/Options.hpp>

PolyMostCommand::PolyMostCommand(PolyMost& mainReference, Core& coreReference) :
	main(mainReference), core(coreReference), printOptions(), parseOptions() {

	server = new MattermostServer("mattermost.keithserver.net", 443, "/api/v4");
	user = nullptr;
	printOptions.preserve_proto_field_names = true;
	parseOptions.ignore_unknown_fields = true;
	Poco::Net::initializeSSL();
}

PolyMostCommand::~PolyMostCommand() {
	delete server;
	Poco::Net::uninitializeSSL();
}

bool PolyMostCommand::onCommand(std::vector<std::string> args) {
	if (args.size() > 0) {

		std::string command = args[0];
		args.erase(args.begin());

		if (Poco::icompare(command, "login") == 0) {
			return loginCommand(args);
		} else if (Poco::icompare(command, "teams") == 0) {
			return listTeamsCommand(args);
		} else if (Poco::icompare(command, "selectteam") == 0) {
			return selectTeamCommand(args);
		} else if (Poco::icompare(command, "selectchannel") == 0) {
			return selectChannelCommand(args);
		} else if (Poco::icompare(command, "channels") == 0) {
			return listChannelsCommand(args);
		} else if (Poco::icompare(command, "send") == 0) {
			return sendMessageCommand(args);
		}
	}
	core.alert("Commands: login, send, teams, channels, selectteam, selectchannel");
	return false;
};

void PolyMostCommand::handleError(std::string &json, google::protobuf::util::JsonParseOptions parseOptions) {
	mattermost::Error errorResponse;
	google::protobuf::util::Status responseResult
		= google::protobuf::util::JsonStringToMessage(json, &errorResponse, parseOptions);

	if (responseResult.ok()) {
		core.alert(errorResponse.message());
	} else {
		core.alert("Unknown error " + responseResult.ToString());
	}
}

bool PolyMostCommand::loginCommand(std::vector<std::string> args) {
	core.alert("Trying..");
	if (args.size() == 2) {
		core.alert("Correct num args");
		if (this->user != nullptr && this->user->token != "") {
			core.alert("Already logged in");
			return true;
		}

		mattermost::Login login;

		login.set_login_id(args[0]);
		login.set_password(args[1]);

		std::string loginJSON;

		google::protobuf::util::Status sendResult
			= google::protobuf::util::MessageToJsonString(login, &loginJSON, printOptions);

		if (sendResult.ok()) {
			core.alert("Formatted..");

			try {


				try {

					HTTPMessage message(HTTPMethod::POST, server->getURI() + "/users/login");
					std::shared_ptr<HTTPStringContent> requestContent(new HTTPStringContent(loginJSON));
					requestContent->setContentType("application/json");
					message.setContent(requestContent);

					HTTPMessage response = core.getCommunicator().sendRequestSync(this->server->getAddress(), this->server->getPort(), message);

					std::shared_ptr<HTTPStringContent> responseContent = std::static_pointer_cast<HTTPStringContent>(response.getContent());
					std::string responseJSON = responseContent->getContent();

					if (response.count("Token")) {

						// Get the token
						std::string token = response["Token"];

						// Get the user
						mattermost::User* userResponse = mattermost::User::default_instance().New(); // To keep for later
						google::protobuf::util::Status responseResult
							= google::protobuf::util::JsonStringToMessage(responseJSON, userResponse, parseOptions);

						if (responseResult.ok()) {
							this->user = new MattermostUser(userResponse);
							this->user->token = token;

							core.alert("Logged in!");
						} else {
							core.alert("Error parsing result. " + responseResult.ToString());
						}

					} else {
						core.alert("No token.");
						handleError(responseJSON, parseOptions);
					}
				} catch (Poco::Exception& e) {
					core.alert("Error: " + e.displayText());
					return false;
				}


			} catch (const std::invalid_argument&) {
				core.alert("Invalid port.");
			}
		} else {
			core.alert("Could not parse input");
		}
	} else {
		core.alert("Usage: <username/email> <password> NOTE: This is not over a secure connection!");
	}
	return true;
}


bool PolyMostCommand::listTeamsCommand(std::vector<std::string> args) {
	if (args.size() >= 0 && args.size() <= 2) {

		if (this->user == nullptr) {
			core.alert("Not logged in.");
			return true;
		} else if (this->user->token.size() == 0) {
			core.alert("Invalid login token.");
			return true;
		}

		HTTPMessage message(HTTPMethod::GET, server->getURI() + "/users/me/teams");
		message.setAuthorization("Bearer", this->user->token);

		HTTPMessage response = core.getCommunicator().sendRequestSync(this->server->getAddress(), this->server->getPort(), message);

		std::shared_ptr<HTTPStringContent> responseContent = std::static_pointer_cast<HTTPStringContent>(response.getContent());
		std::string responseJSON = responseContent->getContent();

		// Get the team list object
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var parsed = parser.parse(responseJSON);

		Poco::JSON::Stringifier::stringify(parsed, std::cout, 1);
		/*Poco::JSON::Parser parser;
		Poco::Dynamic::Var parsed = parser.parse(s);

		int size = parsed.size();
		core.alert("Teams: (" + size + ")");
		for (int i = 0; i < size; i++) {
			mattermost::Team teamsRespons;

			google::protobuf::util::Status responseResult
				= google::protobuf::util::JsonStringToMessage("\"teams\": []", &teamsResponse, parseOptions);

			if (responseResult.ok()) {
				int size = teamsResponse.teams_size();
				core.alert("Teams: (" + size + ")");
				for (int i = 0; i < size; i++) {
					core.alert("- " + teamsResponse.teams(i).name());
				}
			} else {
				core.alert("Error parsing result. " + responseResult.ToString() + " (" + responseJSON + ")");
			}
		}*/

		/*
			std::ostringstream os;
			os << s.rdbuf();
			std::string responseJSON = os.str();
			handleError(responseJSON, parseOptions);
		*/

	} else {
		core.alert("Usage: teams [page] [per_page]");
	}
	return true;
}

bool PolyMostCommand::listChannelsCommand(std::vector<std::string> args) {
	if (args.size() == 0) {

		if (this->user == nullptr) {
			core.alert("Not logged in.");
			return true;
		} else if (this->user->token.size() == 0) {
			core.alert("Invalid login token.");
			return true;
		} else if (this->user->team.size() == 0) {
			core.alert("No team selected. Select one with \"mattermost selectteam\"");
			return true;
		} else if (this->user->userData == nullptr
			|| !this->user->userData->has_id()) {
			core.alert("No user ID. Login with \"mattermost login\"");
			return true;
		}

		core.alert("Getting channels of team " + this->user->team);

		std::string URL = this->server->getURI() + "/users/" + this->user->userData->id() + "/teams/" +
			this->user->team + "/channels";
		HTTPMessage message(HTTPMethod::GET, URL);
		message.setAuthorization("Bearer", this->user->token);

		HTTPMessage response = core.getCommunicator().sendRequestSync(this->server->getAddress(), this->server->getPort(), message);

		std::shared_ptr<HTTPStringContent> responseContent = std::static_pointer_cast<HTTPStringContent>(response.getContent());
		std::string responseJSON = responseContent->getContent();

		// Get the team list object
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var parsed = parser.parse(responseJSON);

		Poco::JSON::Stringifier::stringify(parsed, std::cout, 1);
		core.alert("");

		/*Poco::JSON::Object::Ptr userObjectJSON = parsed.extract<Poco::JSON::Object::Ptr>();
		core.alert response.getStatus() + " " + response.getReason());

		if (userObjectJSON->has("message")) {

			core.alert(userObjectJSON->get("message").toString());
		}
		else {
			core.alert("Unknown error");
		}
		}*/

	} else {
		core.alert("Usage: channels");
	}
	return true;
}

bool PolyMostCommand::selectTeamCommand(std::vector<std::string> args) {
	if (args.size() == 1) {

		if (this->user == nullptr) {
			core.alert("Not logged in.");
			return true;
		} else if (this->user->token.size() == 0) {
			core.alert("Invalid login token.");
			return true;
		}

		HTTPMessage message(HTTPMethod::GET, this->server->getURI() + "/teams/name/" + args[0]);
		message.setAuthorization("Bearer", this->user->token);

		HTTPMessage response = core.getCommunicator().sendRequestSync(this->server->getAddress(), this->server->getPort(), message);

		std::shared_ptr<HTTPStringContent> responseContent = std::static_pointer_cast<HTTPStringContent>(response.getContent());
		std::string responseJSON = responseContent->getContent();

		// Get the team list object
		core.alert("Status: " + response.getStatus());
		if (response.getStatus() == HTTPStatus::HTTP_OK) {
			mattermost::Team teamResponse;
			google::protobuf::util::Status responseResult
				= google::protobuf::util::JsonStringToMessage(responseJSON, &teamResponse, parseOptions);

			if (responseResult.ok()) {
				core.alert("Team selected");
				this->user->team = teamResponse.id();
			} else {
				core.alert("Error parsing result. " + responseResult.ToString());
			}
		} else {
			handleError(responseJSON, parseOptions);
		}

	} else {
		core.alert("Usage: selectteam <name>");
	}
	return true;
}

bool PolyMostCommand::selectChannelCommand(std::vector<std::string> args) {
	if (args.size() == 1) {

		if (this->user == nullptr) {
			core.alert("Not logged in.");
			return true;
		} else if (this->user->token.size() == 0) {
			core.alert("Invalid login token.");
			return true;
		} else if (this->user->team.size() == 0) {
			core.alert("No team selected. Select one with \"mattermost selectteam\"");
			return true;
		} else if (this->user->userData == nullptr
			|| !this->user->userData->has_id()) {
			core.alert("No user ID. Login with \"mattermost login\"");
			return true;
		}

		HTTPMessage message(HTTPMethod::GET, this->server->getURI() + "/teams/" + this->user->team + "/channels/name/" + args[0]);
		message.setAuthorization("Bearer", this->user->token);

		HTTPMessage response = core.getCommunicator().sendRequestSync(this->server->getAddress(), this->server->getPort(), message);

		std::shared_ptr<HTTPStringContent> responseContent = std::static_pointer_cast<HTTPStringContent>(response.getContent());
		std::string responseJSON = responseContent->getContent();

		// Get the channel object
		if (response.getStatus() == HTTPStatus::HTTP_OK) {
			mattermost::Channel channelResponse;
			google::protobuf::util::Status responseResult
				= google::protobuf::util::JsonStringToMessage(responseJSON, &channelResponse, parseOptions);

			if (responseResult.ok()) {
				core.alert("Channel selected");
				this->user->channel = channelResponse.id();
			} else {
				core.alert("Error parsing result. " + responseResult.ToString());
			}
		} else {
			handleError(responseJSON, parseOptions);
		}

	} else {
		core.alert("Usage: selectchannel <name>");
	}
	return true;
}

bool PolyMostCommand::sendMessageCommand(std::vector<std::string> args) {
	if (args.size() > 0) {

		if (this->user == nullptr) {
			core.alert("Not logged in.");
			return true;
		} else if (this->user->token.size() == 0) {
			core.alert("Invalid login token.");
			return true;
		} else if (this->user->channel.size() == 0) {
			core.alert("No channel selected.");
			return true;
		}

		std::string message = "";
		for (int i = 0; i < args.size(); i++) {
			if (i > 0)
				message += " ";
			message += args[i];
		}

		mattermost::Post newPost;
		newPost.set_channel_id(this->user->channel);
		newPost.set_message(message);

		std::string newPostJSON;

		google::protobuf::util::Status sendResult
			= google::protobuf::util::MessageToJsonString(newPost, &newPostJSON, printOptions);

		if (sendResult.ok()) {

			try {

				HTTPMessage message(HTTPMethod::POST, this->server->getURI() + "/posts");
				std::shared_ptr<HTTPStringContent> requestContent(new HTTPStringContent(newPostJSON));
				requestContent->setContentType("application/json");
				message.setContent(requestContent);
				message.setAuthorization("Bearer", this->user->token);

				HTTPMessage response = core.getCommunicator().sendRequestSync(this->server->getAddress(), this->server->getPort(), message);

				std::shared_ptr<HTTPStringContent> responseContent = std::static_pointer_cast<HTTPStringContent>(response.getContent());
				std::string responseJSON = responseContent->getContent();


				if (response.getStatus() == HTTPStatus::HTTP_OK) {
					mattermost::Post postResponse;
					google::protobuf::util::Status responseResult
						= google::protobuf::util::JsonStringToMessage(responseJSON, &postResponse, parseOptions);

					if (responseResult.ok()) {
						core.alert("Sent at " + postResponse.create_at());
					} else {
						core.alert("Error parsing result. " + responseResult.ToString());
					}
				} else {
					handleError(responseJSON, parseOptions);
				}

			} catch (const std::invalid_argument&) {
				core.alert("Invalid port.");
			}
		}
	} else {
		core.alert("Usage: <username/email> <password> NOTE: This is not over a secure connection!");
	}
	return true;
}

std::vector<std::string>* PolyMostCommand::onTabCompletion(std::vector<std::string>, int position) {
	return nullptr;
};
