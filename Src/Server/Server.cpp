#include "Server.hpp"
#include <deque>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

#define DISC -1
#define CONN -2
#define HAND -3

MServer::~MServer()
{
}

MServer::MServer() : servers(Config::getConfig())
{
  nserv = Config::getConfig().size();
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
  for (int i = 0; i < (int)nserv; i++)
  {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
      cerror("Server: creating socket failed");

    bzero(&addrserv, sizeof(addrserv));
    addrserv.sin_family = AF_INET;
    addrserv.sin_addr.s_addr = htonl(INADDR_ANY);
    addrserv.sin_port = htons(atoi(servers[i].listen.second.c_str()));
    std::cout << "Server Listening on port : " << servers[i].listen.second << std::endl;

    int optval = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
        -1)
      cerror("Error: Setting SO_REUSEADDR");

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) ==
        -1)
      cerror("Error: Setting SO_REUSEPORT");

    if (bind(sock, (struct sockaddr *)&addrserv, sizeof(addrserv)) != 0)
      cerror("Error: Could not bind socket");

    if (listen(sock, SOMAXCONN) != 0)
      cerror("Error: Listen");

    if (fcntl(sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
      cerror("Error: Setting socket to nonblocking");

    struct pollfd tmp;
    tmp.fd = sock;
    tmp.events = POLLIN | POLLOUT;
    fds.push_back(tmp);
    std::cout << GREEN << "Server ADDED" << RESET << std::endl;
    std::cout << GREEN << "fds.size()=" << fds.size() << RESET << std::endl;
  }
}

void MServer::acceptClient(int index)
{
  std::cout << BLUE << "MServer::acceptClient" << RESET << std::endl;
  int clientSocket;
  int s = fds[index].fd;

  clientSocket = accept(s, (struct sockaddr *)0, (socklen_t *)0);
  if (clientSocket > 0)
  {
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
      return (close(clientSocket), std::cout << "failed to set socket on nonblocking mod" << std::endl, (void)0);
    struct pollfd tmp;
    tmp.fd = clientSocket;
    tmp.events = POLLIN;
    fds.push_back(tmp);
    logevent(CONN, fds.size() - 1, clientSocket);
    Client cl;
    clients.push_back(cl);
    std::cout << "addena client" << std::endl;
  }
  else
    return (std::cout << RED << "ACCEPT : Failed to accept client !! " << RESET << std::endl, (void)0);
}

void MServer::handleClient(int index)
{
  std::cout << BLUE << "MServer::handleClient" << RESET << std::endl;
  char buffer[PAGE];
  memset(buffer, 0, sizeof(buffer));
  ssize_t re = recv(fds[index].fd, buffer, sizeof(buffer) - 1, 0);
  if (re == 0)
    return (deleteClient(index), (void)0);
  if (re == -1)
    return;
  clients[index - nserv].req.feedMe(st_(buffer, buffer + re));
  if (!clients[index - nserv].req.reading && clients[index - nserv].req.upDone)
    fds[index].events = POLLOUT;
  logevent(HAND, index, fds[index].fd);
}

void MServer::routin()
{
  while (true)
  {
    int status = poll(fds.data(), fds.size(), -1);
    if (status == -1)
    {
      std::cerr << "Error in poll" << std::endl;
      continue;
    }
    for (int i = 0; i < (int)fds.size(); i++)
    {
      if (i < (int)nserv)
        acceptClient(i);
      else
      {
        if (fds[i].revents & POLLIN)
          handleClient(i);
        else if (fds[i].events & POLLOUT)
          sendReesp(i);
        else if (fds[i].revents & POLLHUP)
          deleteClient(i);
      }
    }
  }
}

void MServer::sendReesp(int index)
{
  std::cout << "index=" << index << std::endl;
  std::cout << BLUE << "MServer::sendReesp" << RESET << std::endl;

  if (!clients[index - nserv].gotResponse)
  {
    clients[index - nserv].resp.RetResponse(clients[index - nserv].req);
    clients[index - nserv].gotResponse = true;
  }

  st_ res = clients[index - nserv].resp.getRet();

  if (!clients[index - nserv].resp.headersent)
  {
    if (send(fds[index].fd, res.c_str(), strlen(res.c_str()), 0) == -1)
    {
      deleteClient(index);
      return;
    }
    clients[index - nserv].resp.headersent = true;
  }

  int fd = clients[index - nserv].resp.getFd();
  if (clients[index - nserv].resp.headersent && fd == -1)
  {
    deleteClient(index);
    return;
  }

  if (fd != -1)
  {
    size_t MAXSEND = 1024;
    char *buff = new char[MAXSEND];
    clients[index - nserv].resp.sentData = read(fd, buff, MAXSEND);
    if (clients[index - nserv].resp.sentData == -1 || clients[index - nserv].resp.sentData == 0)
    {
      delete[] buff;
      close(fd);
      deleteClient(index);
      return;
    }
    else
    {
      if (send(fds[index].fd, buff, clients[index - nserv].resp.sentData, 0) == -1)
        deleteClient(index);
      delete[] buff;
      return;
    }
    fds[index].events = POLLOUT;
    return;
  }
  deleteClient(index);
}

void MServer::deleteClient(int index)
{
  close(fds[index].fd);
  clients.erase(clients.begin() + index - nserv);
  fds.erase(fds.begin() + index);

  std::cout << "a client disconnected" << std::endl;
}

int MServer::getFreeClientIdx()
{
  int i = 0;
  for (i = 0; i < MAX_CLENTS; i++)
  {
    if (fds[i].fd == -1)
      break;
  }
  if (i == MAX_CLENTS)
  {
    i = -1;
  }
  return i;
}

void MServer::Serving()
{
  this->initServers();
  this->routin();
}

void MServer::logevent(int code, int index, int fd)
{
  switch (code)
  {
  case DISC:
    std::cout << RED << "CLIENT Disconnected ";
    break;

  case CONN:
    std::cout << GREEN << "CLIENT Connected ";
    break;

  case HAND:
    std::cout << BLUE << "CLIENT Handled ";
    break;

  default:
    break;
  }

  std::cout << "index=" << index << " fd=" << fd << RESET << std::endl;
}