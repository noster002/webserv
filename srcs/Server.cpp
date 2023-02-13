/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:49 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/13 15:55:47 by nosterme         ###   ########.fr       */
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

Server::Server()\
 : _sock()
{
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = /* address */;
	_addr.sin_port = htons(/* port */);

	_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (_sock < 0)
	{
		return ;
	}

	fcntl(fd, F_SETFL, O_NONBLOCK);
}

Server::~Server(void)
{
	delete [] this->_buffer;
	return ;
}


int			Server::bind(void)
{
	int		err = ::bind(_sock, \
						 reinterpret_cast<struct sockaddr *>(&_address), \
						 sizeof(_address));

	if (err < 0)
	{
		std::cerr << "bind: " << strerror(errno) << std::endl;
	}

	return (err);
}

int			Server::listen(void)
{
	int		err = ::listen(_sock, _max_pending_clients);

	if (err < 0)
	{
		std::cerr << "listen: " << strerror(errno) << std::endl;
	}

	return (err);
}

int			Server::setupRun(void)
{
	int		err = 0;
	
	if (err = bind())
		return (err);
	if (err = listen())
		return (err);

	_kq = kqueue();

	EV_SET = (&_event_set, _sock, EVFILT_READ, EV_ADD, 0, 0, NULL);

	err = kevent(_kq, &_event_set, 1, NULL, 0, NULL);

	if (err < 0)
	{
		std::cerr << "kqueue setup: " << strerror(errno) << std::endl;
	}

	return (err);
}

void		Server::run(void)
{
	if (setupRun() < 0)
		return ;

	int		event_count = 0;

	while (1)
	{
		event_count = kevent(_kq, NULL, 0, &_event, 1, NULL);

		if (event_count < 0)
		{
			std::cerr << "kevent: " << stderror(errno) << std::endl;
		}

		for (int i = 0; i < event_count; ++i)
		{
			if (_event.ident == _sock)
				clientConnect(_event);
			else if (_event.flags & EV_EOF)
				eof(_event);
			else if (_event.filter == EVFILT_READ)
				read(_event);
		}
	}
}

void		Server::clientConnect(struct kevent const & event)
{
	int		client = ::accept(event.ident, );
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
