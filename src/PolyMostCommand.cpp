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
	std::cout << "Commands: login, send, teams, channels, selectteam, selectchannel" << std::endl;
	return false;
};

void handleError(std::string &json, google::protobuf::util::JsonParseOptions parseOptions) {
	mattermost::Error errorResponse;
	google::protobuf::util::Status responseResult
		= google::protobuf::util::JsonStringToMessage(json, &errorResponse, parseOptions);

	if (responseResult.ok()) {
		std::cout << errorResponse.message() << std::endl;
	} else {
		std::cout << "Unknown error " << responseResult.ToString() << std::endl;
	}
}

bool PolyMostCommand::loginCommand(std::vector<std::string> args) {
	std::cout << "Trying..\n";
	if (args.size() == 2) {
		std::cout << "Correct num args\n";
		if (this->user != nullptr && this->user->token != "") {
			std::cout << "Already logged in" << std::endl;
			return true;
		}

		mattermost::Login login;

		login.set_login_id(args[0]);
		login.set_password(args[1]);

		std::string loginJSON;

		google::protobuf::util::Status sendResult
			= google::protobuf::util::MessageToJsonString(login, &loginJSON, printOptions);

		if (sendResult.ok()) {
			std::cout << "Formatted..\n";

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

							std::cout << "Logged in!" << std::endl;
						} else {
							std::cout << "Error parsing result. " << responseResult.ToString() << std::endl;
						}

					} else {
						std::cout << "No token." << std::endl;
						handleError(responseJSON, parseOptions);
					}
				} catch (Poco::Exception& e) {
					std::cout << "Error: " << e.displayText() << "\n";
					return false;
				}


			} catch (const std::invalid_argument&) {
				std::cout << "Invalid port." << std::endl;
			}
		} else {
			std::cout << "Could not parse input" << std::endl;
		}
	} else {
		std::cout << "Usage: <username/email> <password> NOTE: This is not over a secure connection!" << std::endl;
	}
	return true;
}


