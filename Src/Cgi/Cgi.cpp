#include "Cgi.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

Cgi::~Cgi()
{
  if (!cgiDone)
    std::cerr << "CGI Terminated Before Doing The Job !!" << std::endl;
}
Cgi::Cgi()
{
  if (!cgiDone)
    std::cerr << "CGI Started !!" << std::endl;
}

Cgi::Cgi(st_ uri, st_ methode, int loc, st_ cgiRes, std::map<st_, st_> heads, st_ upPath, Server _srv) : _uri(uri), _methode(methode), _location(loc), upload_path(upPath), _reqHeaders(heads), _respPath(cgiRes), srv(_srv)
{
  forked = false;
  cgiDone = false;
  _CgiScriptPath = srv.location[_location].cgi.second;
  _isPost = _methode == "POST";
}

void Cgi::formatKey(std::string &key)
{
  std::string tmp = "HTTP_";
  for (size_t i = 0; i < key.length(); i++)
  {
    if (key[i] == '-')
      key[i] = '_';
    key[i] = std::toupper(key[i]);
  }
  key = tmp + key;
}

void Cgi::formatHeaders()
{
  std::string tmpKey;
  for (std::map<std::string, std::string>::iterator it = _reqHeaders.begin();
       it != _reqHeaders.end(); it++)
  {
    tmpKey = it->first;
    formatKey(tmpKey);
    _envLst.push_back(tmpKey + "=" + it->second + "");
  }
}

void Cgi::setExtraEnv()
{
  formatHeaders();
}

std::pair<st_, st_> Cgi::getPathQuery(st_ uri)
{
  std::pair<st_, st_> res;
  size_t pos = uri.find("?");
  if (pos != st_::npos)
  {
    res.first = uri.substr(0, pos);
    res.second = uri.substr(pos + 1, uri.length());
  }
  else
  {
    res.first = uri;
    res.second = "";
  }
  return res;
}

void Cgi::setEnv()
{

  st_ SERVER_SOFTWARE = "SA3DYA/V1.0";
  st_ SERVER_NAME = "SA3DYA";
  st_ GATEWAY_INTERFACE = "SA3SYA_CGI/1.1";

  std::pair<st_, st_> tmp = getPathQuery(_uri);
  st_ root = srv.location[_location].root;
  int pref_len = srv.location[_location].prefix.length();
  _envLst.push_back("SERVER_SOFTWARE=" + SERVER_SOFTWARE + "");
  _envLst.push_back("GATEWAY_INTERFACE=" + GATEWAY_INTERFACE + "");
  _envLst.push_back("SERVER_NAME=" + SERVER_NAME + "");
  _envLst.push_back("SERVER_PROTOCOL=HTTP/1.1");
  _envLst.push_back("SERVER_PORT=" + srv.listen.second + "");
  _envLst.push_back("REQUEST_METHOD=" + _methode + "");
  _envLst.push_back("PATH_INFO=" + srv.location[_location].prefix + tmp.first.substr(pref_len) + "");
  _envLst.push_back("SCRIPT_NAME=" + srv.location[_location].prefix + tmp.first.substr(pref_len) + "");
  _envLst.push_back("PATH_TRANSLATED=" + root + "/" + tmp.first.substr(pref_len) + "");
  _scriptPath = root + "/" + tmp.first.substr(pref_len);
  _envLst.push_back("QUERY_STRING=" + tmp.second + "");
  if (access((root + "/" + tmp.first.substr(pref_len) + "").c_str(), F_OK) == -1)
    throw 404;
  _envLst.push_back("UPLOAD_DIRECTORY=" + st_(upload_path));
  _envLst.push_back("REDIRECT_STATUS=200");
  this->setUnique();
}

void Cgi::setUnique()
{
  _envLst.push_back("CONTENT_LENGTH=" + _reqHeaders["content-length"]);
  _envLst.push_back("CONTENT_TYPE=" + _reqHeaders["content-type"]);
  _reqHeaders.erase(_reqHeaders.find("content-length"));
  _reqHeaders.erase(_reqHeaders.find("content-type"));
}

void Cgi::excecCgi(std::string bodyPath)
{
  this->_postBody = bodyPath;
  _isPost = bodyPath.length() != 0;
  setEnv();
  setExtraEnv();
  execute();
}

