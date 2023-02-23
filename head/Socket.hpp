/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 13:05:21 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/23 13:28:58 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

// C POSIX

# include <sys/socket.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>

// C++98

# include <exception>
# include <string>
# include <iostream>
# include <cstring>
# include <cerrno>

class	Socket
{
	public:

		Socket(void);
		Socket(int fd, struct sockaddr addr, socklen_t addrlen);
		~Socket(void);

		int			getFd(void);

		void		clean(void);
		void		close(void);
		void		getaddrinfo(char const * hostname, \
								char const * port, \
								struct addrinfo * hints);
		void		create(void);
		void		setnonblocking(void);
		void		setopt(int level, int option);
		void		bind(void);
		void		listen(int max_pending_clients);
		void		setkevent(int kq, int filter, int flags);

	private:

		int							_err;
		int							_fd;
		struct addrinfo *			_addr;
		struct kevent				_event_set;


		// exceptions

		int			_level;

		class		Exception;

		// canonical class form

		Socket(Socket const & other);
		Socket &	operator=(Socket const & rhs);

};

enum		e_level
{
	close, \
	getaddrinfo, \
	create, \
	setnonblocking, \
	setopt, \
	bind, \
	listen, \
	setkevent
};

class		Socket::Exception : public std::exception
{
	public:

		char const *			what(void) const throw();

		static char const *		close(void);
		static char const *		getaddrinfo(void);
		static char const *		create(void);
		static char const *		setnonblocking(void);
		static char const *		setopt(void);
		static char const *		bind(void);
		static char const *		listen(void);
		static char const *		setkevent(void);

};

#endif
