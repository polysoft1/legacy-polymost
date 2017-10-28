#include "headers/PolyMostCommand.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/String.h"

const std::string WEB_ADDRESS = "www.keithserver.net";
const int WEB_PORT = 8065;
const std::string APIv4_URI = "/api/v4";



PolyMostCommand::PolyMostCommand(PolyMost& mainReference) : main(mainReference) {
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

bool PolyMostCommand::loginCommand(std::vector<std::string> args) {
	if (args.size() == 2) {

		Poco::JSON::Object loginJSON;
		loginJSON.set("login_id", args[0]);
		loginJSON.set("password", args[1]);

		std::ostringstream oss;
		loginJSON.stringify(oss);

		try {
			std::cout << "Attempting to connect to login. JSON: " << oss.str() << std::endl;

			Poco::Net::HTTPClientSession clientSession(WEB_ADDRESS, WEB_PORT);
			Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, APIv4_URI + "/users/login", Poco::Net::HTTPRequest::HTTP_1_1);
			Poco::Net::HTTPResponse response;

			request.setKeepAlive(true);
			request.setContentType("application/json");
			request.setContentLength(oss.str().size());

			std::ostream& o = clientSession.sendRequest(request);

			loginJSON.stringify(o);

			Poco::JSON::Object recieved;

			std::cout << response.getStatus() << " " << response.getReason() << std::endl;

			std::istream& s = clientSession.receiveResponse(response);

			// Get the user object
			Poco::JSON::Parser parser;
			Poco::Dynamic::Var parsed = parser.parse(s);
			Poco::JSON::Object::Ptr userObjectJSON = parsed.extract<Poco::JSON::Object::Ptr>();

			if (response.has("Token")) {
				// Get the token
				std::string token = response.get("Token");
				std::cout << "Token: " << token << std::endl;
				main.token = token;

				if (userObjectJSON->has("id")) {
					main.user = userObjectJSON->get("id").toString();
				} else {
					std::cout << "Error getting user's ID" << std::endl;
				}

				if (response.has("Set-Cookie")) {
					// Get the token
					std::string token = response.get("Set-Cookie");
					std::cout << "Set-Cookie: " << token << std::endl;
				}

				Poco::JSON::Stringifier::stringify(parsed, true, std::cout, 1);
				std::cout << std::endl;

			} else {
				if (userObjectJSON->has("message")) {

					std::cout << userObjectJSON->get("message").toString() << std::endl;
				} else {
					std::cout << "Unknown error" << std::endl;
				}
			}

		} catch (const std::invalid_argument&) {
			std::cout << "Invalid port." << std::endl;
		}
	} else {
		std::cout << "Usage: <username/email> <password> NOTE: This is not over a secure connection!" << std::endl;
	}
	return true;
}

bool PolyMostCommand::listTeamsCommand(std::vector<std::string> args) {
	if (args.size() >= 0 && args.size() <= 2) {

		if (main.token.size() == 0) {
			std::cout << "Invalid login token." << std::endl;
			return true;
		}

		Poco::JSON::Object loginJSON;
		loginJSON.set("page", args.size() >= 1 ? args[0] : "0");
		loginJSON.set("per_page", args.size() >= 2 ? args[1] : "60");

		std::ostringstream oss;
		loginJSON.stringify(oss);

		Poco::Net::HTTPClientSession clientSession(WEB_ADDRESS, WEB_PORT);
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, APIv4_URI + "/users/me/teams", Poco::Net::HTTPRequest::HTTP_1_1);
		Poco::Net::HTTPResponse response;

		//request.setKeepAlive(true);
		request.setContentType("application/json");
		request.setContentLength(oss.str().size());
		request.setCredentials("Bearer", main.token);
		//request.set("Date", "Sat, 28 Oct 2017 16:09:30 GMT");
		std::ostream& o = clientSession.sendRequest(request);

		loginJSON.stringify(o);

		Poco::JSON::Object recieved;

		std::istream& s = clientSession.receiveResponse(response);

		// Get the team list object
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var parsed = parser.parse(s);

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
			Poco::JSON::Stringifier::stringify(parsed, true, std::cout, 1);
			std::cout << std::endl;
		} else {
			std::cout << response.getStatus() << " " << response.getReason() << std::endl;

			Poco::JSON::Object::Ptr userObjectJSON = parsed.extract<Poco::JSON::Object::Ptr>();
			if (userObjectJSON->has("message")) {

				std::cout << userObjectJSON->get("message").toString() << std::endl;
			} else {
				std::cout << "Unknown error" << std::endl;
			}
		}

	} else {
		std::cout << "Usage: teams [page] [per_page]" << std::endl;
	}
	return true;
}

