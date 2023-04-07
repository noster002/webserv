/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 12:57:55 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/07 11:07:29 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

http::Client::Client(int fd, std::vector<Server> const & servers)\
 : _servers(servers), _socket(fd), _current_message(NULL), _message_queue()
{
	_current_message = new std::pair<Request *, Response *>(std::make_pair(new Request(), new Response()));

	return ;
}

http::Client::~Client(void)
{
	delete _current_message->first;
	delete _current_message->second;
	delete _current_message;

	while (_message_queue.empty() == false)
		clear();

	return ;
}


void			http::Client::connect(struct kevent * ev1, struct kevent * ev2)
{
	_socket.set_kevent(ev1, EVFILT_READ, EV_ADD);
	_socket.set_kevent(ev2, EVFILT_WRITE, EV_ADD | EV_DISABLE);
	_socket.set_non_blocking();

	return ;
}

void			http::Client::disconnect(void)
{
	return ;
}

void			http::Client::read(char const * input, ssize_t bytes, struct kevent * event)
{
	_current_message->first->add_buffer(input, bytes);

	bool		is_interpreted = _current_message->first->parse();

	if (is_interpreted == EXIT_FAILURE)
		return ;
	std::cout << GREEN << "BUILD" << RESET << std::endl;

	_current_message->second->set_server(_get_server(_current_message->first->get_conf()));
	_current_message->second->build(_current_message->first->get_error(), _current_message->first->get_conf());

	_message_queue.push(*_current_message);
	new (_current_message) std::pair<Request *, Response *>(std::make_pair(new Request(), new Response()));

	_socket.set_kevent(event, EVFILT_WRITE, EV_ENABLE);

	return ;
}

std::string		http::Client::write(struct kevent * event)
{
	std::string		output;

	if (_message_queue.empty())
	{
		_socket.set_kevent(event, EVFILT_WRITE, EV_DISABLE);

		return ("");
	}

	if (_message_queue.front().second->get_buffer(output))
		return (output);

	clear();

	_socket.set_kevent(event, EVFILT_WRITE, EV_DISABLE);

	return (output);
}

void			http::Client::clear(void)
{
	delete _message_queue.front().first;
	delete _message_queue.front().second;
	_message_queue.pop();

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
 : _servers(other._servers), _socket(), _current_message(NULL), _message_queue()
{
	return ;
}

http::Client &	http::Client::operator=(Client const & rhs)
{
	(void)rhs;

	return (*this);
}