bool PolyMostCommand::listTeamsCommand(std::vector<std::string> args) {
	if (args.size() >= 0 && args.size() <= 2) {

		if (this->user == nullptr) {
			std::cout << "Not logged in." << std::endl;
			return true;
		} else if (this->user->token.size() == 0) {
			std::cout << "Invalid login token." << std::endl;
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
		std::cout << "Teams: (" << size << ")" << std::endl;
		for (int i = 0; i < size; i++) {
			mattermost::Team teamsRespons;

			google::protobuf::util::Status responseResult
				= google::protobuf::util::JsonStringToMessage("\"teams\": []", &teamsResponse, parseOptions);

			if (responseResult.ok()) {
				int size = teamsResponse.teams_size();
				std::cout << "Teams: (" << size << ")" << std::endl;
				for (int i = 0; i < size; i++) {
					std::cout << "- " << teamsResponse.teams(i).name();
				}
			} else {
				std::cout << "Error parsing result. " << responseResult.ToString() << " (" << responseJSON << ")" << std::endl;
			}
		}*/

		/*
			std::ostringstream os;
			os << s.rdbuf();
			std::string responseJSON = os.str();
			handleError(responseJSON, parseOptions);
		*/

	} else {
		std::cout << "Usage: teams [page] [per_page]" << std::endl;
	}
	return true;
}

bool PolyMostCommand::listChannelsCommand(std::vector<std::string> args) {
	if (args.size() == 0) {

		if (this->user == nullptr) {
			std::cout << "Not logged in." << std::endl;
			return true;
		} else if (this->user->token.size() == 0) {
			std::cout << "Invalid login token." << std::endl;
			return true;
		} else if (this->user->team.size() == 0) {
			std::cout << "No team selected. Select one with \"mattermost selectteam\"" << std::endl;
			return true;
		} else if (this->user->userData == nullptr
			|| !this->user->userData->has_id()) {
			std::cout << "No user ID. Login with \"mattermost login\"" << std::endl;
			return true;
		}

		std::cout << "Getting channels of team " << this->user->team << std::endl;

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
		std::cout << std::endl;

		/*Poco::JSON::Object::Ptr userObjectJSON = parsed.extract<Poco::JSON::Object::Ptr>();
		std::cout << response.getStatus() << " " << response.getReason() << std::endl;

		if (userObjectJSON->has("message")) {

			std::cout << userObjectJSON->get("message").toString() << std::endl;
		}
		else {
			std::cout << "Unknown error" << std::endl;
		}
		}*/

	} else {
		std::cout << "Usage: channels" << std::endl;
	}
	return true;
}

bool PolyMostCommand::selectTeamCommand(std::vector<std::string> args) {
	if (args.size() == 1) {

		if (this->user == nullptr) {
			std::cout << "Not logged in." << std::endl;
			return true;
		} else if (this->user->token.size() == 0) {
			std::cout << "Invalid login token." << std::endl;
			return true;
		}

		HTTPMessage message(HTTPMethod::GET, this->server->getURI() + "/teams/name/" + args[0]);
		message.setAuthorization("Bearer", this->user->token);

		HTTPMessage response = core.getCommunicator().sendRequestSync(this->server->getAddress(), this->server->getPort(), message);

		std::shared_ptr<HTTPStringContent> responseContent = std::static_pointer_cast<HTTPStringContent>(response.getContent());
		std::string responseJSON = responseContent->getContent();

		// Get the team list object
		std::cout << "Status: " << response.getStatus() << std::endl;
		if (response.getStatus() == HTTPStatus::HTTP_OK) {
			mattermost::Team teamResponse;
			google::protobuf::util::Status responseResult
				= google::protobuf::util::JsonStringToMessage(responseJSON, &teamResponse, parseOptions);

			if (responseResult.ok()) {
				std::cout << "Team selected" << std::endl;
				this->user->team = teamResponse.id();
			} else {
				std::cout << "Error parsing result. " << responseResult.ToString() << std::endl;
			}
		} else {
			handleError(responseJSON, parseOptions);
		}

	} else {
		std::cout << "Usage: selectteam <name>" << std::endl;
	}
	return true;
}

bool PolyMostCommand::selectChannelCommand(std::vector<std::string> args) {
	if (args.size() == 1) {

		if (this->user == nullptr) {
			std::cout << "Not logged in." << std::endl;
			return true;
		} else if (this->user->token.size() == 0) {
			std::cout << "Invalid login token." << std::endl;
			return true;
		} else if (this->user->team.size() == 0) {
			std::cout << "No team selected. Select one with \"mattermost selectteam\"" << std::endl;
			return true;
		} else if (this->user->userData == nullptr
			|| !this->user->userData->has_id()) {
			std::cout << "No user ID. Login with \"mattermost login\"" << std::endl;
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
				std::cout << "Channel selected" << std::endl;
				this->user->channel = channelResponse.id();
			} else {
				std::cout << "Error parsing result. " << responseResult.ToString() << std::endl;
			}
		} else {
			handleError(responseJSON, parseOptions);
		}

	} else {
		std::cout << "Usage: selectchannel <name>" << std::endl;
	}
	return true;
}

bool PolyMostCommand::sendMessageCommand(std::vector<std::string> args) {
	if (args.size() > 0) {

		if (this->user == nullptr) {
			std::cout << "Not logged in." << std::endl;
			return true;
		} else if (this->user->token.size() == 0) {
			std::cout << "Invalid login token." << std::endl;
			return true;
		} else if (this->user->channel.size() == 0) {
			std::cout << "No channel selected." << std::endl;
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
						std::cout << "Sent at " << postResponse.create_at() << std::endl;
					} else {
						std::cout << "Error parsing result. " << responseResult.ToString() << std::endl;
					}
				} else {
					handleError(responseJSON, parseOptions);
				}

			} catch (const std::invalid_argument&) {
				std::cout << "Invalid port." << std::endl;
			}
		}
	} else {
		std::cout << "Usage: <username/email> <password> NOTE: This is not over a secure connection!" << std::endl;
	}
	return true;
}

std::vector<std::string>* PolyMostCommand::onTabCompletion(std::vector<std::string>, int position) {
	return nullptr;
};
