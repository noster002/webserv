/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:41 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/04 13:09:33 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include "ServerConf.hpp"

class	Server
{
	public:

		Server(ServerConf const & conf, int fd);
		~Server(void);

		int const					get_fd(void) const;
		struct sockaddr const *		get_addr(void) const;
		socklen_t const				get_addr_len(void) const;
		int const					get_max_pending_clients(void) const;
		int const					get_socket(void) const;
		void						set_socket(int fd);

	private:

		int const					_fd;
		struct sockaddr_in const	_addr;
		int const					_max_pending_clients;
		int							_socket;

		// canonical class form
		Server(void);
		Server(Server const & other);
		Server &	operator=(Server const & rhs);

};

#endif
