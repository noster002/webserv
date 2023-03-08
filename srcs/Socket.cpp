/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 13:05:08 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/08 18:41:46 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

web::Socket::Socket(void) : _err(0), _fd(), _addr()
{
	return ;
}

web::Socket::Socket(int fd, struct sockaddr addr, socklen_t addrlen)\
 : _err(0), _fd(fd)
{
	_sockaddr = addr;
	_sockaddrlen = addrlen;

	return ;
}

web::Socket::~Socket(void)
{
	clean();

	return ;
}


int			web::Socket::get_fd(void)
{
	return (_fd);
}

void		web::Socket::clean(void)
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

void		web::Socket::close(void)
{
	_err = ::close(_fd);

	if (_err < 0)
	{
		std::cerr << "close fd: " << _fd << ": " << std::strerror(errno) << std::endl;
		throw Exception(e_close);
	}
}

void		web::Socket::get_addr_info(char const * hostname, \
								char const * port, \
								struct addrinfo * hints)
{
	_err = ::getaddrinfo(hostname, port, hints, &_addr);

	if (_err)
	{
		std::cerr << "get_addr_info: " << ::gai_strerror(_err) << std::endl;
		throw Exception(e_get_addr_info);
	}
}

void		web::Socket::create(void)
{
	_err = ::socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);

	if (_err < 0)
	{
		std::cerr << "socket: " << std::strerror(errno) << std::endl;
		throw Exception(e_create);
	}

	_fd = _err;
}

void		web::Socket::set_non_blocking(void)
{
	_err = ::fcntl(_fd, F_SETFL, O_NONBLOCK);

	if (_err < 0)
	{
		std::cerr << "fcntl: " << std::strerror(errno) << std::endl;
		throw Exception(e_set_non_blocking);
	}
}

void		web::Socket::set_opt(int level, int option)
{
	int		one = 1;

	_err = ::setsockopt(_fd, level, option,\
						reinterpret_cast<void const *>(&one), sizeof(int) );

	if (_err < 0)
	{
		std::cerr << "setsockopt: level: " << level << " option: " << option;
		std::cerr << ": " << std::strerror(errno) << std::endl;
		throw Exception(e_set_opt);
	}
}

void		web::Socket::bind(void)
{
	_err = ::bind(_fd, _addr->ai_addr, _addr->ai_addrlen);

	if (_err < 0)
	{
		std::cerr << "bind: " << std::strerror(errno) << std::endl;
		throw Exception(e_bind);
	}
}

void		web::Socket::listen(int max_pending_clients)
{
	_err = ::listen(_fd, max_pending_clients);

	if (_err < 0)
	{
		std::cerr << "listen: " << std::strerror(errno) << std::endl;
		throw Exception(e_listen);
	}
}

void		web::Socket::set_kevent(int kq, int filter, int flags)
{
	EV_SET(&_event_set, _fd, filter, flags, 0, 0, NULL);

	_err = ::kevent(kq, &_event_set, 1, NULL, 0, NULL);

	if (_err < 0)
	{
		std::cerr << "kevent: " << std::strerror(errno) << std::endl;
		throw Exception(e_set_kevent);
	}
}


// canonical class form

web::Socket::Socket(Socket const & other)\
 : _err(other._err), _fd(other._fd), _addr(other._addr),\
   _event_set(other._event_set)
{
	return ;
}

web::Socket &			web::Socket::operator=(Socket const & rhs)
{
	_err = rhs._err;
	_fd = rhs._fd;
	_addr = rhs._addr;
	_event_set = rhs._event_set;

	return (*this);
}


// exceptions

web::Socket::Exception::Exception(int lvl) : level(lvl) {}

char const *		web::Socket::Exception::what(void) const throw()
{
	switch (level)
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

char const *		web::Socket::Exception::close(void)
{
	return ("Socket: error while closing");
}

char const *		web::Socket::Exception::get_addr_info(void)
{
	return ("Socket: cannot get addrinfo struct");
}

char const *		web::Socket::Exception::create(void)
{
	return ("Socket: cannot create socket");
}

char const *		web::Socket::Exception::set_non_blocking(void)
{
	return ("Socket: cannot set socket nonblocking");
}

char const *		web::Socket::Exception::set_opt(void)
{
	return ("Socket: cannot set socket to user defined options");
}

char const *		web::Socket::Exception::bind(void)
{
	return ("Socket: cannot bind socket");
}

char const *		web::Socket::Exception::listen(void)
{
	return ("Socket: cannot listen");
}

char const *		web::Socket::Exception::set_kevent(void)
{
	return ("Socket: cannot set kevent");
}
