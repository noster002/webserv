/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fakouyat <fakouyat@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:52 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/02 14:44:57 by fakouyat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../head/ServerConf.hpp"

// constructor & destructor

ServerConf::ServerConf(char const * filename): servers()
 {
	this->_file = filename;
	this->validation = true;
	Helpers::parse_file(this, filename);
 }

ServerConf::~ServerConf(void) {}

// member functions

int				ServerConf::parse(void)
{
	return (0);
}

// canonical class form

ServerConf::ServerConf(void) {}

ServerConf::ServerConf(ServerConf const & other) : _file(other._file) {}

ServerConf &	ServerConf::operator=(ServerConf const & rhs)
{
	this->_file = rhs._file;
	return (*this);
}

bool			ServerConf::getValidation() const { return this->validation; }

void			ServerConf::setValidation(bool status)
{
	this->validation = status;
}