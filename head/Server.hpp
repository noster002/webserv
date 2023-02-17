/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:41 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/16 14:42:37 by nosterme         ###   ########.fr       */
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

		Server(std::string const & filename);
		~Server(void);

		void	run(void);

	private:

		int							_err;
		int							_sock;
		struct addrinfo *			_addr;
		int							_kq;
		struct kevent				_event_set;
		struct kevent				_event;
		int const					_max_pending_clients;

		// std::map<int, Client *>		_client;

		// canonical class form
		Server(void);
		Server(Server const & other);
		Server &	operator=(Server const & rhs);

};

#endif
