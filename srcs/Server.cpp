/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:49 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/23 13:06:39 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head/Server.hpp"

// constructor & destructor

Server::Server(std::string const & filename)\
 : _socket(), _kq(-1), _event(), _max_pending_clients(0)
{
	return ;
}

Server::~Server(void)
{
	return ;
}


void		Server::setaddrinfo(void)
{
	struct addrinfo		hints;
	struct protoent *	protocol;

	protocol = getprotobyname("ip");

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = protocol.p_proto;
	hints.ai_flags = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;

	_socket.getaddrinfo(/* hostname */, /* port */, &hints);

	return ;
}

int			Server::setupSocket(void)
{
	try
	{
		setaddrinfo();
		_socket.create();
		_socket.setnonblocking();
		_socket.setopt(EVFILT_READ, EV_ADD);
		_socket.bind();
		_socket.listen(_max_pending_clients);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}

	return (0);
}

int			Server::setupKqueue(void)
{
	_kq = ::kqueue();

	if (_kq < 0)
	{
		std::cerr << "kqueue: " << strerror(errno) << std::endl;
		return (_kq);
	}

	try
	{
		_socket.setkevent(_kq, EVFILT_READ, EV_ADD);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
		if (::close(_kq) < 0)
		{
			std::cerr << "close kq: " << _kq << strerror(errno) << std::endl;
		}
		return (-1);
	}

	return (0);
}

void		Server::run(void)
{
	if (setupSocket() < 0 || setupKqueue() < 0)
	{
		std::cerr << "server setup failed" << std::endl\
				  << "Aborting..." << std::endl;
		_socket.clean();
		return ;
	}

	int		event_count = 0;

	while (1)
	{
		event_count = kevent(_kq, NULL, 0, &_event, 1, NULL);

		if (event_count < 0)
		{
			std::cerr << "kevent: " << strerror(errno) << std::endl;
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
	struct sockaddr		addr;
	socklen_t			addrlen = sizeof(addr);

	int		fd = ::accept(event.ident, &addr, &addrlen);

	if (fd < 0)
	{
		std::cerr << "accept: " << strerror(errno) << std::endl;
		return (-1);
	}

	_client[fd] = new Client(fd, addr, addrlen);

	try
	{
		_client[fd]->setnonblocking();
		_client[fd]->setopt(EVFILT_READ, EV_ADD);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
		_client[fd]->disconnect();
		return (-1);
	}

	return (0);
}

void		Server::eventClientDisconnect(struct kevent const & event)
{
	int		fd = event.ident;

	try
	{
		_client[fd]->setkevent(EVFILT_READ, EV_DELETE);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	_client[fd]->disconnect();

	return ;
}

void		Server::eventEof(struct kevent const & event)
{
	eventClientDisconnect(event);
	return ;
}

void		Server::eventRead(struct kevent const & event)
{
	int		fd = event.ident;

	_client[fd]->read(fd, /* max_size */);

	_client[fd]->write(fd);

	return ;
}



// canonical class form

Server::Server(void)\
 : _socket(), _kq(), _event(), _max_pending_clients(), _client()
{
	return ;
}

Server::Server(Server const & other)\
 : _socket(), _kq(other._kq), _event(other._event), \
   _max_pending_clients(other._max_pending_clients), \
   _client(other._client);
{
	return ;
}

Server &				Server::operator=(Server const & rhs)
{
	_kq = rhs._kq;
	_event = rhs._event;
	_max_pending_clients = rhs._max_pending_clients;
	_client = rhs._client;

	return (*this);
}
