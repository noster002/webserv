/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:49 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/09 15:26:08 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head/Server.hpp"

// constructor & destructor

web::Server::Server(params_t const & conf)\
 : _socket(), _kq(-1), _event(), _max_pending_clients(0), _conf(conf)
{
	return ;
}

web::Server::~Server(void)
{
	return ;
}


void		web::Server::set_addr_info(void)
{
	struct addrinfo		hints;
	struct protoent *	protocol;

	protocol = ::getprotobyname("ip");

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = protocol->p_proto;
	hints.ai_flags = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;

	_socket.get_addr_info(_conf.host.c_str(), _conf.port[0].c_str(), &hints);

	return ;
}

int			web::Server::setup_socket(void)
{
	try
	{
		set_addr_info();
		_socket.create();
		_socket.set_non_blocking();
		_socket.set_opt(SOL_SOCKET, SO_REUSEADDR);
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

int			web::Server::setup_kqueue(void)
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

void		web::Server::run(void)
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
		event_count = ::kevent(_kq, NULL, 0, &_event, 5, NULL);

		if (event_count < 0)
		{
			std::cerr << "kevent: " << std::strerror(errno) << std::endl;
		}

		for (int i = 0; i < event_count; ++i)
		{
			if (static_cast<int>(_event.ident) == _socket.get_fd())
				event_client_connect(_event);
			else if (_event.flags & EV_EOF)
				event_eof(_event);
			else if (_event.filter == EVFILT_READ)
				event_read(_event);
		}
	}
}

void		web::Server::event_client_connect(struct kevent const & event)
{
	struct sockaddr		addr;
	socklen_t			addrlen = sizeof(addr);

	int		fd = ::accept(event.ident, &addr, &addrlen);

	if (fd < 0)
	{
		std::cerr << "accept: " << std::strerror(errno) << std::endl;
		return ;
	}

	std::cout << "fd: " << fd << std::endl;

	_client[fd] = new Client(fd, addr, addrlen);

	try
	{
		_client[fd]->set_non_blocking();
		_client[fd]->set_kevent(_kq, EVFILT_READ, EV_ADD);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
		_client[fd]->disconnect();
		delete _client[fd];
		_client.erase(fd);
		return ;
	}
	std::cout << "connected" << std::endl;

	return ;
}

void		web::Server::event_client_disconnect(struct kevent const & event)
{
	int		fd = event.ident;

	try
	{
		_client[fd]->set_kevent(_kq, EVFILT_READ, EV_DELETE);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	_client[fd]->disconnect();
	delete _client[fd];
	_client.erase(fd);

	return ;
}

void		web::Server::event_eof(struct kevent const & event)
{
	event_client_disconnect(event);
	std::cout << "disconnected" << std::endl;
	return ;
}

void		web::Server::event_read(struct kevent const & event)
{
	int		fd = event.ident;

	int		nbr = _client[fd]->read(fd);

	_client[fd]->write(fd, nbr);

	return ;
}



// canonical class form

web::Server::Server(void)\
 : _socket(), _kq(), _event(), _max_pending_clients(), _client()
{
	return ;
}

web::Server::Server(Server const & other)\
 : _socket(), _kq(other._kq), _event(other._event), \
   _max_pending_clients(other._max_pending_clients), \
   _client()
{
	return ;
}

web::Server &				web::Server::operator=(Server const & rhs)
{
	_kq = rhs._kq;
	_event = rhs._event;
	_max_pending_clients = rhs._max_pending_clients;
	_client = rhs._client;

	return (*this);
}
