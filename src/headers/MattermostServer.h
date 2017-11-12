#ifndef MATTERMOST_SERVER_H
#define MATTERMOST_SERVER_H

#include <string>

class MattermostServer {
public:
	MattermostServer(std::string address, int port, std::string uri) {
		this->address = address;
		this->port = port;
		this->URI = uri;
	};

	std::string getAddress() { return address; };
	std::string getURI() { return URI; };
	unsigned int getPort() { return port; };
private:
	std::string address, URI;
	unsigned int port;
};
#endif
