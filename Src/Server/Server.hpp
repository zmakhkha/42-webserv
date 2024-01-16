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

#define MAX_CLENTS 1024
#define PAGE 1024

class request;
class Response;

class MServer
{
	private:
		const std::vector<Server> servers;
		struct pollfd fds[MAX_CLENTS];
		sockaddr_in addrserv;
		int clientIndex;
		size_t nserv;
		std::map<int, request> reqsMap;
		std::map<int, Response> respMap;

	public:
		void handleClient(int clientFd);
		void acceptClient(int index);
		void cerror(const st_ &str);
		int getClientIndex(int fd);
		void initServers();
		void Serving();
		void routin();
		void sendReesp(int index);
		~MServer();
		MServer();
};
