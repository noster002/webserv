/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:49 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/13 16:27:27 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head/Server.hpp"

// constructor & destructor

http::Server::Server(void)\
 : _conf(), _sockets()
{
	return ;
}

http::Server::Server(params_t const & conf)\
 : _conf(conf), _sockets()
{
	return ;
}

http::Server::~Server(void)
{
	return ;
}


int			http::Server::setup(int kq)
{
	for (std::vector<std::string>::iterator it = _conf.port.begin(); it != _conf.port.end(); ++it)
	{
		try
		{
			Socket	socket;

			set_addr_info(socket, it->c_str());
			socket.create();
			socket.set_non_blocking();
			socket.set_opt(SOL_SOCKET, SO_REUSEADDR);
			socket.bind();
			socket.listen(_max_pending_clients);
			socket.set_kevent(kq, EVFILT_READ, EV_ADD);

			_sockets.push_back(socket);
		}
		catch (std::exception const & e)
		{
			std::cerr << e.what() << std::endl;
			socket.clean();
		}
	}

	return (_sockets.size());
}

void		http::Server::set_addr_info(Socket & socket, char const * port)
{
	struct addrinfo		hints;
	struct protoent *	protocol;

	protocol = ::getprotobyname("ip");

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = protocol->p_proto;
	hints.ai_flags = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;

	socket.get_addr_info(_conf.host.c_str(), port, &hints);

	return ;
}


// canonical class form

http::Server::Server(Server const & other)\
 : _conf(other._conf), _sockets()
{
	return ;
}

http::Server &				http::Server::operator=(Server const & rhs)
{
	_conf = rhs._conf;

	return (*this);
}
