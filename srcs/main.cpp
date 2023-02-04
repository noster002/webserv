/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 10:31:09 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/04 13:19:23 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


int		main(int argc, char** argv)
{
	if (argc != 2)
		return (error_arg());
	
	ServerConf		serverconf(argv[1]);

	if (serverconf.parse() < 0)
		return (error_conf_file());

	Server			server(serverconf, socket(AF_INET, SOCK_STREAM, 0));

	if (server.get_fd() < 0)
		return (error_socket());
	if (bind(server.get_fd(), server.get_addr(), server.get_addr_len()) < 0)
		return (error_bind());
	if (listen(server.get_fd(), server.get_max_pending_clients()))
		return (error_listen());
	while (up)
	{
		server.set_socket(accept(server.get_fd(), ));
		if (server.get_socket() < 0)
			return (error_accept());
	}
	return (0);
}