#include "ConfigFile.hpp"

template < typename T >
void	Listen( deque_ &conf, T &fill ) {
	size_t pos = conf[0].find(":");
	if (pos == std::string::npos)
		fill.listen.first = conf[0].substr(0, conf[0].find(";"));
	else {
		fill.listen.first = conf[0].substr(0, pos);
		fill.listen.second = conf[0].substr(pos + 1, conf[0].find(";"));
	}
	conf.pop_front();
}

template < typename T >
void	Root( deque_ &conf, T &fill ) {
	size_t pos = conf[0].find(";");
	if (pos != std::string::npos)
		fill.root = conf[0].substr(0, pos);
	else
		throw std::runtime_error("Error: missing \";\" at the end");
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
	if (conf[0][conf[0].length() - 1] != ';')
		throw std::runtime_error("Error: missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].find(";"));
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
	if (conf[1][conf[1].length() - 1] != ';')
		throw std::runtime_error("Error: Missing ; at the end");
	conf[1] = conf[1].substr(0, conf[1].find(";"));
	fill.error_page[ft_stoi(conf[0])] = conf[1];
	conf.pop_front();
	conf.pop_front();
}

template < typename T >
void	maxbodySize( deque_ &conf, T &fill ) {
	if (conf[0][conf[0].length() - 1] != ';')
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
	if (conf[0][conf[0].length() - 1] != ';')
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].find(";"));
	fill.up_path = conf[0];
	if (access(fill.up_path.c_str(), F_OK) == -1)
		throw std::runtime_error("Error: UpPath Not Found");
	conf.pop_front();
}

template < typename T >
void	autoindex( deque_ &conf, T &fill ) {
	if (conf[0][conf[0].length() - 1] != ';')
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].find(";"));
	fill.autoindex = (conf[0] == "ON" || conf[0] == "on");
	conf.pop_front();
}

template < typename T >
void	serverName( deque_ &conf, T &fill ) {
	if (conf[0][conf[0].length() - 1] != ';')
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].find(";"));
	fill.server_name = conf[0];
	conf.pop_front();
}

template < typename T >
void	cgi( deque_ &conf, T &fill ) {
	fill.cgi.first = conf[0];
	conf.pop_front();
	if (conf[0][conf[0].length() - 1] != ';')
		throw std::runtime_error("Error: Missing ; at the end");
	conf[0] = conf[0].substr(0, conf[0].find(";"));
	fill.cgi.second = conf[0];
	conf.pop_front();
}
