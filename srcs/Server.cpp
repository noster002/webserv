/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:49 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/15 15:39:13 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head/Server.hpp"

// constructor & destructor

Server::Server(std::string const & filename)\
 : _err(0), _sock()
{
	
}

Server::~Server(void)
{
	freeaddrinfo(_addr);

	return ;
}


int			Server::getaddrinfo(void)
{
	struct addrinfo		hints;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IP;
	hints.ai_flags = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;

	_err = ::getaddrinfo(/* hostname */, /* port */, &hints, &_addr);

	if (_err < 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(err) << std::endl;
	}

	return (_err);
}

int			Server::setNonBlocking(int fd)
{
	_err = ::fcntl(fd, F_SETFL, O_NONBLOCK);

	if (_err < 0)
	{
		std::cerr << "fcntl: " << strerror(errno) << std::endl;
	}

	return (_err);
}

int			Server::setsockopt(void)
{
	int		option = 1;

	_err = ::setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR,\
						reinterpret_cast<void const *>(&option), sizeof(int) );

	if (_err < 0)
	{
		std::cerr << "setsockopt: " << strerror(errno) << std::endl;
	}

	return (_err);
}

int			Server::socket(void)
{
	if (_err = getaddrinfo())
		return (_err);

	_err = ::socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);

	if (_err < 0)
	{
		std::cerr << "socket: " << strerror(errno) << std::endl;
		return (_err);
	}

	_sock = _err;

	if (_err = setNonBlocking(_sock) < 0)
		return (_err);

	_err = setoptsock();

	return (_err);
}

int			Server::bind(void)
{
	_err = ::bind(_sock, _addr.ai_addr, _addr.ai_addrlen);

	if (_err < 0)
	{
		std::cerr << "bind: " << strerror(errno) << std::endl;
	}

	return (_err);
}

int			Server::listen(void)
{
	_err = ::listen(_sock, _max_pending_clients);

	if (_err < 0)
	{
		std::cerr << "listen: " << strerror(errno) << std::endl;
	}

	return (_err);
}

int			Server::kqueue(void)
{
	_err = ::kqueue();

	if (_err < 0)
	{
		std::cerr << "kqueue: " << strerror(errno) << std::endl;
		return (_err);
	}

	_kq = _err;

	_err = 0;

	return (_err);
}

int			Server::setupRun(void)
{
	if (_err = socket() < 0)
		return (err);
	if (_err = bind() < 0)
		return (err);
	if (_err = listen() < 0)
		return (err);
	if (_err = kqueue() < 0)
		return (err);

	EV_SET = (&_event_set, _sock, EVFILT_READ, EV_ADD, 0, 0, NULL);

	_err = kevent(_kq, &_event_set, 1, NULL, 0, NULL);

	if (_err < 0)
	{
		std::cerr << "kqueue setup: " << strerror(errno) << std::endl;
	}

	return (_err);
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
				eventClientConnect(_event);
			else if (_event.flags & EV_EOF)
				eventEof(_event);
			else if (_event.filter == EVFILT_READ)
				eventRead(_event);
		}
	}
}

void		Server::eventClientConnect(struct kevent const & event)
{
	int		client = ::accept(event.ident, NULL, NULL);
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
