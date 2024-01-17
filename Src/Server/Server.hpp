#pragma once

#include "../ConfigFile/ConfigFile.hpp"
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

#define MAX_CLENTS 1024
#define PAGE 65536

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
		std::map<int, bool> gotResp;

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

		void deleteClient(int index);

		~MServer();
		MServer();
};