bool PolyMostCommand::listChannelsCommand(std::vector<std::string> args) {
	if (args.size() == 0) {

		if (main.token.size() == 0) {
			std::cout << "Invalid login token. Login with \"mattermost login\"" << std::endl;
			return true;
		} else if (main.team.size() == 0) {
			std::cout << "No team selected. Select one with \"mattermost selectteam\"" << std::endl;
			return true;
		} else if (main.user.size() == 0) {
			std::cout << "No user ID. Login with \"mattermost login\"" << std::endl;
			return true;
		}

		std::cout << "Getting channels of team " << main.team << std::endl;
		Poco::Net::HTTPClientSession clientSession(WEB_ADDRESS, WEB_PORT);
		// https://your-mattermost-url.com/api/v4/users/{user_id}/teams/{team_id}/channels
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, APIv4_URI + "/users/" + main.user + "/teams/" + main.team + "/channels", Poco::Net::HTTPRequest::HTTP_1_1);
		Poco::Net::HTTPResponse response;

		request.setContentType("application/json");
		request.setContentLength(0);
		request.setCredentials("Bearer", main.token);
		std::ostream& o = clientSession.sendRequest(request);

		Poco::JSON::Object recieved;

		std::istream& s = clientSession.receiveResponse(response);

		// Get the team list object
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var parsed = parser.parse(s);

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
			Poco::JSON::Stringifier::stringify(parsed, true, std::cout, 1);
			std::cout << std::endl;
		} else {
			Poco::JSON::Object::Ptr userObjectJSON = parsed.extract<Poco::JSON::Object::Ptr>();
			std::cout << response.getStatus() << " " << response.getReason() << std::endl;

			if (userObjectJSON->has("message")) {

				std::cout << userObjectJSON->get("message").toString() << std::endl;
			} else {
				std::cout << "Unknown error" << std::endl;
			}
		}

	} else {
		std::cout << "Usage: channels" << std::endl;
	}
	return true;
}

bool PolyMostCommand::selectTeamCommand(std::vector<std::string> args) {
	if (args.size() == 1) {

		if (main.token.size() == 0) {
			std::cout << "Invalid login token." << std::endl;
			return true;
		}

		Poco::Net::HTTPClientSession clientSession(WEB_ADDRESS, WEB_PORT);
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, APIv4_URI + "/teams/name/" + args[0], Poco::Net::HTTPRequest::HTTP_1_1);
		Poco::Net::HTTPResponse response;

		request.setContentType("application/json");
		request.setContentLength(0);
		request.setCredentials("Bearer", main.token);
		std::ostream& o = clientSession.sendRequest(request);

		Poco::JSON::Object recieved;

		std::istream& s = clientSession.receiveResponse(response);

		// Get the team list object
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var parsed = parser.parse(s);
		Poco::JSON::Object::Ptr teamObjectJSON = parsed.extract<Poco::JSON::Object::Ptr>();

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
			if (teamObjectJSON->has("id")) {
				main.team = teamObjectJSON->get("id").toString();
				std::cout << "Team selected" << std::endl;
				
			} else {
				std::cout << "Unknown team?" << std::endl;
			}
		} else {
			std::cout << response.getStatus() << " " << response.getReason() << std::endl;

			if (teamObjectJSON->has("message")) {

				std::cout << teamObjectJSON->get("message").toString() << std::endl;
			} else {
				std::cout << "Unknown error" << std::endl;
			}
		}

	} else {
		std::cout << "Usage: selectteam <name>" << std::endl;
	}
	return true;
}

