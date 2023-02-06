/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/06 14:46:12 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/06 15:08:14 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head/Error.hpp"

int		Error::arg(void)
{
	std::cerr << "Wrong number of arguments" << std::endl <<\
				 "Usage: ./webserv [conf_file]" << std::endl;
	return (1);
}

int		Error::conf_file(void)
{
	std::cerr << "Error in configuration file" << std::endl;
	return (1);
}

int		Error::socket(void)
{
	std::cerr << "Socket creation failed" << std::endl <<\
				 "Aborting..." << std::endl;
	return (1);
}

int		Error::bind(void)
{
	std::cerr << "Binding socket to address failed" << std::endl <<\
				 "Aborting..." << std::endl;
	return (1);
}

int		Error::listen(void)
{
	std::cerr << "Socket not listening" << std::endl <<\
				 "Aborting..." << std::endl;
	return (1);
}

int		Error::accept(void)
{
	std::cerr << "Connection failed" << std::endl <<\
				 "Aborting..." << std::endl;
	return (1);
}
