/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 12:57:55 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/20 16:32:27 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

http::Client::Client(int fd, Server const & server)\
 : _server(server), _socket(fd), \
   _request(server.get_conf()), \
   _response(server.get_conf())
{
	return ;
}

http::Client::~Client(void)
{
	return ;
}


void			http::Client::connect(int kq)
{
	_socket.set_non_blocking();
	_socket.set_kevent(kq, EVFILT_READ, EV_ADD);
	_socket.set_kevent(kq, EVFILT_WRITE, EV_ADD | EV_DISABLE);

	return ;
}

void			http::Client::disconnect(int kq)
{
	try
	{
		_socket.set_kevent(kq, EVFILT_READ, EV_DELETE);
		_socket.set_kevent(kq, EVFILT_WRITE, EV_DELETE);
		_socket.close();
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return ;
}

void			http::Client::read(std::string const & input, int kq)
{
	_request.add_buffer(input);

	_request.parse();
	std::cout << GREEN << "BUILD" << RESET << std::endl;

	_response.build(_request.get_error(), _request.get_conf());

	try
	{
		_socket.set_kevent(kq, EVFILT_WRITE, EV_ENABLE);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return ;
}

std::string		http::Client::write(int kq)
{
	std::string		output = _response.get_buffer();

	clear();

	try
	{
		_socket.set_kevent(kq, EVFILT_WRITE, EV_DISABLE);
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (output);
}

void			http::Client::clear(void)
{
	_request.~Request();
	new (&_request) Request(_server.get_conf());
	_response.~Response();
	new (&_response) Response(_server.get_conf());

	return ;
}


// canonical class form

http::Client::Client(Client const & other)\
 : _server(other._server), _socket(), \
   _request(other._server.get_conf()), \
   _response(other._server.get_conf())
{
	return ;
}

http::Client &	http::Client::operator=(Client const & rhs)
{
	(void)rhs;

	return (*this);
}