bool PolyMostCommand::selectChannelCommand(std::vector<std::string> args) {
	if (args.size() == 1) {

		if (main.token.size() == 0) {
			std::cout << "Invalid login token." << std::endl;
			return true;
		}

		Poco::Net::HTTPClientSession clientSession(WEB_ADDRESS, WEB_PORT);
		// https://your-mattermost-url.com/api/v4/teams/{team_id}/channels/name/{channel_name}
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, APIv4_URI + "/teams/" + main.team + "/channels/name/" + args[0], Poco::Net::HTTPRequest::HTTP_1_1);
		Poco::Net::HTTPResponse response;

		request.setContentType("application/json");
		request.setContentLength(0);
		request.setCredentials("Bearer", main.token);
		std::ostream& o = clientSession.sendRequest(request);

		Poco::JSON::Object recieved;

		std::istream& s = clientSession.receiveResponse(response);

		// Get the team list object
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var parsed = parser.parse(s);
		Poco::JSON::Object::Ptr teamObjectJSON = parsed.extract<Poco::JSON::Object::Ptr>();

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
			if (teamObjectJSON->has("id")) {
				main.channel = teamObjectJSON->get("id").toString();
				std::cout << "Channel selected" << std::endl;

			} else {
				std::cout << "Unknown channel?" << std::endl;
			}
		} else {
			std::cout << response.getStatus() << " " << response.getReason() << std::endl;

			if (teamObjectJSON->has("message")) {

				std::cout << teamObjectJSON->get("message").toString() << std::endl;
			} else {
				std::cout << "Unknown error" << std::endl;
			}
		}

	} else {
		std::cout << "Usage: selectchannel <name>" << std::endl;
	}
	return true;
}
bool PolyMostCommand::sendMessageCommand(std::vector<std::string> args) {
	if (args.size() > 0) {

		if (main.channel.size() == 0) {
			std::cout << "No channel selected." << std::endl;
			return true;
		}

		std::string message = "";
		for (int i = 0; i < args.size(); i++) {
			if (i > 0)
				message += " ";
			message += args[i];
		}

		Poco::JSON::Object newPostJSON;
		newPostJSON.set("channel_id", main.channel);
		newPostJSON.set("message", message);
		

		std::ostringstream oss;
		newPostJSON.stringify(oss);

		try {
			Poco::Net::HTTPClientSession clientSession(WEB_ADDRESS, WEB_PORT);
			Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, APIv4_URI + "/posts", Poco::Net::HTTPRequest::HTTP_1_1);
			Poco::Net::HTTPResponse response;

			request.setKeepAlive(true);
			request.setContentType("application/json");
			request.setCredentials("Bearer", main.token);
			request.setContentLength(oss.str().size());

			std::ostream& o = clientSession.sendRequest(request);

			newPostJSON.stringify(o);

			Poco::JSON::Object recieved;

			std::istream& s = clientSession.receiveResponse(response);

			if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
				std::cout << "Sent" << std::endl;
			} else {
				Poco::JSON::Parser parser;
				Poco::Dynamic::Var parsed = parser.parse(s);
				Poco::JSON::Object::Ptr responseJSON = parsed.extract<Poco::JSON::Object::Ptr>();

				std::cout << response.getStatus() << " " << response.getReason() << std::endl;

				if (responseJSON->has("message")) {

					std::cout << responseJSON->get("message").toString() << std::endl;
				} else {
					std::cout << "Unknown error" << std::endl;
				}
			}

		} catch (const std::invalid_argument&) {
			std::cout << "Invalid port." << std::endl;
		}
	} else {
		std::cout << "Usage: <username/email> <password> NOTE: This is not over a secure connection!" << std::endl;
	}
	return true;
}

std::vector<std::string>* PolyMostCommand::onTabCompletion(std::vector<std::string>, int position) {
	return nullptr;
};
