/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 11:59:56 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/06 15:36:53 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

http::Webserv::Webserv(void)\
 : _config(), _servers(), _clients(), _is_setup(false), _up(0)
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

	if (_config.getValidation() == false)
	{
		std::cerr << "configuration file: bad syntax" << std::endl;
		throw SetupException();
	}
	std::cout << "PARSING FINISHED" << std::endl;

	_kq = ::kqueue();

	if (_kq < 0)
	{
		std::cerr << "kqueue: " << std::strerror(errno) << std::endl;
		throw SetupException();
	}

	std::map<std::pair<std::string, std::string>, int>	host_ports;
	int		setup = EXIT_SUCCESS;

	for (int i = 0; i < _config.get_nbr_servers(); ++i)
	{
		Server		server(_config.get_server_conf(i));
		params_t	conf = server.get_conf();

		for (size_t j = 0; j < conf.port.size(); ++j)
		{
			std::pair<std::string, std::string>			host_port = std::make_pair(conf.host, conf.port[j]);

			if (host_ports.count(host_port) == 0)
			{
				setup = server.setup(_kq, conf.host, conf.port[j]);
				if (setup == EXIT_SUCCESS)
					host_ports[host_port] = server.get_last_socket_fd();
			}
			if (setup == EXIT_SUCCESS)
				_servers[host_ports[host_port]].push_back(server);
		}
	}

	_is_setup = true;
	_up = _servers.size();
}

void				http::Webserv::run(void)
{
	if (!_is_setup)
	{
		std::cout << "run loop: no servers setup" << std::endl;
		throw SetupException();
	}
	std::cout << _up << " sockets are setup" << std::endl;

	struct kevent	event[MAX_EVENTS];
	int				event_count;
	int				count = -1;

	while (_up)
	{
		event_count = ::kevent(_kq, NULL, 0, event, MAX_EVENTS, NULL);

		if (event_count < 0)
		{
			std::cerr << "kevent: " << std::strerror(errno) << std::endl;
		}

		for (int i = 0; i < event_count; ++i)
		{
			if (_servers.count(static_cast<int>(event[i].ident)))
				event_client_connect(event[i]);
			else if (event[i].flags & EV_EOF)
				event_eof(event[i]);
			else if (event[i].filter == EVFILT_READ)
				event_read(event[i]);
			else if (event[i].filter == EVFILT_WRITE)
				event_write(event[i]);
			std::ofstream	file("logfile.txt");
			file << ++count;
			file.close();
		}
	}
}

void				http::Webserv::clean(void)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		it->second->disconnect(_kq);
		delete it->second;
	}
	for (std::map<int, std::vector<Server> >::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		for (std::vector<Server>::iterator ite = it->second.begin(); ite != it->second.end(); ++ite)
			ite->clean();
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
		event_client_disconnect(event);
		return ;
	}
	std::cout << fd << " connected" << std::endl;

	return ;
}

void				http::Webserv::event_client_disconnect(struct kevent const & event)
{
	std::cout << RED << "DISCONNECTING" << RESET << std::endl;
	int		fd = event.ident;

	_clients[fd]->disconnect(_kq);
	delete _clients[fd];
	_clients.erase(fd);

	if (::close(fd) < 0)
	{
		std::cerr << "close: " << fd << ": " << std::strerror(errno) << std::endl;
	}
	std::cout << fd << " disconnected" << std::endl;

	return ;
}

void				http::Webserv::event_eof(struct kevent const & event)
{
	if (_clients.count(event.ident))
		event_client_disconnect(event);

	return ;
}

void				http::Webserv::event_read(struct kevent const & event)
{
	std::cout << RED << "READING" << RESET << std::endl;
	int				fd = event.ident;
	char			input[REQUEST_SIZE];

	ssize_t			bytes_read = ::recv(fd, static_cast<void *>(input), REQUEST_SIZE, 0);

	if (bytes_read < 0)
	{
		std::cerr << "recv: " << std::strerror(errno) << std::endl;
		return ;
	}
	std::cout << CYAN << "bytes_read: " << bytes_read << RESET << std::endl;
	/*std::cout << "INPUT:" << std::endl;
	std::cout << "(" << YELLOW;
	for (ssize_t pos = 0; pos < bytes_read; ++pos)
		std::cout << input[pos];
	std::cout << CYAN << ")"  << RESET << std::endl;*/

	_clients[fd]->read(input, bytes_read, _kq);

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
	std::cout << CYAN << "bytes_sent: " << bytes_sent << RESET << std::endl;
	std::cout << "OUTPUT:" << std::endl;
	std::cout << "(" << YELLOW << output << CYAN << ")"  << RESET << std::endl;

	int		wait = 10000000;

	while (wait)
		--wait;

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
