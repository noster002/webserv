/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:49 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/15 11:44:43 by nosterme         ###   ########.fr       */
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

http::Server::Server(Server const & other)\
 : _conf(other._conf), _sockets()
{
	for (std::vector<Socket *>::const_iterator it = other._sockets.begin(); it != other._sockets.end(); ++it)
		_sockets.push_back(new Socket(*(*it)));

	return ;
}

http::Server &				http::Server::operator=(Server const & rhs)
{
	clean();

	_conf = rhs._conf;

	for (std::vector<Socket *>::const_iterator it = rhs._sockets.begin(); it != rhs._sockets.end(); ++it)
		_sockets.push_back(new Socket(*(*it)));

	return (*this);
}

http::Server::~Server(void)
{
	clean();

	return ;
}


int			http::Server::get_socket_fd(int index) const
{
	return (_sockets[index]->get_fd());
}

int			http::Server::get_nbr_sockets(void) const
{
	return (_sockets.size());
}

void		http::Server::setup(int kq)
{
	for (std::vector<std::string>::iterator it = _conf.port.begin(); it != _conf.port.end(); ++it)
	{
		Socket	socket;

		try
		{
			set_addr_info(socket, it->c_str());
			socket.create();
			socket.set_non_blocking();
			socket.set_opt(SOL_SOCKET, SO_REUSEADDR);
			socket.bind();
			socket.listen(1024/* max_pending_clients */);
			socket.set_kevent(kq, EVFILT_READ, EV_ADD);

			_sockets.push_back(new Socket(socket));
		}
		catch (http::Socket::Exception const & e)
		{
			std::cerr << e.what() << std::endl;
			if (e.get_level() > e_create)
				socket.clean();
		}
		catch (std::exception const & e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	return ;
}

void		http::Server::clean(void)
{
	for (std::vector<Socket *>::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
		delete *it;

	return ;
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
