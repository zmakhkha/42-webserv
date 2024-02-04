#pragma once

#include<iostream>
#include <string>
//#include <sys/_types/_size_t.h>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define OpenBrace '{'
#define CloseBrace '}'
#define vect_ std::vector < Server >
#define deque_ std::deque < std::string >

// #define uploadPath "/goinfre/zmakhkha/upload/"
#define cgiResStr "/tmp/r"
#define cgiBodyStr "/tmp/f"

struct Methods
{
    bool Get;
    bool Post;
    bool Delete;
    bool empty();
    Methods();
    ~Methods();
};

struct Shared
{
    std::string root;
    std::string up_path;
    Methods allow;
    std::vector<std::string> index;
    std::map<int, std::string> error_page;
    std::pair<size_t, char> body_size;
    std::pair<int, std::string> redirect;
};

struct Location : public Shared
{
    Location() {
        autoindex = false;
    }
    std::string prefix;
    bool autoindex;
    std::pair<std::string, std::string> cgi;
};

struct Server : public Shared
{
    Server() {
        body_size.first = 2;
        body_size.second = 'G';
    };
    std::vector<Location> location;
    std::pair<std::string, std::string> listen;
    std::string server_name;
    ~Server() {};
};

class Config
{
    private:
        static std::vector<Server> server;
    public:
        static const std::vector<Server> &getConfig();
        static Server getservconf(std::string server_name, std::string host);
        static void	LooponServers( std::string path );

};
Server	parsing_conf(std::string data);
// void	missingpoint( std::string	parse );
void	printfVec( vect_ conf );
int     ft_stoi( std::string var );
#include "myconfig.tpp"