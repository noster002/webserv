/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 12:57:55 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/08 18:22:39 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

web::Client::Client(int fd, struct sockaddr addr, socklen_t addrlen)\
 : _socket(fd, addr, addrlen), _request(1024)
{
	return ;
}

web::Client::~Client(void)
{
	return ;
}


void		web::Client::disconnect(void)
{
	try
	{
		_socket.close();
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return ;
}

void		web::Client::set_non_blocking(void)
{
	_socket.set_non_blocking();
	return ;
}

void		web::Client::set_opt(int filter, int option)
{
	_socket.set_opt(filter, option);
	return ;
}

void		web::Client::set_kevent(int kq, int filter, int flags)
{
	_socket.set_kevent(kq, filter, flags);
	return ;
}

void		web::Client::read(int fd)
{
	std::string		input;

	ssize_t	bytes_read = ::recv(fd, static_cast<void *>(&input), 1024, 0);

	if (bytes_read < 0)
	{
		std::cerr << "recv: " << std::strerror(errno) << std::endl;
		return ;
	}

	try
	{
		_request.parse(input, bytes_read);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return ;
}

void		web::Client::write(int fd)
{
	std::string		output;

	_response.build(_request, output);

	ssize_t	bytes_sent = ::send(fd, static_cast<void const *>(&output), output.size(), 0);

	if (bytes_sent < 0)
	{
		std::cerr << "send: " << std::strerror(errno) << std::endl;
	}

	return ;
}


// canonical class form

web::Client::Client(Client const & other)\
 : _socket(), _request(0), _response()
{
	(void)other;

	return ;
}

web::Client &	web::Client::operator=(Client const & rhs)
{
	(void)rhs;

	return (*this);
}
