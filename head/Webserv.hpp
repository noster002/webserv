/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 11:53:54 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/07 08:47:59 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

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

# include "utils.hpp"
# include "Config.hpp"
# include "Server.hpp"
# include "Client.hpp"

namespace http
{
	class	Webserv
	{
		public:

			Webserv(void);
			~Webserv(void);

			void		setup(std::string const & filename);
			void		run(void);
			void		clean(void);

		private:

			Config									_config;
			std::map<int, std::vector<Server> >		_servers;
			std::map<int, Client *>					_clients;

			bool									_is_setup;
			unsigned int							_up;

			int										_kq;
			struct kevent							_events[MAX_CHANGES];
			int										_event_count;

			void		event_client_connect(struct kevent const & event);
			void		event_client_disconnect(struct kevent const & event);
			void		event_eof(struct kevent const & event);
			void		event_read(struct kevent const & event);
			void		event_write(struct kevent const & event);


			class		SetupException;

			// canonical class form

			Webserv(Webserv const & other);
			Webserv &	operator=(Webserv const & rhs);

	};

	class		Webserv::SetupException : public std::exception
	{
		public:

			char const *		what(void) const throw();

	};
}

#endif
