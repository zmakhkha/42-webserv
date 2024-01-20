#include "Server.hpp"
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>

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
  memset(fds, 0, sizeof(pollfd));
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
  std::cerr << "Listening socket file descriptor: " << fds[index].fd
            << std::endl;
  if (clientSocket == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return;
    } else {
      freopen("out", "w", stdout);
      perror("Error accepting connection");
      std::cerr << "Error accepting connection: " << strerror(errno)
                << std::endl;
      std::cerr << "Index: " << index << ", fd: " << fds[index].fd << std::endl;
      return;
    }
  }
  std::cout << "[Client connected]" << std::endl;
  int clientIdx = getFreeClientIdx();
  if (clientIdx == -1)
    return;
  fds[clientIdx].events = POLLIN;
  fds[clientIdx].fd = clientSocket;
  reqsMap[clientIdx] = request();
  respMap[clientIdx] = Response();
}

void MServer::handleClient(int index) {
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  ssize_t re = recv(fds[index].fd, buffer, sizeof(buffer) - 1, 0);
  if (re == -1) {
    perror("Error reading from client");
    return;
    // exit(EXIT_FAILURE);
  }
  reqsMap[index].feedMe(st_(buffer));
  fds[index].events = POLLOUT;
  // std::cout << "Received from client: " << buffer << std::endl;
}

void MServer::routin() {
  while (true) {
    int status = poll(fds, MAX_CLENTS, -1);
    if (status == -1) {
      perror("Error in poll");
      break;
    }
    for (int i = 0; i < MAX_CLENTS; i++) {
      if (fds[i].revents & POLLIN) {
        if (i < nserv)
          acceptClient(i);
      }
    }
    for (int i = nserv; (i < MAX_CLENTS) && (fds[i].fd != -1); i++) {

      if (fds[i].events & POLLIN) {
        handleClient(i);
      }
      if (fds[i].events & POLLOUT) {
        sendReesp(i);
      }
    }
  }
}

void MServer::sendReesp(int index) {
  std::cout << "---------> index : " << index << std::endl;
  respMap[index].RetResponse(reqsMap[index]);
  st_ res = respMap[index].getRet();

  if (!respMap[index].headersent) {
     send(fds[index].fd, res.c_str(), strlen(res.c_str()), 0);
     write(1, res.c_str(), strlen(res.c_str()));
    respMap[index].headersent = true;
  }

  int fd = respMap[index].getFd();
  if (fd != -1) {
    char *buff = new char[PAGE];
    respMap[index].sentData = read(fd, buff, PAGE);
    if (respMap[index].sentData == -1) {
      delete [] buff;
      close(fds[index].fd);
      fds[index].fd = -1;
      fds[index].events = POLLIN;
      if (reqsMap.find(index) != reqsMap.end())
        reqsMap.erase(index);
    } else {
      // Only send data if read was successful
      send(fds[index].fd, buff, respMap[index].sentData, 0);
      delete [] buff;
      return;
    }
  } else {
    close(fds[index].fd);
    fds[index].fd = -1;
    fds[index].events = POLLIN;
    if (reqsMap.find(index) != reqsMap.end())
      reqsMap.erase(index);
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