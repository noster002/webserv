/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 14:59:56 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/23 13:07:15 by nosterme         ###   ########.fr       */
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

# include "Socket.hpp"
# include "Request.hpp"
# include "Response.hpp"

class	Client
{
	public:

		Client(int fd, struct sockaddr addr, socklen_t addrlen);
		~Client(void);

		void		disconnect(void);
		void		setnonblocking(void);
		void		setkevent(int kq, int filter, int flags);
		void		read(int fd);
		void		write(int fd);

	private:

		Socket		_socket;
		Request		_request;
		Response	_response;


		// canonical class form

		Client(Client const & other);
		Client &	operator=(Client const & rhs);

};

#endif
