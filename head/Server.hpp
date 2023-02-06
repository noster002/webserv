/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:41 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/06 14:38:28 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <string>
# include <unistd.h>
# include "ServerConf.hpp"

class	Server
{
	public:

		Server(ServerConf const & conf, int fd);
		~Server(void);

		int							get_fd(void) const;
		struct sockaddr *			get_addr(void);
		socklen_t					get_addr_len(void) const;
		socklen_t *					get_mutable_addr_len(void);
		int							get_max_pending_clients(void) const;
		int							get_socket(void) const;
		void						set_socket(int fd);
		void						set_request_size(ssize_t size);
		char *						get_buffer(void) const;
		long unsigned				get_max_buffer_size(void) const;

	private:

		int const					_fd;
		struct sockaddr_in			_addr;
		socklen_t					_addr_len;
		int const					_max_pending_clients;
		int							_socket;
		ssize_t						_request_size;
		long unsigned const			_max_buffer_size;
		char *						_buffer;

		// canonical class form
		Server(void);
		Server(Server const & other);
		Server &	operator=(Server const & rhs);

};

#endif