void Cgi::execute()
{
    _CgiScriptPath = srv.location[_location].cgi.second;
  if (!_scriptPath.length())
    throw(501);
  if (!forked)
  {
    forked = true;
    status = true;
    pid = fork();
    if (pid == 0)
    {
      char *envp[_envLst.size() + 1];
      for (std::size_t i = 0; i < _envLst.size(); ++i)
      {
        envp[i] = const_cast<char *>(_envLst[i].c_str());
      }
      envp[_envLst.size()] = NULL;
      if (access(_CgiScriptPath.c_str(), F_OK) != 0)
      {
        perror("access : ");
        throw 502;
      }
      char *argv[] = {const_cast<char *>(_CgiScriptPath.c_str()),
                      const_cast<char *>(_scriptPath.c_str()), NULL};
      int fd = open(_respPath.c_str(), O_CREAT | O_RDWR, 0644);
      if (fd < 0)
      {
        status = false;
        perror("open : ");
      }
      FILE *out = freopen(_respPath.c_str(), "w", stdout);
      if (_isPost)
      {
        FILE *in = freopen(_postBody.c_str(), "r", stdin);
        if (in == nullptr)
        {
          perror("freopen : ");
          status = false;
        }
      }
      if (out == nullptr)
      {
        status = false;
        perror("freopen : ");
      }
      alarm(10);
      if (execve(argv[0], argv, envp) == -1)
      {
        perror("execve");
        throw 502;
      }
    }
    else if (pid == -1)
    {
      std::cerr << "Cgi::execute() : Fork Error" << std::endl;
    }
  }

  if (pid > 0)
  {
    int stat;
    int res = waitpid(pid, &stat, WNOHANG);
    if (res == 0)
    {
      std::cout << "-------------------|mazaaaaaaal" << std::endl;
      return;
    }
    if (res < 0)
    {
      std::cout << "Internal Server Error" << std::endl;
      throw(500);
      exit(0);
    }
    else if (res > 0)
    {
      std::cout << "-------------------|CGI Done" << std::endl;
      if (WIFSIGNALED(stat) && WTERMSIG(stat) == SIGALRM)
        throw 504;
      else if ((WIFEXITED(stat) && WEXITSTATUS(stat)) != 0 || WIFSIGNALED(stat) || !status)
        throw 502;
      cgiDone = true;
    }
  }
  else
  {
    perror("fork");
  }
}

Cgi::Cgi(const Cgi &tmp)
{
  this->_uri = tmp.getUri();
  this->_methode = tmp.getMethode();
  this->_location = tmp.getLocation();
  this->upload_path = tmp.getUploadPath();
  this->_reqHeaders = tmp.getReqHeaders();
  this->_respPath = tmp.getRespPath();
  this->srv = tmp.getServer();
  _CgiScriptPath = srv.location[_location].cgi.second;
}

Cgi &Cgi::operator=(const Cgi &tmp)
{
  if (this != &tmp)
  {
    this->_uri = tmp.getUri();
    this->_methode = tmp.getMethode();
    this->_location = tmp.getLocation();
    this->upload_path = tmp.getUploadPath();
    this->_reqHeaders = tmp.getReqHeaders();
    this->_respPath = tmp.getRespPath();
    this->srv = tmp.getServer();
    // std::cout << "----->|" << srv.location[].cgi.first << std::endl;
  }
  return (*this);
}

void Cgi::setUri(const st_ &uri)
{
  _uri = uri;
}

st_ Cgi::getUri() const
{
  return _uri;
}

void Cgi::setMethode(const st_ &methode)
{
  _methode = methode;
}

st_ Cgi::getMethode() const
{
  return _methode;
}

void Cgi::setLocation(int location)
{
  _location = location;
}

int Cgi::getLocation() const
{
  return _location;
}

void Cgi::setUploadPath(const st_ &uploadPath)
{
  upload_path = uploadPath;
}

st_ Cgi::getUploadPath() const
{
  return upload_path;
}

void Cgi::setReqHeaders(const std::map<st_, st_> &reqHeaders)
{
  _reqHeaders = reqHeaders;
}

std::map<st_, st_> Cgi::getReqHeaders() const
{
  return _reqHeaders;
}

void Cgi::setRespPath(const st_ &respPath)
{
  _respPath = respPath;
}

st_ Cgi::getRespPath() const
{
  return _respPath;
}

void Cgi::setServer(const Server &server)
{
  srv = server;
}

Server Cgi::getServer() const
{
  return srv;
}