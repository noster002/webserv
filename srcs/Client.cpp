/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 12:57:55 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/06 16:34:57 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd, struct sockaddr addr, socklen_t addrlen)\
 : _socket(fd, addr, addrlen), _request(client_max_body_size)
{
	return ;
}

Client::~Client(void)
{
	return ;
}


void		Client::disconnect(void)
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

void		Client::set_non_blocking(void)
{
	_socket.set_non_blocking();
	return ;
}

void		Client::set_kevent(int kq, int filter, int flags)
{
	_socket.set_kevent(kq, filter, flags);
	return ;
}

void		Client::read(int fd, size_t max_size)
{
	std::string		input;

	ssize_t	bytes_read = ::recv(fd, static_cast<void *>(&input), max_size, NULL);

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

void		Client::write(int fd)
{
	std::string		output;

	_response.build(_request, output);

	ssize_t	bytes_sent = ::send(fd, static_cast<void const *>(&output), output.size(), NULL);

	if (bytes_sent < 0)
	{
		std::cerr << "send: " << std::strerror(errno) << std::endl;
	}

	return ;
}


// canonical class form

Client::Client(Client const & other)\
 : _socket(), _request(), _response()
{
	(void)other;

	return ;
}

Client &	Client::operator=(Client const & rhs)
{
	(void)rhs;

	return (*this);
}
