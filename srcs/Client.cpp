/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 12:57:55 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/23 09:07:18 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

http::Client::Client(int fd, Server const & server)\
 : _server(server), _socket(fd), \
   _request(NULL), \
   _response(NULL)
{
	_request = new Request(server.get_conf());
	_response = new Response(server.get_conf());
	return ;
}

http::Client::~Client(void)
{
	delete _request;
	delete _response;
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

void			http::Client::read(char const * input, ssize_t bytes, int kq)
{
	_request->add_buffer(input, bytes);

	if (_request->parse() == EXIT_FAILURE)
		return ;
	std::cout << GREEN << "BUILD" << RESET << std::endl;

	_response->build(_request->get_error(), _request->get_conf());

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
	std::string		output = _response->get_buffer();

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
	new (_request) Request(_server.get_conf());
	new (_response) Response(_server.get_conf());

	return ;
}


// canonical class form

http::Client::Client(Client const & other)\
 : _server(other._server), _socket(), \
   _request(NULL), \
   _response(NULL)
{
	_request = new Request(other._server.get_conf());
	_response = new Response(other._server.get_conf());
	return ;
}

http::Client &	http::Client::operator=(Client const & rhs)
{
	(void)rhs;

	return (*this);
}
