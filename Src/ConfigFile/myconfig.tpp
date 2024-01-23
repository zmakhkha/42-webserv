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

template < typename T >
void	missingpoint( T	parse ) {
	size_t bpos = parse.find(";");
	size_t rpos = parse.rfind(";");
	if (bpos != rpos)
		throw std::runtime_error("Error: multiple ; at the end");
}

template < typename T >
void    Listen( deque_ &conf, T &fill ) {
    if (conf[0][conf[0].length() - 1] == ';')
        missingpoint( conf[0] );
    else
        throw std::runtime_error("Error: Missing ; at the end");
    size_t pos = conf[0].find(":");
    if (pos == std::string::npos) {
        fill.listen.first = conf[0].substr(0, conf[0].find(";"));
        if (ft_stoi(fill.listen.first) > 65535 || ft_stoi(fill.listen.first) < 1024)
            throw std::runtime_error("Error: Port");
    }
    else {
        fill.listen.first = conf[0].substr(0, pos);
        fill.listen.second = conf[0].substr(pos + 1, std::string(&conf[0][pos + 1]).length() - 1);
        if (ft_stoi(fill.listen.second) > 65535 || ft_stoi(fill.listen.second) < 1024)
            throw std::runtime_error("Error: Port");
    }
    conf.pop_front();
}

template < typename T >
void	Root( deque_ &conf, T &fill ) {
	if (conf[0][conf[0].length() - 1] == ';')
		missingpoint( conf[0] );
	else
		throw std::runtime_error("Error: Missing ; at the end");
	fill.root = conf[0].substr(0, conf[0].length() - 1);
	conf.pop_front();
}

template < typename T >
void	Allow( deque_ &conf, T &fill ) {
	int	count;
	for (count = 0; conf[0][conf[0].length() - 1] != ';' && count < 3; count++) {
		(conf[0] == "POST") && (fill.allow.Post = true);
		(conf[0] == "DELETE") && (fill.allow.Delete = true);
		(conf[0] == "GET") && (fill.allow.Get = true);
		if (conf[0] != "GET" && conf[0] != "POST" && conf[0] != "DELETE")
			throw std::runtime_error("Error: Unknown HTTP method " + conf[0].substr(0, conf[0].find(";")));
		conf.pop_front();
	}
	if (conf[0][conf[0].length() - 1] == ';')
		missingpoint( conf[0] );
	else
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].length() - 1);
	if (conf[0] != "GET" && conf[0] != "POST" && conf[0] != "DELETE")
		throw std::runtime_error("Error: Unknown HTTP method " + conf[0]);
	(conf[0] == "POST") && (fill.allow.Post = true);
	(conf[0] == "DELETE") && (fill.allow.Delete = true);
	(conf[0] == "GET") && (fill.allow.Get = true);
	if (count > 2)
		std::runtime_error("Error: too much args");
	conf.pop_front();
}

template < typename T >
void	errorPage( deque_ &conf, T &fill ) {
	if (conf[1][conf[1].length() - 1] == ';')
		missingpoint( conf[1] );
	else
		throw std::runtime_error("Error: Missing ; at the end");
	conf[1] = conf[1].substr(0, conf[1].length() - 1);
	fill.error_page[ft_stoi(conf[0])] = conf[1];
	conf.pop_front();
	conf.pop_front();
}

template < typename T >
void	maxbodySize( deque_ &conf, T &fill ) {
	if (conf[0][conf[0].length() - 1] == ';')
		missingpoint( conf[0] );
	else
		throw std::runtime_error("Error: Missing ; at the end");
	if (conf[0].length() < 3)
		throw std::runtime_error("Error: Max Body Size bad Syntax");
	fill.body_size.first = ft_stoi(conf[0].substr(0, conf[0].length() - 2));
	fill.body_size.second = conf[0][conf[0].length() - 2];
	if (fill.body_size.second != 'G' && fill.body_size.second != 'B' && fill.body_size.second != 'M')
		throw std::runtime_error("Error: Invalid Char");
	conf.pop_front();
}

template < typename T >
void	uploadPath( deque_ &conf, T &fill ) {
	if (conf[0][conf[0].length() - 1] == ';')
		missingpoint( conf[0] );
	else
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].length() - 1);
	fill.up_path = conf[0];
	if (access(fill.up_path.c_str(), F_OK) == -1)
		throw std::runtime_error("Error: UpPath Not Found");
	conf.pop_front();
}

template < typename T >
void	autoindex( deque_ &conf, T &fill ) {
	if (conf[0][conf[0].length() - 1] == ';')
		missingpoint( conf[0] );
	else
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].length() - 1);
	if (conf[0] != "ON" && conf[0] != "OFF" && conf[0] != "on" && conf[0] != "off")
		throw std::runtime_error("Error: Bad keyword Autoindex " + conf[0]);
	(conf[0] == "ON" || conf[0] == "on") && (fill.autoindex = true, 0);
	(conf[0] == "OFF" || conf[0] == "off") && (fill.autoindex = false, 0);
	conf.pop_front();
}

template < typename T >
void	serverName( deque_ &conf, T &fill ) {
	if (conf[0][conf[0].length() - 1] == ';')
		missingpoint( conf[0] );
	else
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].length() - 1);
	fill.server_name = conf[0];
	conf.pop_front();
}

template < typename T >
void	cgi( deque_ &conf, T &fill ) {
	fill.cgi.first = conf[0];
	conf.pop_front();
	if (conf[0][conf[0].length() - 1] == ';')
		missingpoint( conf[0] );
	else
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].find(";"));
	fill.cgi.second = conf[0];
	conf.pop_front();
}

template < typename T >
void	indexfiles( deque_ &conf, T &fill ) {
	for (int i = 0; i < 3 && conf[0][conf[0].length() - 1] != ';'; i++) {
		fill.index.push_back(conf[0]);
		conf.pop_front();
		if (conf[0][conf[0].length() - 1] == ';')
			missingpoint( conf[0] );
	}
	if (conf[0][conf[0].length() - 1] != ';')
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].find(";"));
	fill.index.push_back(conf[0]);
	conf.pop_front();
}

template < typename T >
void    redirect( deque_ &conf, T &fill ) {
	fill.redirect.first = ft_stoi(conf[0]);
	conf.pop_front();
	if (fill.redirect.first < 200 || fill.redirect.first > 504)
		throw std::runtime_error("Error: code not found");
    if (conf[0][conf[0].length() - 1] == ';')
        missingpoint( conf[0] );
    else
        throw std::runtime_error("Error: Missing ; at the end");
	fill.redirect.second = conf[0].substr(0, conf[0].length() - 1);
    conf.pop_front();
}