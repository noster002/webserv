/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:49 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/01 10:15:59 by nosterme         ###   ########.fr       */
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


void		Server::set_addr_info(void)
{
	struct addrinfo		hints;
	struct protoent *	protocol;

	protocol = ::getprotobyname("ip");

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = protocol.p_proto;
	hints.ai_flags = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;

	_socket.get_addr_info(/* hostname */, /* port */, &hints);

	return ;
}

int			Server::setup_socket(void)
{
	try
	{
		set_addr_info();
		_socket.create();
		_socket.set_non_blocking();
		_socket.set_opt(EVFILT_READ, EV_ADD);
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

int			Server::setup_kqueue(void)
{
	_kq = ::kqueue();

	if (_kq < 0)
	{
		std::cerr << "kqueue: " << std::strerror(errno) << std::endl;
		return (_kq);
	}

	try
	{
		_socket.set_kevent(_kq, EVFILT_READ, EV_ADD);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
		if (::close(_kq) < 0)
		{
			std::cerr << "close kq: " << _kq << std::strerror(errno) << std::endl;
		}
		return (-1);
	}

	return (0);
}

void		Server::run(void)
{
	if (setup_socket() < 0 || setup_kqueue() < 0)
	{
		std::cerr << "server setup failed" << std::endl\
				  << "Aborting..." << std::endl;
		_socket.clean();
		return ;
	}

	int		event_count = 0;

	while (1)
	{
		event_count = ::kevent(_kq, NULL, 0, &_event, 1, NULL);

		if (event_count < 0)
		{
			std::cerr << "kevent: " << std::strerror(errno) << std::endl;
		}

		for (int i = 0; i < event_count; ++i)
		{
			if (_event.ident == _sock)
				event_client_connect(_event);
			else if (_event.flags & EV_EOF)
				event_eof(_event);
			else if (_event.filter == EVFILT_READ)
				event_read(_event);
		}
	}
}

void		Server::event_client_connect(struct kevent const & event)
{
	struct sockaddr		addr;
	socklen_t			addrlen = sizeof(addr);

	int		fd = ::accept(event.ident, &addr, &addrlen);

	if (fd < 0)
	{
		std::cerr << "accept: " << std::strerror(errno) << std::endl;
		return (-1);
	}

	_client[fd] = new Client(fd, addr, addrlen);

	try
	{
		_client[fd]->set_non_blocking();
		_client[fd]->set_opt(EVFILT_READ, EV_ADD);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
		_client[fd]->disconnect();
		return (-1);
	}

	return (0);
}

void		Server::event_client_disconnect(struct kevent const & event)
{
	int		fd = event.ident;

	try
	{
		_client[fd]->set_kevent(EVFILT_READ, EV_DELETE);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	_client[fd]->disconnect();

	return ;
}

void		Server::event_eof(struct kevent const & event)
{
	event_client_disconnect(event);
	return ;
}

void		Server::event_read(struct kevent const & event)
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
