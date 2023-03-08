/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 14:59:56 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/22 15:00:01 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>

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

};

#endif
