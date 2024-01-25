#pragma once

#include "../ConfigFile/myconfig.hpp"
#include "../Client/Client.hpp"

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

#define MAX_CLENTS SOMAXCONN
#define PAGE 65535

class request;
class Response;



// Modify the MServer class definition

class MServer {
private:
    const std::vector<Server> servers;
    std::vector <struct pollfd> fds;
    std::vector< Client> clients;
    sockaddr_in addrserv;
    int clientIndex;
    size_t nserv;

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
    void logevent(int code, int index, int fd);

    ~MServer();
    MServer();
};
