#pragma once

#include "../ConfigFile/myconfig.hpp"
#include "../Response/Response.hpp"

#include <deque>
#include <arpa/inet.h>
#include <map>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/types.h>
#include <netdb.h>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <vector>
#include <ostream>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fstream>

#define GREEN "\u001b[32m"
#define RESET "\u001b[0m"

#define MAX_CLENTS 1024
#define PAGE 1024

class request;
class Response;

class MServer
{
	private:
		const std::vector<Server> servers;
		struct pollfd *fds;
		sockaddr_in addrserv;
		int clientIndex;
		size_t nserv;
		std::map<int, request> reqsMap;
		std::map<int, Response> respMap;

	public:
		void handleClient(int clientFd);
		void acceptClient(int index);
		void cerror(const st_ &str);
		void initServers();
		void Serving();
		void routin();
		void sendReesp(int index);
		int getClientIndex(int fd);
		int getFreeClientIdx();

		~MServer();
		MServer();
};
