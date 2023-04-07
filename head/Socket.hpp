/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 13:05:21 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/07 09:29:53 by nosterme         ###   ########.fr       */
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

namespace http
{
	class	Socket
	{
		public:

			Socket(void);
			Socket(int fd);
			Socket(Socket const & other);
			Socket &	operator=(Socket const & rhs);
			~Socket(void);

			int			get_fd(void) const;

			void		clean(void);
			void		close(void);
			void		get_addr_info(char const * hostname,\
									  char const * port,\
									  struct addrinfo * hints);
			void		create(void);
			void		set_non_blocking(void);
			void		set_opt(int level, int option);
			void		bind(void);
			void		listen(int max_pending_clients);
			void		set_kevent(struct kevent * event, int filter, int flags);


			class		Exception;

		private:

			int							_fd;
			struct addrinfo *			_addr;

	};

	enum		e_level
	{
		e_close, \
		e_get_addr_info, \
		e_create, \
		e_set_non_blocking, \
		e_set_opt, \
		e_bind, \
		e_listen
	};

	class		Socket::Exception : public std::exception
	{
		public:

			Exception(int level);

			int						get_level(void) const;

			char const *			what(void) const throw();

			static char const *		close(void);
			static char const *		get_addr_info(void);
			static char const *		create(void);
			static char const *		set_non_blocking(void);
			static char const *		set_opt(void);
			static char const *		bind(void);
			static char const *		listen(void);

		private:

			int						_level;

	};
}

#endif
