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

#include "ConfigFile.hpp"
#include <fstream>
#include <sys/stat.h>

void	validate( Server check ) {
	(check.root.empty()) && (throw std::runtime_error("Error: root is needed"), 0);
	(check.up_path.empty()) && (throw std::runtime_error("Error: upPath is needed"), 0);
	(check.listen.first.empty() && check.listen.second.empty()) && (throw std::runtime_error("Error: Port is needed"), 0);
	// (check.allow.empty()) && (throw std::runtime_error("Error: Methods is needed"), 0);
}

int	ft_stoi( std::string var ) {
	int ret;
	std::stringstream   ss(var);
	if (!(ss >> ret))
		throw std::runtime_error("Error: Not Integer");
	return ret;
}

void	splitData( std::deque < std::string > &conf, std::string data ) {
	char *p = std::strtok((char *)data.c_str(), " \n\t;");
	while (p != NULL) {
		conf.push_back(std::string(p));
		p = std::strtok(NULL, " \n\t");
	}
	for (size_t in = 0; in < conf.size(); in++) {
		if (conf[in + 1] == ";") {
			conf[in] += conf[in + 1];
			conf.erase(conf.begin() + in + 1);
		}
	}
}

void	parseElementLoc( deque_ &conf, int index, Location &fill ) {
	conf.pop_front();
	if (conf[0].empty())
		throw std::runtime_error("Error: empty Directive");
	void (*fc[])(deque_ &conf, Location &fill) = {
		&maxbodySize, &Root, &Allow, &errorPage,
		&uploadPath, &cgi, &autoindex,
	};
	(void)(*fc[index])(conf, fill);
}

void	location( deque_ &conf, Server &fill ) { // location needs to be full with the same elements as  server | ; at the end | {} | multiple servers | comments
	Location	loc;
	int 		i;
	(conf[0][0] != '/') && (throw std::runtime_error("Error: Bad Prefix"), 0);
	loc.prefix = conf[0];
	conf.pop_front();
	(conf[0] != "{") && (throw std::runtime_error("Error: Bad Syntax Location"), 0);
	conf.pop_front();
	std::string		directives[] = {"max_body_size",
		"root", "allow",  "error_page",
		"upload_path", "cgi", "autoindex", "index"};
	while (conf[0] != "}") {
		for (i = 0; i < (int)directives->size(); i++) {
			if (directives[i] == conf[0]) {
				parseElementLoc( conf, i, loc );
				break ;
			}
		}
		if (i == (int)directives->size())
			throw std::runtime_error("Error: no such Directive as" + conf[0]);
	}
	fill.location.push_back(loc);
	conf.pop_front();
}

void	parseElement( deque_ &conf, int index, Server &fill ) {
	conf.pop_front();
	if (conf[0].empty())
		throw std::runtime_error("Error: empty Directive");
	void (*f[])(deque_ &conf, Server &fill) = {
		&maxbodySize, &Listen, &Root, &Allow, &errorPage,
		&uploadPath, &serverName, &location,
	};
	(void)(*f[index])(conf, fill);
}

Server	parsing_conf(const std::string &path) {
	int				i;
	Server			serv;
	deque_			conf; // get the vector and fill it somehow
	std::string		data;
	std::fstream	file(path);
	std::string		directives[] = {"max_body_size",
		"listen", "root", "allow",  "error_page",
		"upload_path", "server_name", "location", "cgi", "autoindex", "index"};
	while (std::getline(file, data))
		splitData( conf, data );
	if (conf[0] != "server" || conf[1] != "{")
		throw std::runtime_error("Error: Bad Syntax");
	conf.pop_front();
	conf.pop_front();
	while (conf[0] != "}") {
		for (i = 0; i < (int)directives->size(); i++) {
			if (directives[i] == conf[0]) {
				parseElement( conf, i, serv );
				break ;
			}
		}
		if (i == (int)directives->size())
			throw std::runtime_error("Error: no such Directive as" + conf[0]);
	}
	validate(serv);
	return serv;
}

void	printfVec( vect_ conf ) {
	std::cout << "server {\n";
	std::cout << "\tlisten " << conf[0].listen.first << ":" << conf[0].listen.second << std::endl;
	std::cout << "\troot " << conf[0].root << std::endl;
	std::cout << "\tallow " << conf[0].allow.Get << " " << conf[0].allow.Post << " " << conf[0].allow.Delete << std::endl;
	std::cout << "\terror_page " << conf[0].error_page[404] << std::endl;
	std::cout << "\tmax_body_size " << conf[0].body_size.first << conf[0].body_size.second << std::endl;
	std::cout << "\tupload_path " << conf[0].up_path << std::endl;
	std::cout << "\tservername " << conf[0].server_name << std::endl;
	std::cout << "\tlocation " << conf[0].location[0].prefix << " {" << std::endl;
	std::cout << "\t\tautoindex " << conf[0].location[0].autoindex << std::endl;
	std::cout << "\t\tupload_path " << conf[0].location[0].up_path << std::endl;
	std::cout << "\t\tcgi" << " " << conf[0].location[0].cgi.first << " " << conf[0].location[0].cgi.second << std::endl;
	std::cout << "\t}\n";
	std::cout << "}\n";
}

int main(int ac, char **av) {
	try {
		(void)ac;
		std::vector < Server > config;
		config.push_back(parsing_conf(av[1]));
		printfVec(config);
	}
	catch ( std::exception &e ) {
		std::cout << e.what() << std::endl;
	}
}