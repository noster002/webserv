/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 12:57:55 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/13 17:57:34 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

http::Client::Client(int fd, Server const & server)\
 : _server(server), _socket(fd), _request(), _response()
{
	return ;
}

http::Client::~Client(void)
{
	return ;
}


void					http::Client::connect(int kq)
{
	_socket.set_non_blocking();
	_socket.set_kevent(kq, EVFILT_READ, EV_ADD);

	return ;
}

void					http::Client::disconnect(int kq)
{
	try
	{
		_socket.set_kevent(kq, EVFILT_READ, EV_DELETE);
		_socket.close();
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return ;
}

void					http::Client::read(std::string const & input, int kq)
{
	_request.add_buffer(input);

	try
	{
		_request.parse();
		_response.build(_request);
		_socket.set_kevent(kq, EVFILT_WRITE, EV_ADD | EV_ENABLE);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return ;
}

std::string const &		http::Client::write(int kq)
{
	std::string		output = _response.get_buffer();

	try
	{
		_request.clear();
		_response.clear();
		_socket.set_kevent(kq, EVFILT_WRITE, EV_DISABLE);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (output);
}


// canonical class form

http::Client::Client(void)\
 : _server(), _socket(), _request(), _response()
{
	return ;
}

http::Client::Client(Client const & other)\
 : _server(other._server), _socket(), _request(), _response()
{
	return ;
}

http::Client &	http::Client::operator=(Client const & rhs)
{
	(void)rhs;

	return (*this);
}
