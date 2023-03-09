/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 12:57:55 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/09 13:39:20 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

web::Client::Client(int fd, struct sockaddr addr, socklen_t addrlen)\
 : _socket(fd, addr, addrlen)
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

int			web::Client::read(int fd)
{
	char		input[1024];

	ssize_t	bytes_read = ::recv(fd, static_cast<void *>(input), 1024, 0);
	std::cout << "(\033[31m" << input << "\033[0m)" << std::endl;

	if (bytes_read < 0)
	{
		std::cerr << "recv: " << std::strerror(errno) << std::endl;
		return (-1);
	}

	int			nbr;

	for (nbr = 0; nbr < 20; ++nbr)
	{
		if (_request.count(nbr) == 0)
			break ;
	}
	_request[nbr] = new Request(1024);

	try
	{
		_request[nbr]->parse(input, bytes_read);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (nbr);
}

void		web::Client::write(int fd, int nbr)
{
	std::string		output;

	_response[nbr] = new Response();

	_response[nbr]->build(_request[nbr], output);

	ssize_t	bytes_sent = ::send(fd, static_cast<void const *>(output.c_str()), output.size(), 0);

	if (bytes_sent < 0)
	{
		std::cerr << "send: " << std::strerror(errno) << std::endl;
	}

	delete _request[nbr];
	delete _response[nbr];

	return ;
}


// canonical class form

web::Client::Client(Client const & other)\
 : _socket()
{
	(void)other;

	return ;
}

web::Client &	web::Client::operator=(Client const & rhs)
{
	(void)rhs;

	return (*this);
}
