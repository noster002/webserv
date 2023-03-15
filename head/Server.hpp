/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:41 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/15 11:24:38 by nosterme         ###   ########.fr       */
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
# include "Config.hpp"

namespace http
{
	class	Server
	{
		public:

			Server(void);
			Server(params_t const & conf);
			Server(Server const & other);
			Server &	operator=(Server const & rhs);
			~Server(void);

			int		get_socket_fd(int index) const;
			int		get_nbr_sockets(void) const;

			void	setup(int kq);
			void	clean(void);

		private:

			params_t					_conf;
			std::vector<Socket *>		_sockets;

			void	set_addr_info(Socket & socket, char const * port);

	};
}

#endif
