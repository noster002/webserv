/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:49 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/06 15:19:00 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head/Server.hpp"

// constructor & destructor

Server::Server(ServerConf const & conf, int fd)\
 : _fd(fd), _addr(), _addr_len(static_cast<socklen_t>(sizeof(_addr))), \
   _max_pending_clients(10), _max_buffer_size(2048)
{
	(void)conf;
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(8000);
	this->_buffer = new char[this->_max_buffer_size];
	return ;
}

Server::~Server(void)
{
	delete [] this->_buffer;
	return ;
}

// getter & setter

int						Server::get_fd(void) const
{
	return (this->_fd);
}

struct sockaddr *		Server::get_addr(void)
{
	return (reinterpret_cast<struct sockaddr *>(&(this->_addr)));
}

socklen_t				Server::get_addr_len(void) const
{
	return (this->_addr_len);
}

socklen_t *				Server::get_mutable_addr_len(void)
{
	return (&this->_addr_len);
}

int						Server::get_max_pending_clients(void) const
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

void					Server::set_request_size(ssize_t size)
{
	this->_request_size = size;
	return ;
}

long unsigned			Server::get_max_buffer_size(void) const
{
	return (this->_max_buffer_size);
}

char *					Server::get_buffer(void) const
{
	return (this->_buffer);
}

// canonical class form

Server::Server(void) : _fd(), _max_pending_clients(), _max_buffer_size() {}

Server::Server(Server const & other)\
 : _fd(other._fd), _max_pending_clients(other._max_pending_clients), \
   _max_buffer_size(other._max_buffer_size) {}

Server &				Server::operator=(Server const & rhs)
{
	new (this) Server(rhs);
	return (*this);
}
