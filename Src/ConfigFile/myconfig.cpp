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
void	theresBraces( std::string data, int &in, int &out ) {
	std::strtok(const_cast<char *>(data.c_str()), " ;\n\t");
	for (int i = 0; i < data.length(); i++) {
		if (data[i] == '#')
			return ;
		else if (data[i] == CloseBrace)
			in++;
		else if (data[i] == OpenBrace)
			out++;
	}
}

void	checkBraces( std::string path ) {
	std::string		data;
	int				in = 0;
	int				out = 0;
	std::fstream	file(path);
	while (std::getline( file, data ))
		theresBraces( data, in, out );
	if (in != out)
		throw std::runtime_error("close the braces");
}

void	createInstance( std::vector < Server > &config, std::string path ) {
	std::string 	data;
	std::fstream	file(path);
	while (std::getline(file, data)) {
		std::strtok(const_cast<char *>(data.c_str()), "\n\t\0; ");
		std::cout << data << std::endl;
		if (data == "server{")
			std::cout << "---->server" << std::endl;
			// config.push_back(inst_);
	}
	if (!config.size())
		throw std::runtime_error("Bad Syntax");
}

void	parseServer( std::vector < Server > &config, std::string path ) {
	try {
		std::string		content;
		// std::fstream	file(av[1]);
		checkBraces( path );
		createInstance( config, path );
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	// while (std::getline(file, content)) {
	// 	std::strtok(content.c_str(), " \t;");
	// 	if (content == "server")
			
	// }
}

int main(int ac, char **av) {
	std::vector < Server > config;
	parseServer( config, av[1] );
}