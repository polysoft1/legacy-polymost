#include "headers/PolyMostCommand.h"

#include <iostream>
#include <string>
#include <stdexcept>

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

PolyMostCommand::PolyMostCommand(PolyMost& mainReference) : main(mainReference), printOptions(), parseOptions() {
	server = new MattermostServer("www.shadowxcraft.net", 2053, "/api/v4");
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
		if (this->user != nullptr) {
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
					/*std::cout << "About to do it\n";
					curlpp::Easy myRequest;
					std::cout << "Request created\n";
					//myRequest.setOpt(new curlpp::options::Url(std::string("https://" + this->server->getAddress() + ":" + std::to_string(this->server->getPort()) + this->server->getURI())));
					myRequest.setOpt(new curlpp::options::Url(std::string("https://www.google.com")));
					std::cout << "Opt\n";
					myRequest.setOpt(new curlpp::options::SslEngineDefault());
					myRequest.setOpt(new curlpp::options::SslKey());
					myRequest.setOpt(curlpp::options::Verbose(true));
					std::cout << "Opt2\n";
					myRequest.perform();
					std::cout << "Worked\n";*/
					Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrCert = new Poco::Net::AcceptCertificateHandler(false); // Obviously not the most secure..
					Poco::Net::Context* context = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
					context->disableProtocols(Poco::Net::Context::PROTO_SSLV2 | Poco::Net::Context::PROTO_SSLV3);
					Poco::Net::SSLManager::instance().initializeClient(0, ptrCert, context);

					Poco::Net::SocketAddress address(this->server->getAddress(),
						this->server->getPort());
					Poco::Net::SecureStreamSocket socket(address, context);

					if (socket.havePeerCertificate()) {
						Poco::Net::X509Certificate cert = socket.peerCertificate();
						std::cout << cert.issuerName() << "\n";
					} else {
						std::cout << "No certificate";
					}

					/*Poco::Net::HTTPSClientSession clientSession(this->server->getAddress(),
						this->server->getPort(), &context);
					Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST,
						this->server->getURI() + "/users/login", Poco::Net::HTTPRequest::HTTP_1_1);
					Poco::Net::HTTPResponse response;

					request.setKeepAlive(true);
					request.setContentType("application/json");
					request.setContentLength(loginJSON.size());

					std::ostream& o = clientSession.sendRequest(request);

					o << loginJSON;

					std::istream& s = clientSession.receiveResponse(response);

					std::ostringstream os;
					os << s.rdbuf();
					std::string responseJSON = os.str();

					if (response.has("Token")) {

						// Get the token
						std::string token = response.get("Token");

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
						handleError(responseJSON, parseOptions);
					}*/
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

		mattermost::MultiPage pageRequest;
		if (args.size() >= 1) {
			try {
				int page = std::stoi(args[0], nullptr, 0);
				pageRequest.set_page(page);
			} catch (const std::invalid_argument&) {
				std::cout << "Invalid page number." << std::endl;
			}
		}
		if (args.size() >= 2) {
			try {
				int page = std::stoi(args[1], nullptr, 0);
				pageRequest.set_per_page(page);
			} catch (const std::invalid_argument&) {
				std::cout << "Invalid page size." << std::endl;
			}
		}

		std::string pageRequestJSON;

		google::protobuf::util::Status toJSONResult
			= google::protobuf::util::MessageToJsonString(pageRequest, &pageRequestJSON, printOptions);

		if (toJSONResult.ok()) {

			Poco::Net::HTTPClientSession clientSession(this->server->getAddress(), this->server->getPort());
			Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, this->server->getURI() + "/users/me/teams", Poco::Net::HTTPRequest::HTTP_1_1);
			Poco::Net::HTTPResponse response;

			request.setContentType("application/json");
			request.setContentLength(pageRequestJSON.size());
			request.setCredentials("Bearer", this->user->token);
			std::ostream& o = clientSession.sendRequest(request);

			o << toJSONResult;

			std::istream& s = clientSession.receiveResponse(response);

			// Get the team list object
			if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
				// Get the team list object
				Poco::JSON::Parser parser;
				Poco::Dynamic::Var parsed = parser.parse(s);

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

			} else {
				std::ostringstream os;
				os << s.rdbuf();
				std::string responseJSON = os.str();
				handleError(responseJSON, parseOptions);
			}
		}

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
		Poco::Net::HTTPClientSession clientSession(this->server->getAddress(), this->server->getPort());
		// https://your-mattermost-url.com/api/v4/users/{user_id}/teams/{team_id}/channels
		std::cout << this->user << std::endl;

		std::string URL = this->server->getURI() + "/users/" + this->user->userData->id() + "/teams/" +
			this->user->team + "/channels";
		std::cout << URL << std::endl;
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET,
			URL, Poco::Net::HTTPRequest::HTTP_1_1);
		Poco::Net::HTTPResponse response;

		request.setContentType("application/json");
		request.setContentLength(0);
		request.setCredentials("Bearer", this->user->token);
		std::ostream& o = clientSession.sendRequest(request);

		Poco::JSON::Object recieved;

		std::istream& s = clientSession.receiveResponse(response);

		// Get the team list object
		Poco::JSON::Parser parser;
		Poco::Dynamic::Var parsed = parser.parse(s);

		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
			Poco::JSON::Stringifier::stringify(parsed, std::cout, 1);
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

		if (this->user == nullptr) {
			std::cout << "Not logged in." << std::endl;
			return true;
		} else if (this->user->token.size() == 0) {
			std::cout << "Invalid login token." << std::endl;
			return true;
		}

		Poco::Net::HTTPClientSession clientSession(this->server->getAddress(), this->server->getPort());
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET,
			this->server->getURI() + "/teams/name/" + args[0], Poco::Net::HTTPRequest::HTTP_1_1);
		Poco::Net::HTTPResponse response;

		request.setContentType("application/json");
		request.setContentLength(0);
		request.setCredentials("Bearer", this->user->token);
		std::ostream& o = clientSession.sendRequest(request);

		std::istream& s = clientSession.receiveResponse(response);

		std::ostringstream os;
		os << s.rdbuf();
		std::string responseJSON = os.str();

		// Get the team list object
		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
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

		Poco::Net::HTTPClientSession clientSession(this->server->getAddress(), this->server->getPort());
		// https://your-mattermost-url.com/api/v4/teams/{team_id}/channels/name/{channel_name}
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, this->server->getURI() + "/teams/" + this->user->team + "/channels/name/" + args[0], Poco::Net::HTTPRequest::HTTP_1_1);
		Poco::Net::HTTPResponse response;

		request.setContentType("application/json");
		request.setContentLength(0);
		request.setCredentials("Bearer", this->user->token);
		std::ostream& o = clientSession.sendRequest(request);

		Poco::JSON::Object recieved;

		std::istream& s = clientSession.receiveResponse(response);

		std::ostringstream os;
		os << s.rdbuf();
		std::string responseJSON = os.str();

		// Get the channel object
		if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
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
				Poco::Net::HTTPClientSession clientSession(this->server->getAddress(), this->server->getPort());
				Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, this->server->getURI() + "/posts", Poco::Net::HTTPRequest::HTTP_1_1);
				Poco::Net::HTTPResponse response;

				request.setKeepAlive(true);
				request.setContentType("application/json");
				request.setCredentials("Bearer", this->user->token);
				request.setContentLength(newPostJSON.size());

				std::ostream& o = clientSession.sendRequest(request);

				o << newPostJSON;

				Poco::JSON::Object recieved;

				std::istream& s = clientSession.receiveResponse(response);

				std::ostringstream os;
				os << s.rdbuf();
				std::string responseJSON = os.str();

				if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
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
