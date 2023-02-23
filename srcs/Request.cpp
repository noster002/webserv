/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:41:12 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/23 12:49:44 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void)
{
	return ;
}

Request::~Request(void)
{
	return ;
}

void		Request::parse(std::string const & input, ssize_t len)
{
	return ;
}


// canonical class form

Request::Request(Request const & other)
{
	(void)other;

	return ;
}

Request &	Request::operator=(Request const & rhs)
{
	(void)rhs;

	return (*this);
}
