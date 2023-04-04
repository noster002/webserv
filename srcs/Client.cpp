/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 12:57:55 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/04 11:23:17 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

http::Client::Client(int fd, std::vector<Server> const & servers)\
 : _servers(servers), _socket(fd), \
   _request(NULL), \
   _response(NULL)
{
	_request = new Request();
	_response = new Response();
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
	}
	catch (std::exception const & e)
	{
		std::cerr << e.what() << std::endl;
	}
	try
	{
		_socket.set_kevent(kq, EVFILT_WRITE, EV_DELETE);
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

	int		is_interpreted = _request->parse();

	_response->set_server(_get_server(_request->get_conf()));

	if (is_interpreted == EXIT_FAILURE)
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
	std::string		output;

	if (_response->get_buffer(output))
		return (output);

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
	new (_request) Request();
	new (_response) Response();

	return ;
}

params_t const &	http::Client::_get_server(t_request const & request)
{
	for (std::vector<Server>::const_iterator server = _servers.begin(); server != _servers.end(); ++server)
	{
		for (size_t cnt = 0; cnt < server->get_conf().s_names.size(); ++cnt)
		{
			if (server->get_conf().s_names[cnt] == request.host)
				return (server->get_conf());
		}
	}
	return (_servers[0].get_conf());
}


// canonical class form

http::Client::Client(Client const & other)\
 : _servers(other._servers), _socket(), \
   _request(NULL), \
   _response(NULL)
{
	_request = new Request();
	_response = new Response();
	return ;
}

http::Client &	http::Client::operator=(Client const & rhs)
{
	(void)rhs;

	return (*this);
}
