/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 14:59:56 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/06 13:11:22 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

// C POSIX

# include <sys/socket.h>

// C++98

# include <queue>
# include <utility>
# include <exception>
# include <string>
# include <iostream>
# include <cstring>
# include <cerrno>

// private

# include "Server.hpp"
# include "Socket.hpp"
# include "Request.hpp"
# include "Response.hpp"

namespace http
{
	class	Client
	{
		public:

			Client(int fd, std::vector<Server> const & servers);
			~Client(void);

			void							connect(int kq);
			void							disconnect(int kq);

			void							read(char const * input, ssize_t bytes, int kq);
			std::string						write(int kq);

			void							clear(void);

		private:

			std::vector<Server> const &						_servers;
			Socket											_socket;
			std::pair<Request *, Response *> *				_current_message;
			std::queue<std::pair<Request *, Response *> >	_message_queue;

			params_t const &				_get_server(t_request const & request);


			// canonical class form

			Client(void);
			Client(Client const & other);
			Client &	operator=(Client const & rhs);

	};
}

#endif
