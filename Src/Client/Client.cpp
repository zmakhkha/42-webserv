#include "Client.hpp"

Client::Client() : req(), resp(), gotResponse(false) {}
Client::~Client()
{
	std::cout << "closed " << resp.getFd() << std::endl;
	close(resp.getFd());
}
