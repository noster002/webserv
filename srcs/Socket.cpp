/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 13:05:08 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/22 15:02:52 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(void) : _err(0), _fd(), _addr()
{
	return ;
}

Socket::Socket(int fd, struct sockaddr addr, socklen_t addrlen)\
 : _err(0), _fd(fd), _addr()
{
	_addr.ai_addr = &addr;
	_addr.ai_addrlen = addrlen;

	return ;
}

Socket::~Socket(void)
{
	clean();

	return ;
}


int			Socket::getFd(void)
{
	return (_fd);
}

void		Socket::clean(void)
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

void		Socket::close(void)
{
	_err = ::close(_fd);

	if (_err < 0)
	{
		std::cerr << "close fd: " << _fd << ": " << strerror(errno) << std::endl;
		_level = e_close;
		throw Exception;
	}
}

void		Socket::getaddrinfo(char const * hostname, \
								char const * port, \
								struct addrinfo * hints)
{
	_err = ::getaddrinfo(hostname, port, hints, &_addr);

	if (_err)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(_err) << std::endl;
		_level = e_getaddrinfo;
		throw Exception;
	}
}

void		Socket::create(void)
{
	_err = ::socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);

	if (_err < 0)
	{
		std::cerr << "socket: " << strerror(errno) << std::endl;
		_level = e_create;
		throw Exception;
	}

	_fd = _err;
}

void		Socket::setnonblocking(void)
{
	_err = ::fcntl(_fd, F_SETFL, O_NONBLOCK);

	if (_err < 0)
	{
		std::cerr << "fcntl: " << strerror(errno) << std::endl;
		_level = e_setnonblocking;
		throw Exception;
	}
}

void		Socket::setopt(int level, int option)
{
	int		one = 1;

	_err = setsockopt(_fd, level, option,\
					  reinterpret_cast<void const *>(&one), sizeof(int) );

	if (_err < 0)
	{
		std::cerr << "setsockopt: level: " << level << " option: " << option;
		std::cerr << ": " << strerror(errno) << std::endl;
		_level = e_setopt;
		throw Exception;
	}
}

void		Socket::bind(void)
{
	_err = ::bind(_fd, _addr.ai_addr, _addr.ai_addrlen);

	if (_err < 0)
	{
		std::cerr << "bind: " << strerror(errno) << std::endl;
		_level = e_bind;
		throw Exception;
	}
}

void		Socket::listen(int max_pending_clients)
{
	_err = ::listen(_fd, max_pending_clients);

	if (_err < 0)
	{
		std::cerr << "listen: " << strerror(errno) << std::endl;
		_level = e_listen;
		throw Exception;
	}
}

void		Socket::setkevent(int kq, int filter, int flags)
{
	EV_SET(&_event_set, _fd, filter, flags, 0, 0, NULL);

	_err = ::kevent(kq, &_event_set, 1, NULL, 0, NULL);

	if (_err < 0)
	{
		std::cerr << "kevent: " << strerror(errno) << std::endl;
		_level = e_setkevent;
		throw Exception;
	}
}


// exceptions

char const *		Socket::Exception::what(void) const throw()
{
	switch (_level)
	{
		case e_close:
			return (Exception::close());
		case e_getaddrinfo:
			return (Exception::getaddrinfo());
		case e_create:
			return (Exception::create());
		case e_setnonblocking:
			return (Exception::setnonblocking());
		case e_setopt:
			return (Exception::setopt());
		case e_bind:
			return (Exception::bind());
		case e_listen:
			return (Exception::listen());
		case e_setkevent:
			return (Exception::setkevent());
	}
}

char const *		Socket::Exception::close(void)
{
	return ("Socket: error while closing");
}

char const *		Socket::Exception::getaddrinfo(void)
{
	return ("Socket: cannot get addrinfo struct");
}

char const *		Socket::Exception::create(void)
{
	return ("Socket: cannot create socket");
}

char const *		Socket::Exception::setnonblocking(void)
{
	return ("Socket: cannot set socket nonblocking");
}

char const *		Socket::Exception::setopt(void)
{
	return ("Socket: cannot set socket to user defined options");
}

char const *		Socket::Exception::bind(void)
{
	return ("Socket: cannot bind socket");
}

char const *		Socket::Exception::listen(void)
{
	return ("Socket: cannot listen");
}

char const *		Socket::Exception::setkevent(void)
{
	return ("Socket: cannot set kevent");
}
