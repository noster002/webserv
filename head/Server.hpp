/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:41 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/13 15:43:06 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/event.h>
# include <fcntl.h>
# include <string>
# include <unistd.h>
# include "ServerConf.hpp"

class	Server
{
	public:

		Server(ServerConf const & conf, int fd);
		~Server(void);

		void	run(void);

	private:

		int							_sock;
		struct sockaddr_in			_addr;
		struct kevent				_event_set;
		struct kevent				_event;
		int const					_max_pending_clients;
		int							_socket;
		ssize_t						_request_size;
		long unsigned const			_max_buffer_size;
		char *						_buffer;

		// canonical class form
		Server(void);
		Server(Server const & other);
		Server &	operator=(Server const & rhs);

};

#endif
