/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 10:31:09 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/07 08:18:11 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

int		main(int argc, char** argv)
{
	std::string		filename;

	if (argc == 1)
		filename = "conf/server.conf";
	else if (argc == 2)
		filename = argv[1];
	else
	{
		std::cerr << "Wrong number of arguments" << std::endl <<\
					 "Usage: ./webserv [conf_file]" << std::endl;
		return (1);
	}
	
	try
	{
		http::Webserv	webserv;

		webserv.setup(filename);
		webserv.run();
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
