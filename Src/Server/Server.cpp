#include "Server.hpp"
#include <deque>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>

#define RED "\u001b[31m"
#define ORANGE "\u001b[33m"
#define GREEN "\u001b[32m"
#define RESET "\u001b[0m"
#define MAX_SEND 1024

const char *RESPONSE_MESSAGE = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 15\r\n"
                               "\r\n"
                               "Hello, Client!\n";

MServer::~MServer() { delete[] fds; }
MServer::MServer() : servers(Config::getConfig()) {
  nserv = Config::getConfig().size();
  fds = new pollfd[MAX_CLENTS];
  for (int i = 0; i < MAX_CLENTS; i++) {
    fds[i].fd = -1;
    fds[i].events = POLLIN;
  }
  clientIndex = nserv;
}

void MServer::cerror(const st_ &str) {
  std::cerr << str << std::endl;
  exit(EXIT_FAILURE);
}

void MServer::initServers() {
  if (nserv >= MAX_CLENTS)
    cerror("Unable to accept incoming clients, reduce the servers number !!");
  memset(fds, 0, sizeof(fds));
  for (int i = 0; i < nserv; i++) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
      cerror("Server: creating socket failed");

    bzero(&addrserv, sizeof(addrserv));
    addrserv.sin_family = AF_INET;
    addrserv.sin_addr.s_addr = htonl(INADDR_ANY);
    addrserv.sin_port = htons(stoi(servers[i].listen.second));

    int optval = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
        -1)
      cerror("Error: Setting SO_REUSEADDR");

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) ==
        -1)
      cerror("Error: Setting SO_REUSEPORT");

    if (bind(sock, (struct sockaddr *)&addrserv, sizeof(addrserv)) != 0)
      cerror("Error: Could not bind socket");

    if (listen(sock, 200) != 0)
      cerror("Error: Listen");

    if (fcntl(sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
      cerror("Error: Setting socket to nonblocking");

    fds[i].fd = sock;
    fds[i].events = POLLIN;
    std::cout << "Server listening on port " << ntohs(addrserv.sin_port)
              << std::endl;
  }
}

void MServer::acceptClient(int index) {
  int clientSocket;
  int s = fds[index].fd;

  std::cout << "[server]index :" << index << " FD : " << s << std::endl;
  clientSocket = accept(s, (struct sockaddr *)0, (socklen_t *)0);
  if (clientSocket == -1) {
    std::cerr << "Listening socket file descriptor: " << fds[index].fd
              << std::endl;
    return;
  }
  std::cout << GREEN << "[Client connected]" << RESET << std::endl;
  int clientIdx = getFreeClientIdx();
  if (clientIdx == -1)
    return;
  fds[clientIdx].events = POLLIN;
  fds[clientIdx].fd = clientSocket;
  reqsMap[clientIdx] = request();
  respMap[clientIdx] = Response();
  gotResp[clientIdx] = false;
}

void MServer::handleClient(int index) {
  std::cout << ORANGE << "handling client with [index] " << index << " [fd] "
            << fds[index].fd << RESET << std::endl;
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  ssize_t re = recv(fds[index].fd, buffer, sizeof(buffer) - 1, 0);
  if (re == -1) {
    std::cerr << "Error reading from client" << std::endl;
    deleteClient(index);
    return;
  }
  if (re == 0)
    return;
  reqsMap[index].feedMe(st_(buffer));
  fds[index].events = POLLOUT;
}

void MServer::routin() {
  int i;
  while (true) {
    int status = poll(fds, MAX_CLENTS, -1);
    if (status == -1) {
      perror("Error in poll");
      continue;
    }
    i = -1;
    while (++i < nserv) {
      if (fds[i].revents & POLLIN) {
        acceptClient(i);
      }
    }
    i = nserv - 1;
    while (++i < MAX_CLENTS) {
      if (fds[i].fd != -1) {
        if (fds[i].events & POLLIN) {
          handleClient(i);
        }
        if (fds[i].events & POLLOUT) {
          sendReesp(i);
        }
      }
    }
  }
}

void MServer::sendReesp(int index) {
  if (!gotResp[index])
    respMap[index].RetResponse(reqsMap[index]);
  gotResp[index] = true;

  if (!respMap[index].headersent) {
    st_ res = respMap[index].getRet();

    // write(1, res.c_str(), strlen(res.c_str()));
    if (send(fds[index].fd, res.c_str(), strlen(res.c_str()), 0) == 1)
      return (deleteClient(index), (void)0);
    respMap[index].headersent = true;
  }

  int fd = respMap[index].getFd();
  if (respMap[index].headersent && fd == -1) {
    deleteClient(index);
    return;
  }

  if (fd != -1) {
    char *buff = new char[PAGE];
    respMap[index].sentData = read(fd, buff, MAX_SEND);
    if (respMap[index].sentData == -1 || respMap[index].sentData == 0) {
      delete[] buff;
      close(fd);
      deleteClient(index);
    } else {
      // write(1, buff, respMap[index].sentData);
      if (send(fds[index].fd, buff, respMap[index].sentData, 0) == -1)
        return (delete[] buff, close(fd), deleteClient(index), (void)0);
      delete[] buff;
    }
    fds[index].events = POLLOUT;
    return;
  }

  deleteClient(index);
}

void MServer::deleteClient(int index) {
  bool keep = reqsMap[index].getConnection();
  if (reqsMap.find(index) != reqsMap.end())
    reqsMap.erase(index);
  if (respMap.find(index) != respMap.end())
    respMap.erase(index);
  if (gotResp.find(index) != gotResp.end())
    gotResp.erase(index);

  if (!keep) {
    close(fds[index].fd);
    fds[index].fd = -1;
    fds[index].events = POLLIN;
    std::cout << RED << "[Client left !!]" << RESET << std::endl;
  } else {
    reqsMap[index] = request();
    respMap[index] = Response();
    gotResp[index] = false;
  }
}

int MServer::getFreeClientIdx() {
  int i = 0;
  for (i = 0; i < MAX_CLENTS; i++) {
    if (fds[i].fd == -1)
      break;
  }
  if (i == MAX_CLENTS) {
    std::cerr << "Unable to add another client !!" << std::endl;
    i = -1;
  }
  return i;
}

void MServer::Serving() {
  this->initServers();
  this->routin();
}