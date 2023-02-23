/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:43:02 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/23 14:38:27 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void)
{
	_initstatus();

	return ;
}

Response::~Response(void)
{
	return ;
}

void		Response::build(Request const & request, std::string & output)
{
	return ;
}

void		Response::_initstatus(void)
{
	_status[100] = "Continue";
	_status[101] = "Switching Protocols";
	_status[102] = "Processing";
	_status[103] = "Early Hints";

	_status[200] = "OK";
	_status[201] = "Created";
	_status[202] = "Accepted";
	_status[203] = "Non-Authoritative Information";
	_status[204] = "No Content";
	_status[205] = "Reset Content";
	_status[206] = "Partial Content";
	_status[207] = "Multi-Status";
	_status[208] = "Already Reported";
	_status[226] = "IM Used";

	_status[300] = "Multiple Choices";
	_status[301] = "Moved Permanently";
	_status[302] = "Found";
	_status[303] = "See Other";
	_status[304] = "Not Modified";
	_status[305] = "Use Proxy";
	_status[306] = "Switch Proxy";
	_status[307] = "Temporary Redirect";
	_status[308] = "Permanent Redirect";

	_status[400] = "Bad Request";
	_status[401] = "Unauthorized";
	_status[402] = "Payment Required";
	_status[403] = "Forbidden";
	_status[404] = "Not Found";
	_status[405] = "Method Not Allowed";
	_status[406] = "Not Acceptable";
	_status[407] = "Proxy Authentication Required";
	_status[408] = "Request Timeout";
	_status[409] = "Conflict";
	_status[410] = "Gone";
	_status[411] = "Length Required";
	_status[412] = "Precondition Failed";
	_status[413] = "Payload Too Large";
	_status[414] = "URI Too Long";
	_status[415] = "Unsupported Media Type";
	_status[416] = "Range Not Satisfiable";
	_status[417] = "Expectation Failed";
	_status[418] = "I'm a teapot";
	_status[421] = "Misdirected Request";
	_status[422] = "Unprocessable Entity";
	_status[423] = "Locked";
	_status[424] = "Failed Dependency";
	_status[425] = "Too Early";
	_status[426] = "Upgrade Required";
	_status[428] = "Precondition Required";
	_status[429] = "Too Many Requests";
	_status[431] = "Request Header Fields Too Large";
	_status[451] = "Unavailable For Legal Reasons";

	_status[500] = "Internal Server Error";
	_status[501] = "Not Implemented";
	_status[502] = "Bad Gateway";
	_status[503] = "Service Unavailable";
	_status[504] = "Gateway Timeout";
	_status[505] = "HTTP Version Not Supported";
	_status[506] = "Variant Also Negotiates";
	_status[507] = "Insufficient Storage";
	_status[508] = "Loop Detected";
	_status[510] = "Not Extended";
	_status[511] = "Network Authentication Required";
}


// canonical class form

Response::Response(Response const & other)
{
	(void)other;

	return ;
}

Response &	Response::operator=(Response const & rhs)
{
	(void)rhs;

	return (*this);
}
