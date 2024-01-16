#include "Server.hpp"
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

const char* RESPONSE_MESSAGE = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 15\r\n"
                               "\r\n"
                               "Hello, Client!\n";

MServer::~MServer(){}
MServer::MServer() : servers(Config::getConfig()) {
  nserv = Config::getConfig().size();
  for(int i = 0; i < MAX_CLENTS ; i++)
  {
    fds[i].fd = -1;
    fds[i].events = POLLIN;
  }
  clientIndex = nserv;
}

		
void MServer::cerror(const st_ &str)
{
  std::cerr << str << std::endl;
  exit(EXIT_FAILURE);
}


void MServer::initServers()
{
    if (nserv >= MAX_CLENTS)
        cerror("Unable to accept incoming clients, reduce the servers number !!");
    memset(fds, 0, sizeof(fds));
    for (int i = 0; i < nserv; i++)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            cerror("Server: creating socket failed");

        bzero(&addrserv, sizeof(addrserv));
        addrserv.sin_family = AF_INET;
        addrserv.sin_addr.s_addr = htonl(INADDR_ANY);
        addrserv.sin_port = htons(stoi(servers[i].listen.second));

        int optval = 1;

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
            cerror("Error: Setting SO_REUSEADDR");

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
            cerror("Error: Setting SO_REUSEPORT");

        if (bind(sock, (struct sockaddr *)&addrserv, sizeof(addrserv)) != 0)
            cerror("Error: Could not bind socket");

        if (listen(sock, 200) != 0)
            cerror("Error: Listen");

        if (fcntl(sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
            cerror("Error: Setting socket to nonblocking");
        
        fds[i].fd = sock;
        fds[i].events = POLLIN;
        std::cout << "Server listening on port " << ntohs(addrserv.sin_port) << std::endl;
    }
}

void MServer::acceptClient(int index)
{
  int clientSocket;
  int s = fds[index].fd;

  std::cout << "index :" << index << " FD : " << s << std::endl;
  clientSocket = accept(s, (struct sockaddr *) 0,  (socklen_t*) 0);
    std::cerr << "Listening socket file descriptor: " << fds[index].fd << std::endl;
    if (clientSocket == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        } else {
         freopen("out", "w", stdout);
            perror("Error accepting connection");
             std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        std::cerr << "Index: " << index << ", fd: " << fds[index].fd << std::endl;
        return;
        }
    }
    std::cout << "\u001b[32m" << "[Client connected]" << "\u001b[0m" << std::endl;
    fds[nserv].events = POLLIN;
    fds[nserv].fd= clientSocket;
}



void MServer::handleClient(int index)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t re = recv(fds[index].fd, buffer, sizeof(buffer) - 1, 0);
    if (re == -1) {
        perror("Error reading from client");
        //return;
        exit(EXIT_FAILURE);
    }
    fds[index].events = POLLOUT;
    send(fds[index].fd, RESPONSE_MESSAGE, strlen(RESPONSE_MESSAGE), 0);
    close(fds[index].fd);
    std::cout << "\u001b[31m" << "[Client disConnected]" << "\u001b[0m" << std::endl;
    fds[index].events = POLLNVAL;
    //std::cout << "Received from client: " << buffer << std::endl;
}

int MServer::getClientIndex(int fd)
{
  for (int i = 0; i < MAX_CLENTS ; i++)
  {
    if (fds[i].fd == fd)
      return i;
  }
  return -1;
}

void MServer::routin()
{
  while(true)
  {
    int status = poll(fds, MAX_CLENTS, -1);
      if (status == -1) {
        perror("Error in poll");
        break;
      }
    for (int i=0; i < MAX_CLENTS; i++)
    {
      if (fds[i].revents & POLLIN)
      {
        if (i < nserv) 
          acceptClient(i);
      } 
    }
    for (int i=nserv; i < MAX_CLENTS; i++)
    {
      if (fds[i].events & POLLIN)
      {
          handleClient(i);
      }
      else if (fds[i].events & POLLOUT)
      {
        sendReesp(i);
      }
    }
  }
}

void MServer:: sendReesp(int index)
{

}

void MServer::Serving()
{
  this->initServers();
  this->routin();
}