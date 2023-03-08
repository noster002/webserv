/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:52 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/08 17:59:32 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConf.hpp"

// constructor & destructor

web::ServerConf::ServerConf(char const * filename): servers()
 {
	this->_file = filename;
	this->validation = true;
	Helpers::parse_file(this, filename);
 }

web::ServerConf::~ServerConf(void) {}

// member functions

int				web::ServerConf::parse(void)
{
	return (0);
}

void			web::ServerConf::run(void)
{
	for (size_t	i = 0; i < servers.size(); ++i)
	{
		Server	server(servers[i]);

		server.run();
	}

	return ;
}

// canonical class form

web::ServerConf::ServerConf(void) {}

web::ServerConf::ServerConf(ServerConf const & other) : _file(other._file) {}

web::ServerConf &	web::ServerConf::operator=(ServerConf const & rhs)
{
	this->_file = rhs._file;
	return (*this);
}

bool			web::ServerConf::getValidation() const { return this->validation; }

void			web::ServerConf::setValidation(bool status)
{
	this->validation = status;
}
