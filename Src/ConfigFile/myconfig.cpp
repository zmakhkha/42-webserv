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

Methods::Methods() : Get(false), Post(false), Delete(false) {}
Methods::~Methods() {}

bool Methods::empty() { return ((!Get && !Post && !Delete)); }

// void Config::setConfig(std::vector<Server> serv) { server = serv; }

// const std::vector<Server> &Config::getConfig() { return server; }

void	validate( Server check ) {
	(check.listen.first.empty() && check.listen.second.empty()) && (throw std::runtime_error("Error: Port is needed"), 0);
	(check.root.empty()) && (throw std::runtime_error("Error: root is needed"), 0);
	(check.allow.empty()) && (throw std::runtime_error("Error: Methods is needed"), 0);
	(check.up_path.empty()) && (throw std::runtime_error("Error: upPath is needed"), 0);
}

int	ft_stoi( std::string var ) {
	int ret;
	std::stringstream   ss(var);
	if (!(ss >> ret))
		throw std::runtime_error("Error: Not Integer");
	return ret;
}

void	splitData( std::deque < std::string > &conf, std::string data ) {
	size_t pos = data.find("#");
	if (pos != std::string::npos)
		data = data.substr(0, pos);
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
	while (conf.size() > 0 && conf[0] != "}") {
		for (i = 0; i < (int)directives->size(); i++) {
			if (directives[i] == conf[0]) {
				parseElementLoc( conf, i, loc );
				break ;
			}
			if (i == 7)
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
		&uploadPath, &serverName, &location,
	};
	(void)(*f[index])(conf, fill);
}

Server	parsing_conf( std::string data ) {
	int				i;
	Server			serv;
	deque_			conf; // get the vector and fill it somehow
	std::string		directives[] = {"max_body_size",
		"listen", "root", "allow",  "error_page",
		"upload_path", "server_name", "location", "cgi", "autoindex", "index"};
	// char *p = std::strtok((char *)data.c_str(), "\n");
	// while (p != NULL) {
		// std::cout << "->>" << p <<std::endl;
		splitData( conf, data );
	// 	p = std::strtok(NULL, "\n");
	// }
	for (size_t in = 0; in < conf.size(); in++) {
		if (conf[in + 1] == ";") {
			conf[in] += conf[in + 1];
			conf.erase(conf.begin() + in + 1);
		}
	}
	if (conf[0] != "server" || conf[1] != "{")
		throw std::runtime_error("Error: Bad Syntax");
	conf.pop_front();
	conf.pop_front();
	while (conf.size() > 0 && conf[0] != "}") {
		for (i = 0; i < (int)directives->size(); i++) {
			if (directives[i] == conf[0]) {
				parseElement( conf, i, serv );
				break ;
			}
			if (i == 10)
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
		std::cout << "\tupload_path " << conf[j].up_path << std::endl;
		std::cout << "\tservername " << conf[j].server_name << std::endl;
		for (int i = 0; i < (int)conf[j].location.size(); i++) {
			std::cout << "\tlocation " << conf[j].location[i].prefix << " {" << std::endl;
			std::cout << "\t\tautoindex " << conf[j].location[i].autoindex << std::endl;
			std::cout << "\t\tupload_path " << conf[j].location[i].up_path << std::endl;
			std::cout << "\t\tcgi" << " " << conf[j].location[i].cgi.first << " " << conf[0].location[i].cgi.second << std::endl;
			std::cout << "\t}\n";
		}
		std::cout << "}\n";
	}
}

int		countservers( std::string path ) { // error
	int				servers = 0;
	std::string		var;
	std::fstream	file(path);
	while (std::getline( file, var )) {
		std::strtok((char *)var.c_str(), " \t");
		std::cout << var << std::endl;
		size_t pos = var.find("server{");
		if (pos == std::string::npos)
			continue ;
		var.erase(0, pos + 8);
		servers++;
	}
	return servers;
}

vect_	LooponServers( vect_ &config, std::string path ) {
	std::string		var;
	std::string		data;
	std::fstream	file(path);
	int count = countservers( path );
	// std::cout << count << std::endl;
	// exit(0);
	if (!file.is_open())
		throw std::runtime_error("Error: Couldnt open the file");
	while (std::getline(file, var))
		data += var + "\n";
	for (int i = 0; !data.empty() && i < count; i++) {
		config.push_back(parsing_conf(data));
		std::cout << "->>" << data << std::endl;
		size_t brace = data.find("}");
		if (brace != std::string::npos && data[brace + 2] == '}')
			data.erase(0, brace + 4);
	}
	return config;
}

int main(int ac, char **av) {
	try {
		(void)ac;
		std::vector < Server > config;
		LooponServers( config, av[1] );
		// printfVec(config);
		// std::cout << config[0].location[7].cgi.first << std::endl;
		// std::cout << config[0].location[7].autoindex << std::endl;
		// std::cout << config[0].location[7].prefix << std::endl;
	}
	catch ( std::exception &e ) {
		std::cout << e.what() << std::endl;
	}
}