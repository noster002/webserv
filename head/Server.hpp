/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:41 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/23 13:03:54 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

// C POSIX

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/event.h>
# include <netdb.h>
# include <fcntl.h>
# include <unistd.h>

// C++98

# include <map>
# include <exception>
# include <string>
# include <iostream>
# include <cstring>
# include <cerrno>

// private

# include "Socket.hpp"
# include "Client.hpp"

class	Server
{
	public:

		Server(std::string const & filename);
		~Server(void);

		void	run(void);

		int		setupSocket(void);
		int		setupKqueue(void);

		void	eventClientConnect(struct kevent const & event);
		void	eventClientDisconnect(struct kevent const & event);
		void	eventEof(struct kevent const & event);
		void	eventRead(struct kevent const & event);

	private:

		Socket						_socket;
		int							_kq;
		struct kevent				_event;
		int							_max_pending_clients;

		std::map<int, Client *>		_client;

		void	setaddrinfo(void);


		// canonical class form

		Server(void);
		Server(Server const & other);
		Server &	operator=(Server const & rhs);

};

#endif
