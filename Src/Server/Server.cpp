#include "Server.hpp"
#include <cstdlib>
#include <deque>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_SEND 1024

MServer::~MServer() { delete[] fds; }
MServer::MServer() : servers(Config::getConfig())
{
  nserv = Config::getConfig().size();
  fds = new pollfd[MAX_CLIENTS];
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    fds[i].fd = -1;
    fds[i].events = POLLIN;
  }
}

void MServer::cerror(const st_ &str)
{
  std::cerr << str << std::endl;
  exit(EXIT_FAILURE);
}

void MServer::initServers()
{
  if (nserv >= MAX_CLIENTS)
    cerror("Unable to accept incoming clients, reduce the servers number !!");
  memset(fds, 0, sizeof(pollfd));
  for (int i = 0; i < (int)nserv; i++)
  {
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

    if (listen(sock, MAX_CLIENTS) != 0)
      cerror("Error: Listen");

    if (fcntl(sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
      cerror("Error: Setting socket to nonblocking");

    fds[i].fd = sock;
    fds[i].events = POLLIN;
    std::cout << "Server listening on port " << ntohs(addrserv.sin_port)
              << std::endl;
  }
}

void MServer::acceptClient(int index)
{
  int clientSocket;
  int s = fds[index].fd;

  clientSocket = accept(s, (struct sockaddr *)0, (socklen_t *)0);
  if (clientSocket <= 0)
    return;
  if (fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
    return;

  int clientIdx = getFreeClientIdx();
  if (clientIdx == -1)
    return;
  fds[clientIdx].events = POLLIN;
  fds[clientIdx].fd = clientSocket;
  mapClients[index] = Client();
}

void MServer::handleClient(int index)
{
  char buffer[PAGE];
  memset(buffer, 0, sizeof(buffer));
  ssize_t re = recv(fds[index].fd, buffer, PAGE, 0);
  if (re <= -1)
  {
    std::cerr << "Error reading from client" << std::endl;
    deleteClient(index);
    return;
  }
  Client &refClient = mapClients[index];
  refClient.req.feedMe(st_(buffer, re));

  if (refClient.req.cgi && !refClient.req.tmp.cgiDone)
    refClient.req.reCheckCgi();
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
  if (!refClient.req.reading && refClient.req.upDone)
    fds[index].events = POLLOUT;
}

void MServer::routin()
{
  int i;
  while (true)
  {
    int status = poll(fds, MAX_CLIENTS, -1);
    std::cout << __FILE__ << " " << __LINE__ << std::endl;
    if (status == -1)
    {
      std::cerr << "Error in poll" << std::endl;
      exit(EXIT_FAILURE);
    }
    i = -1;
    while (++i < MAX_CLIENTS)
    {
      std::cout << __FILE__ << " i = " << i << std::endl;
      if (fds[i].revents & POLLIN)
      {
        if (i < (int)nserv)
          acceptClient(i);
        else
          handleClient(i);
      }
      if (fds[i].revents & POLLOUT)
      {
        sendReesp(i);
      }
      if (fds[i].revents & POLLHUP)
      {
        deleteClient(i);
      }
    }
  }
}

void MServer::sendReesp(int index)
{
  Client &refClient = mapClients[index];

  if (!refClient.gotResp)
    refClient.resp.RetResponse(refClient.req);
  refClient.gotResp = true;

  if (!refClient.resp.headersent)
  {
    st_ res = refClient.resp.getRet();

    ssize_t ret = send(fds[index].fd, res.c_str(), strlen(res.c_str()), 0);
    if (ret == -1)
      return (deleteClient(index), (void)0);
    if (ret == 0)
      return;
    refClient.resp.headersent = true;
  }

  int fd = refClient.resp.getFd();
  if (refClient.resp.headersent && fd == -1)
  {
    deleteClient(index);
    return;
  }

  if (fd != -1)
  {
    char *buff = new char[MAX_SEND];
    refClient.resp.sentData = read(fd, buff, MAX_SEND);
    if (refClient.resp.sentData == -1 || refClient.resp.sentData == 0)
    {
      delete[] buff;
      close(fd);
      deleteClient(index);
    }
    else
    {
      ssize_t ret = send(fds[index].fd, buff, refClient.resp.sentData, 0);
      if (ret == -1)
        return;
      else if (ret == 0)
        return;
      delete[] buff;
    }
    fds[index].events = POLLOUT;
    return;
  }

  deleteClient(index);
}

void MServer::deleteClient(int index)
{
  bool keep = mapClients[index].req.getConnection();
  if (mapClients.find(index) != mapClients.end())
    mapClients.erase(index);

  if (!keep)
  {
    close(fds[index].fd);
    fds[index].fd = -1;
    fds[index].events = 0;
  }
  else
    return;
  mapClients[index] = Client();
}

int MServer::getFreeClientIdx()
{
  int i = 0;
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    if (fds[i].fd == -1)
      break;
  }
  if (i == MAX_CLIENTS)
  {
    std::cerr << "Unable to add another client !!" << std::endl;
    i = -1;
  }
  return i;
}

void MServer::Serving()
{
  this->initServers();
  this->routin();
}