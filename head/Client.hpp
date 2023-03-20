/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 14:59:56 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/20 16:32:43 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

// C POSIX

# include <sys/socket.h>

// C++98

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

			Client(int fd, Server const & server);
			~Client(void);

			void				connect(int kq);
			void				disconnect(int kq);

			void				read(std::string const & input, int kq);
			std::string			write(int kq);

			void				clear(void);

		private:

			Server const &		_server;
			Socket				_socket;
			Request				_request;
			Response			_response;


			// canonical class form

			Client(void);
			Client(Client const & other);
			Client &	operator=(Client const & rhs);

	};
}

#endif
