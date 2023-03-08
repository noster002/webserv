/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 13:05:21 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/08 18:39:52 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

// C POSIX

# include <sys/socket.h>
# include <arpa/inet.h>
# include <sys/event.h>
# include <netdb.h>
# include <fcntl.h>
# include <unistd.h>

// C++98

# include <exception>
# include <string>
# include <iostream>
# include <cstring>
# include <cerrno>

namespace web
{
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
			struct sockaddr				_sockaddr;
			socklen_t					_sockaddrlen;
			struct kevent				_event_set;


			// exceptions

			class		Exception;

			// canonical class form

			Socket(Socket const & other);
			Socket &	operator=(Socket const & rhs);

	};

	enum		e_level
	{
		e_close, \
		e_get_addr_info, \
		e_create, \
		e_set_non_blocking, \
		e_set_opt, \
		e_bind, \
		e_listen, \
		e_set_kevent
	};

	class		Socket::Exception : public std::exception
	{
		public:

			Exception(int lvl);

			int						level;

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
}

#endif
