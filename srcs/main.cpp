/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 10:31:09 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/14 14:35:19 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Error.hpp"

int		main(int argc, char** argv)
{
	if (argc != 2)
		return (Error::arg());

	try
	{
		http::Webserv	webserv;

		webserv.setup(argv[1]);
		webserv.run();
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
