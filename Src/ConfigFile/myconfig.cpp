/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   myconfig.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 19:55:12 by marvin            #+#    #+#             */
/*   Updated: 2024/01/16 19:55:12 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "myconfig.hpp"
#include <fstream>
#include <sys/stat.h>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

Methods::Methods() : Get(false), Post(false), Delete(false) {}
Methods::~Methods() {}

bool Methods::empty() { return ((!Get && !Post && !Delete)); }

std::vector<Server> Config::server;

// void Config::setConfig(std::vector<Server> serv) { server = serv; }

const std::vector<Server> &Config::getConfig() { return server; }

void	validate( Server check ) {
	(check.listen.first.empty() && check.listen.second.empty()) && (throw std::runtime_error("Error: Port is needed"), 0);
	(check.root.empty()) && (throw std::runtime_error("Error: root is needed"), 0);
	(check.allow.empty()) && (throw std::runtime_error("Error: Methods is needed"), 0);
	(check.up_path.empty()) && (throw std::runtime_error("Error: upPath is needed"), 0);
}

int	ft_stoi( std::string var ) {
	int ret;
	for (int i = 0; i < (int)var.length(); i++)
		if (isdigit(var[i]) == 0)
			throw std::runtime_error("Error: Not Integer");
	std::stringstream   ss(var);
	if (!(ss >> ret))
		throw std::runtime_error("Error: Overflow");
	return ret;
}

void	splitData( std::deque < std::string > &conf, std::string data ) {
	size_t pos;
	while ((pos = data.find("#")) != std::string::npos) {
		std::string	trimmed(&data[pos]);
		data = data.erase(pos, trimmed.find("\n"));
	}
	char *p = std::strtok((char *)data.c_str(), " \n\t;");
	while (p != NULL) {
		conf.push_back(std::string(p));
		p = std::strtok(NULL, " \n\t");
	}
}

void	parseElementLoc( deque_ &conf, int index, Location &fill ) {
	conf.pop_front();
	if (conf[0].empty())
		throw std::runtime_error("Error: empty Directive");
	void (*fc[])(deque_ &conf, Location &fill) = {
		&maxbodySize, &Root, &Allow, &errorPage,
		&uploadPath, &cgi, &autoindex, &indexfiles, &redirect,
	};
	(void)(*fc[index])(conf, fill);
}

void	fillTheLoc( Server &fill, Location &loc ) {
	(!fill.root.empty()) && (loc.root = fill.root, 0);
	(!fill.up_path.empty()) && (loc.up_path = fill.up_path, 0);
	(!fill.allow.empty()) && (loc.allow.Get = fill.allow.Get, loc.allow.Post = fill.allow.Post, loc.allow.Delete = fill.allow.Delete, 0);
	(!fill.redirect.second.empty()) && (loc.redirect = fill.redirect, 0);
	loc.body_size = fill.body_size;
}

void	location( deque_ &conf, Server &fill ) { // location needs to be full with the same elements as  server | ; at the end | {} | multiple servers | comments
	Location	loc;
	int 		i;
	(conf[0][0] != '/') && (throw std::runtime_error("Error: Bad Prefix"), 0);
	loc.prefix = conf[0];
	conf.pop_front();
	(conf[0] != "{") && (throw std::runtime_error("Error: Bad Syntax Location"), 0);
	conf.pop_front();
	fillTheLoc( fill, loc );
	std::string		directives[] = {"max_body_size",
		"root", "allow",  "error_page",
		"upload_path", "cgi", "autoindex", "index", "redirect"};
	while (conf.size() > 0 && conf[0] != "}") {
		for (i = 0; i < (int)directives->size(); i++) {
			if (directives[i] == conf[0]) {
				parseElementLoc( conf, i, loc );
				break ;
			}
			if (i == 8)
				throw std::runtime_error("Error: no such Directive as " + conf[0]);
		}
	}
	if (conf[0] != "}")
		throw std::runtime_error("Error: missing } at the end of the scope");
	fill.location.push_back(loc);
	conf.pop_front();
}

void	parseElement( deque_ &conf, int index, Server &fill ) {
	conf.pop_front();
	if (conf[0].empty())
		throw std::runtime_error("Error: empty Directive");
	void (*f[])(deque_ &conf, Server &fill) = {
		&maxbodySize, &Listen, &Root, &Allow, &errorPage,
		&uploadPath, &serverName, &location, &redirect,
	};
	(void)(*f[index])(conf, fill);
}

Server	parsing_conf( std::string data ) {
	int				i;
	Server			serv;
	deque_			conf; // get the vector and fill it somehow
	std::string		directives[] = {"max_body_size",
		"listen", "root", "allow",  "error_page",
		"upload_path", "server_name", "location", "redirect"};
	splitData( conf, data );
	for (size_t in = 0; in < conf.size(); in++) {
		if (conf[in + 1] == ";") {
			conf[in] += conf[in + 1];
			conf.erase(conf.begin() + in + 1);
		}
	}
    if (conf[0] == "server" || conf[1] == "{") {
        conf.pop_front();
        conf.pop_front();
    }
    else if (conf[0] == "server{")
        conf.pop_front();
    else
        throw std::runtime_error("Error: Bad Syntax");
	while (conf.size() > 0 && conf[0] != "}") {
		for (i = 0; i < (int)directives->size(); i++) {
			if (directives[i] == conf[0]) {
				parseElement( conf, i, serv );
				break ;
			}
			if (i == 8)
				throw std::runtime_error("Error: no such Directive as " + conf[0]);
		}
	}
	validate(serv);
	if (conf[0] != "}")
		throw std::runtime_error("Error: missing } at the end of the scope");
	return serv;
}

void	printfVec( vect_ conf ) {
	for (int j = 0; j < (int)conf.size(); j++) {
		std::cout << "server {\n";
		std::cout << "\tlisten " << conf[j].listen.first << ":" << conf[j].listen.second << std::endl;
		std::cout << "\troot " << conf[j].root << std::endl;
		std::cout << "\tallow " << conf[j].allow.Get << " " << conf[j].allow.Post << " " << conf[j].allow.Delete << std::endl;
		std::cout << "\terror_page " << conf[j].error_page[404] << std::endl;
		std::cout << "\tmax_body_size " << conf[j].body_size.first << conf[j].body_size.second << std::endl;
		std::cout << "\t\tredirect " << " " << conf[j].redirect.first <<  " " << conf[j].redirect.second << std::endl;
		std::cout << "\tupload_path " << conf[j].up_path << std::endl;
		std::cout << "\tservername " << conf[j].server_name << std::endl;
		for (int i = 0; i < (int)conf[j].location.size(); i++) {
			std::cout << "\tlocation " << conf[j].location[i].prefix << " {" << std::endl;
			std::cout << "\t\tautoindex " << conf[j].location[i].autoindex << std::endl;
			std::cout << "\t\tupload_path " << conf[j].location[i].up_path << std::endl;
			std::cout << "\t\troot" << " " << conf[j].location[i].root << std::endl;
			std::cout << "\t\tallow" << " " << conf[j].location[i].allow.Get << " " << conf[j].location[i].allow.Post << " " << conf[j].location[i].allow.Delete << std::endl;
			std::cout << "\t\tmax_body_size " << " " << conf[j].location[i].body_size.first << conf[j].location[i].body_size.second << std::endl;
			std::cout << "\t\tredirect " << " " << conf[j].location[i].redirect.first <<  " " << conf[j].location[i].redirect.second << std::endl;
			std::cout << "\t\tindex ";
			for (size_t x = 0; x < conf[j].location[i].index.size(); x++)
				std::cout << conf[j].location[i].index[x] << " ";
			std::cout << std::endl;
			std::cout << "\t}\n";
		}
		std::cout << "}\n";
	}
}

int		countservers( std::string path ) { // error
	size_t			pos;
	std::string		var;
	std::fstream	file(path);
	int				servers = 0;
	if (!file.is_open())
		throw std::runtime_error("Error: Could not Open file");
	while (std::getline( file, var )) {
		std::strtok((char *)var.c_str(), " \t\n");
		if ((pos = var.find("server_name")) != std::string::npos)
			continue ;
		size_t pos = var.find("server\0{");
		if (pos == std::string::npos)
			continue ;
		servers++;
	}
	return servers;
}

void	Config::LooponServers( std::string path ) {
	std::string		var;
	std::string		data;
	std::fstream	file(path);
	if (!file.is_open())
		throw std::runtime_error("Error: Couldnt open the file");
	while (std::getline(file, var))
		data += var + "\n";
	if (data.empty())
		throw std::runtime_error("Error: Empty Config file");
	int count = countservers( path );
	for (int i = 0; !data.empty() && i < count; i++) {
		Config::server.push_back(parsing_conf(data));
		size_t brace = data.find("}\n}");
		if (brace != std::string::npos) {
			data.erase(0, brace + 3);
		}
	}
	for (size_t i = 0; i < server.size(); i++)
    {
		for(size_t j = i + 1; j < server.size(); j++)
			if(server[j].listen.second == server[i].listen.second && server[j].server_name == server[i].server_name)
				throw std::runtime_error("Error: Duplicated server !");
	}
	if (server.size() == 0)
		throw std::runtime_error("Error: no Server Found");
}

Server Config::getservconf( std::string server_name, std::string host )
{
	std::pair<std::string , std::string> listen = std::make_pair(host.substr(0, host.find(":")), host.substr(host.find(":") + 1));
	std::pair<std::string , std::string> dummy = std::make_pair("0.0.0.0", "8090");
	Server ret;
	for(size_t i = 0; i < server.size(); i++)
	{
		if(server[i].listen.first == listen.first && server[i].listen.second == listen.second && (server_name == server[i].server_name || server_name.empty()))
		{
			ret = server[i];
			break;
		}
	}
	return ret;
}