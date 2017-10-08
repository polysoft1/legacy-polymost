#include "headers/ConnectCommand.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/JSON/Object.h"

bool ConnectCommand::onCommand(std::vector<std::string> args) {
	if (args.size() >= 2 && args.size() <= 3) {

		std::string address = args[0];
		try {
			int port = std::stoi(args[1], nullptr, 0);
			std::string URI = args.size() == 2 || args[2].length() == 0 ? "/" : args[2];

			std::cout << "Attempting to connect to " << address << " On port " << port << std::endl << " with URI " << URI;

			Poco::Net::HTTPClientSession clientSession(address, port);
			Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, URI, Poco::Net::HTTPRequest::HTTP_1_1);
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
		} catch (const std::invalid_argument&) {
			std::cout << "Invalid port." << std::endl;
		}
	} else {
		std::cout << "Usage: <URL> <PORT> [URI]" << std::endl;
		std::cout << "given args with size " << args.size() << ":";
		for (auto i = args.begin(); i != args.end(); ++i)
			std::cout << *i << ' ';
	}
	return true;
};

std::vector<std::string>* ConnectCommand::onTabCompletion(std::vector<std::string>, int position) {
	return nullptr;
};
