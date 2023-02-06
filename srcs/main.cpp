/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 10:31:09 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/06 15:11:48 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../head/Server.hpp"
#include "../head/ServerConf.hpp"
#include "../head/Error.hpp"

int		main(int argc, char** argv)
{
	if (argc != 2)
		return (Error::arg());

	ServerConf		serverconf(argv[1]);

	if (serverconf.parse() < 0)
		return (Error::conf_file());

	Server			server(serverconf, socket(AF_INET, SOCK_STREAM, 0));

	if (server.get_fd() < 0)
		return (Error::socket());
	if (bind(server.get_fd(), server.get_addr(), server.get_addr_len()) < 0)
		return (Error::bind());
	if (listen(server.get_fd(), server.get_max_pending_clients()))
		return (Error::listen());
	while (1)//up)
	{
		server.set_socket(accept(server.get_fd(), server.get_addr(), \
								 server.get_mutable_addr_len()));
		if (server.get_socket() < 0)
			return (Error::accept());
		server.set_request_size(read(server.get_socket(), \
									 server.get_buffer(), \
									 server.get_max_buffer_size()));
		std::cout << server.get_buffer() << std::endl;
	}
	return (0);
}