/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:49 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/04 13:28:56 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head/Server.hpp"

// constructor & destructor

Server::Server(ServerConf const & conf, int fd)\
 : _fd(fd)
{
	fcntl(this->_fd, F_SETFL, O_NONBLOCK);
	return ;
}

Server::~Server(void) {}

// getter & setter

int const				Server::get_fd(void) const
{
	return (this->_fd);
}

struct sockaddr const *	Server::get_addr(void) const
{
	return (static_cast<struct sockaddr *>(&(this->_addr)));
}

socklen_t const			Server::get_addr_len(void) const
{
	return (static_cast<socklen_t>(sizeof(this->_addr)));
}

int const				Server::get_max_pending_clients(void) const
{
	return (this->_max_pending_clients);
}

int						Server::get_socket(void) const
{
	return (this->_socket);
}

void					Server::set_socket(int fd)
{
	this->_socket = fd;
	return ;
}

// canonical class form

Server::Server(void) {}

Server::Server(Server const & other) : _fd(other._fd) {}

Server &				Server::operator=(Server const & rhs)
{
	this->_fd = rhs._fd;
	return (*this);
}
