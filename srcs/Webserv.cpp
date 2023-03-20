/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 11:59:56 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/20 15:15:08 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

http::Webserv::Webserv(void)\
 : _config(), _servers(), _clients(), _is_setup(false), _up(0)
{
	return ;
}

http::Webserv::Webserv(std::string const & filename)\
 : _config(filename), _servers(), _clients(), _is_setup(false), _up(0)
{
	return ;
}

http::Webserv::~Webserv(void)
{
	clean();
	return ;
}


void				http::Webserv::setup(std::string const & filename)
{
	_config.parse(filename);
	std::cout << _config.get_server_conf(1).client_max_body_size << std::endl;
	std::cout << "PARSING FINISHED" << std::endl;

	_kq = ::kqueue();

	if (_kq < 0)
	{
		std::cerr << "kqueue: " << std::strerror(errno) << std::endl;
		throw SetupException();
	}

	for (int i = 0; i < _config.get_nbr_servers(); ++i)
	{
		Server		server(_config.get_server_conf(i));

		server.setup(_kq);

		for (int j = 0; j < server.get_nbr_sockets(); ++j)
			_servers[server.get_socket_fd(j)] = server;
	}

	_is_setup = true;
	_up = _servers.size();
}

void				http::Webserv::run(void)
{
	if (!_is_setup)
	{
		std::cout << "Provide a configuration file and set servers up first" << std::endl;
		throw SetupException();
	}
	std::cout << _up << " sockets are setup" << std::endl;

	struct kevent	event;
	int				event_count;

	while (_up)
	{
		event_count = ::kevent(_kq, NULL, 0, &event, 5, NULL);

		if (event_count < 0)
		{
			std::cerr << "kevent: " << std::strerror(errno) << std::endl;
		}

		for (int i = 0; i < event_count; ++i)
		{
			if (_servers.count(static_cast<int>(event.ident)))
				event_client_connect(event);
			else if (event.flags & EV_EOF)
				event_eof(event);
			else if (event.filter == EVFILT_READ)
				event_read(event);
			else if (event.filter == EVFILT_WRITE)
				event_write(event);
		}
	}
	std::cout << _up << " sockets are setup" << std::endl;
}

void				http::Webserv::clean(void)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		it->second->disconnect(_kq);
		delete it->second;
	}
	for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		it->second.clean();
	}
	return ;
}

void				http::Webserv::event_client_connect(struct kevent const & event)
{
	std::cout << RED << "CONNECTING" << RESET << std::endl;
	int		fd = ::accept(event.ident, NULL, NULL);

	if (fd < 0)
	{
		std::cerr << "accept: " << std::strerror(errno) << std::endl;
		return ;
	}

	_clients[fd] = new Client(fd, _servers[event.ident]);

	try
	{
		_clients[fd]->connect(_kq);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
		_clients[fd]->disconnect(_kq);
		delete _clients[fd];
		_clients.erase(fd);
		return ;
	}
	std::cout << fd << " connected" << std::endl;

	return ;
}

void				http::Webserv::event_eof(struct kevent const & event)
{
	std::cout << RED << "DISCONNECTING" << RESET << std::endl;
	int		fd = event.ident;

	_clients[fd]->disconnect(_kq);
	delete _clients[fd];
	_clients.erase(fd);
	std::cout << fd << " disconnected" << std::endl;

	return ;
}

void				http::Webserv::event_read(struct kevent const & event)
{
	std::cout << RED << "READING" << RESET << std::endl;
	int				fd = event.ident;
	char			input[10000/* REQUEST_SIZE */];

	ssize_t			bytes_read = ::recv(fd, static_cast<void *>(input), 10000/* REQUEST_SIZE - 1 */, 0);

	if (bytes_read < 0)
	{
		std::cerr << "recv: " << std::strerror(errno) << std::endl;
		return ;
	}
	input[bytes_read] = '\0';
	std::cout << CYAN << "bytes_read: " << bytes_read << std::endl;
	std::cout << "INPUT:" << std::endl;
	std::cout << "(" << YELLOW << input << CYAN << ")"  << RESET << std::endl;

	_clients[fd]->read(input, _kq);

	return ;
}

void				http::Webserv::event_write(struct kevent const & event)
{
	std::cout << RED << "WRITING" << RESET << std::endl;
	int				fd = event.ident;
	std::string		output = _clients[fd]->write(_kq);

	ssize_t	bytes_sent = ::send(fd, static_cast<void const *>(output.c_str()), output.size(), 0);

	if (bytes_sent < 0)
	{
		std::cerr << "send: " << std::strerror(errno) << std::endl;
	}
	std::cout << CYAN << "bytes_sent: " << bytes_sent << std::endl;
	std::cout << "OUTPUT:" << std::endl;
	std::cout << "(" << YELLOW << output << CYAN << ")"  << RESET << std::endl;

	return ;
}


// canonical class form

http::Webserv::Webserv(Webserv const & other)\
 : _config(), _servers(), _clients(), _is_setup(false), _up(0)
{
	(void)other;
	return ;
}

http::Webserv &		http::Webserv::operator=(Webserv const & rhs)
{
	(void)rhs;
	return (*this);
}


char const *		http::Webserv::SetupException::what(void) const throw()
{
	return ("Setup failed");
}
