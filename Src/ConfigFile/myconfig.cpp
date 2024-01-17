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

void	splitData( std::deque < std::string > &conf, std::string data ) {
	char *p = std::strtok((char *)data.c_str(), " \n\t;");
	while (p != NULL) {
		conf.push_back(std::string(p));
		p = std::strtok(NULL, " \n\t");
	}
}

void	Error( deque_ conf, Server &fill ) {
	throw std::runtime_error("Error: 404 element not found");
}

void	Listen( deque_ &conf, Server &fill ) {
	conf.pop_front();
	std::strtok((char *)conf[0].c_str(), ";");
	size_t pos = conf[0].find(":");
	if (pos == std::string::npos)
		throw std::runtime_error("Error: listen");
	fill.listen.first = conf[0].substr(0, pos);
	fill.listen.second = conf[0].substr(pos + 1, conf[0].find(";"));
	conf.pop_front();
}

void	Root( deque_ &conf, Server &fill ) {
	conf.pop_front();
	size_t pos = conf[0].find(";");
	if (pos != std::string::npos)
		fill.root = conf[0].substr(0, pos);
	else
		throw std::runtime_error("Error: missing \";\" at the end");
	conf.pop_front();
}

void	Allow( deque_ &conf, Server &fill ) {
	int	count;
	conf.pop_front();
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

void	parseElement( deque_ &conf, int index, Server &fill ) {
	void (*f[])(deque_ &conf, Server &fill) = {
		&Error, &Listen, &Root, &Allow,
	};
	(void)(*f[index])(conf, fill);
}

Server	parsing_conf(const std::string &path) {
	Server			serv;
	deque_			conf; // get the vector and fill it somehow
	std::string		data;
	std::fstream	file(path);
	std::string		directives[] = {"autoindex",
		"listen", "root", "allow",  "upload_path",
		"location", "index", "cgi", "server_name"};
	while (std::getline(file, data))
		splitData( conf, data );
	if (conf[0] != "server" && conf[1] != "{")
		throw std::runtime_error("Error: Bad Syntax");
	conf.pop_front();
	conf.pop_front();
	while (conf[0] != "}") {
		for (int i = 0 ; i < directives->size(); i++)
			if (directives[i] == conf[0])
				parseElement( conf, i, serv );
			// else {
				// std::cout << conf[0] << std::endl;
				// throw std::runtime_error("Error: Unknown directive");
			// }
	}
	return serv;
}

void	printfVec( vect_ conf ) {
	std::cout << "server {\n";
	std::cout << "\tlisten " << conf[0].listen.first << ":" << conf[0].listen.second << std::endl;
	std::cout << "\troot " << conf[0].root << std::endl;
	std::cout << "\tallow " << conf[0].allow.Get << " " << conf[0].allow.Post << " " << conf[0].allow.Delete << std::endl;
	std::cout << "}\n";

}

int main(int ac, char **av) {
	try {
		std::vector < Server > config;
		config.push_back(parsing_conf(av[1]));
		printfVec(config);
	}
	catch ( std::exception &e ) {
		std::cout << e.what() << std::endl;
	}
}