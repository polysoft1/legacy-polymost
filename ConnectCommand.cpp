#include "ConnectCommand.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/JSON/Object.h"

bool ConnectCommand::onCommand(std::vector<std::string> args) {
	if (args.size() == 2) {
		std::cout << "Attempting to connect to " << args[0] << " On port " << args[1] << std::endl;

		try {
			int port = std::stoi(args[1], nullptr, 0);
			std::cout << "Parsed port: " << port << std::endl;

			Poco::Net::HTTPClientSession clientSession(args[0], port);
			Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, "/", Poco::Net::HTTPRequest::HTTP_1_1);
			Poco::Net::HTTPResponse response;

			Poco::JSON::Object obj;

			request.setKeepAlive(true);
			request.setContentLength(0);
			request.setContentType("application/json");

			std::ostream& o = clientSession.sendRequest(request);

			std::cout << response.getStatus() << " " << response.getReason() << std::endl;

			std::istream& s = clientSession.receiveResponse(response);

			std::string stringresponse;
			while (std::getline(s, stringresponse)) {
				std::cout << stringresponse << std::endl;
			}
		} catch (const std::invalid_argument& e) {
			std::cout << "Invalid port." << std::endl;
		}
	} else {
		std::cout << "Usage: <URL> <PORT>" << std::endl;
	}
	return true;
};

std::vector<std::string>* ConnectCommand::onTabCompletion(std::vector<std::string>, int position) {
	return nullptr;
};
