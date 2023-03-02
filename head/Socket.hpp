/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 13:05:21 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/01 10:07:02 by nosterme         ###   ########.fr       */
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

		int			get_fd(void);

		void		clean(void);
		void		close(void);
		void		get_addr_info(char const * hostname, \
								char const * port, \
								struct addrinfo * hints);
		void		create(void);
		void		set_non_blocking(void);
		void		set_opt(int level, int option);
		void		bind(void);
		void		listen(int max_pending_clients);
		void		set_kevent(int kq, int filter, int flags);

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
	get_addr_info, \
	create, \
	set_non_blocking, \
	set_opt, \
	bind, \
	listen, \
	set_kevent
};

class		Socket::Exception : public std::exception
{
	public:

		char const *			what(void) const throw();

		static char const *		close(void);
		static char const *		get_addr_info(void);
		static char const *		create(void);
		static char const *		set_non_blocking(void);
		static char const *		set_opt(void);
		static char const *		bind(void);
		static char const *		listen(void);
		static char const *		set_kevent(void);

};

#endif
