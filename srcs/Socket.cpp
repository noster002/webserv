/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 13:05:08 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/13 17:40:14 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

http::Socket::Socket(void) : _fd(), _addr()
{
	return ;
}

http::Socket::Socket(int fd)\
 : _fd(fd), _addr(NULL)
{
	return ;
}

http::Socket::~Socket(void)
{
	clean();

	return ;
}


int			http::Socket::get_fd(void)
{
	return (_fd);
}

void		http::Socket::clean(void)
{
	::freeaddrinfo(_addr);
	try
	{
		close();
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void		http::Socket::close(void)
{
	if (::close(_fd) < 0)
	{
		std::cerr << "close fd: " << _fd << ": " << std::strerror(errno) << std::endl;
		throw Exception(e_close);
	}
}

void		http::Socket::get_addr_info(char const * hostname, \
								char const * port, \
								struct addrinfo * hints)
{
	if (::getaddrinfo(hostname, port, hints, &_addr))
	{
		std::cerr << "get_addr_info: " << ::gai_strerror(_err) << std::endl;
		throw Exception(e_get_addr_info);
	}
}

void		http::Socket::create(void)
{
	_fd = ::socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol)

	if (_fd < 0)
	{
		std::cerr << "socket: " << std::strerror(errno) << std::endl;
		throw Exception(e_create);
	}
}

void		http::Socket::set_non_blocking(void)
{
	if (::fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "fcntl: " << std::strerror(errno) << std::endl;
		throw Exception(e_set_non_blocking);
	}
}

void		http::Socket::set_opt(int level, int option)
{
	int		one = 1;

	if (::setsockopt(_fd, level, option, reinterpret_cast<void const *>(&one), sizeof(int)) < 0)
	{
		std::cerr << "setsockopt: fd: " << _fd << " level: " << level << " option: " << option;
		std::cerr << ": " << std::strerror(errno) << std::endl;
		throw Exception(e_set_opt);
	}
}

void		http::Socket::bind(void)
{
	if (::bind(_fd, _addr->ai_addr, _addr->ai_addrlen) < 0)
	{
		std::cerr << "bind: " << std::strerror(errno) << std::endl;
		throw Exception(e_bind);
	}
}

void		http::Socket::listen(int max_pending_clients)
{
	if (::listen(_fd, max_pending_clients) < 0)
	{
		std::cerr << "listen: " << std::strerror(errno) << std::endl;
		throw Exception(e_listen);
	}
}

void		http::Socket::set_kevent(int kq, int filter, int flags)
{
	struct kevent	event;

	EV_SET(&event, _fd, filter, flags, 0, 0, NULL);

	if (::kevent(kq, &event, 1, NULL, 0, NULL) < 0)
	{
		std::cerr << "kevent: " << std::strerror(errno) << std::endl;
		throw Exception(e_set_kevent);
	}
}


// canonical class form

http::Socket::Socket(Socket const & other)\
 : _fd(other._fd), _addr(other._addr)
{
	return ;
}

http::Socket &			http::Socket::operator=(Socket const & rhs)
{
	_fd = rhs._fd;
	_addr = rhs._addr;

	return (*this);
}


// exceptions

http::Socket::Exception::Exception(int level) : _level(level) {}

char const *		http::Socket::Exception::what(void) const throw()
{
	switch (_level)
	{
		case e_close:
			return (Exception::close());
		case e_get_addr_info:
			return (Exception::get_addr_info());
		case e_create:
			return (Exception::create());
		case e_set_non_blocking:
			return (Exception::set_non_blocking());
		case e_set_opt:
			return (Exception::set_opt());
		case e_bind:
			return (Exception::bind());
		case e_listen:
			return (Exception::listen());
		case e_set_kevent:
			return (Exception::set_kevent());
	}
	return ("Undefined Error");
}

char const *		http::Socket::Exception::close(void)
{
	return ("Socket: error while closing");
}

char const *		http::Socket::Exception::get_addr_info(void)
{
	return ("Socket: cannot get addrinfo struct");
}

char const *		http::Socket::Exception::create(void)
{
	return ("Socket: cannot create socket");
}

char const *		http::Socket::Exception::set_non_blocking(void)
{
	return ("Socket: cannot set socket nonblocking");
}

char const *		http::Socket::Exception::set_opt(void)
{
	return ("Socket: cannot set socket to user defined options");
}

char const *		http::Socket::Exception::bind(void)
{
	return ("Socket: cannot bind socket");
}

char const *		http::Socket::Exception::listen(void)
{
	return ("Socket: cannot listen");
}

char const *		http::Socket::Exception::set_kevent(void)
{
	return ("Socket: cannot set kevent");
}
