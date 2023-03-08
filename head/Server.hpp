/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:41 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/08 17:23:42 by nosterme         ###   ########.fr       */
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
# include "ServerConf.hpp"

namespace web
{
	class	Server
	{
		public:

			Server(params_t const & conf);
			~Server(void);

			void	run(void);

			int		setup_socket(void);
			int		setup_kqueue(void);

			void	event_client_connect(struct kevent const & event);
			void	event_client_disconnect(struct kevent const & event);
			void	event_eof(struct kevent const & event);
			void	event_read(struct kevent const & event);

		private:

			Socket						_socket;
			int							_kq;
			struct kevent				_event;
			int							_max_pending_clients;

			params_t					_conf;

			std::map<int, Client *>		_client;

			void	set_addr_info(void);


			// canonical class form

			Server(void);
			Server(Server const & other);
			Server &	operator=(Server const & rhs);

	};
}

#endif
