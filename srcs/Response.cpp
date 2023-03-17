/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:43:02 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/17 15:37:35 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

http::Response::Response(params_t const & server)\
 : _server(server), _buffer(), _protocol("HTTP/1.1"), _status(), _header(), _body()
{
	return ;
}

http::Response::~Response(void)
{
	return ;
}


std::string const &	http::Response::get_buffer(void) const
{
	return (_buffer);
}

void				http::Response::build(int error, t_request const & request)
{
	_status = error;

	if (_status)
		_serve_request_error();
	else if (request.method == "GET")
		_serve_get_request(request);
	else if (request.method == "POST")
		_serve_post_request(request);
	else if (request.method == "DELETE")
		_serve_delete_request(request);
	_set_status_line();
	_set_head();
	_set_body();

	return ;
}

void				http::Response::clear(void)
{
	new (this) Response(_server);
	return ;
}

void				http::Response::_serve_get_request(t_request const & request)
{
	std::fstream	file(request.path);

	if (file.is_open() == false)
	{
		_status = 404;
		_serve_file_error();
	}

}

void				http::Response::_serve_post_request(t_request const & request)
{
	(void)request;
}

void				http::Response::_serve_delete_request(t_request const & request)
{
	(void)request;
}

void				http::Response::_serve_request_error(void)
{

}

void				http::Response::_serve_file_error(void)
{

}

void				http::Response::_set_status_line(void)
{
	std::stringstream	status_line;

	status_line << _protocol << ' ' << _status << ' ' << _statuses[_status] << "\r\n";

	_buffer = status_line.str();

	return ;
}

void				http::Response::_set_head(void)
{
	std::stringstream	head;

	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); ++it)
		head << it->first << ": " << it->second << "\r\n";

	head << "\r\n";

	_buffer += head.str();

	return ;
}

void				http::Response::_set_body(void)
{
	_buffer += _body;
	return ;
}

std::map<int, std::string>		http::Response::_init_statuses(void)
{
	std::map<int, std::string>	statuses;

	statuses[100] = "Continue";
	statuses[101] = "Switching Protocols";
	statuses[102] = "Processing";
	statuses[103] = "Early Hints";

	statuses[200] = "OK";
	statuses[201] = "Created";
	statuses[202] = "Accepted";
	statuses[203] = "Non-Authoritative Information";
	statuses[204] = "No Content";
	statuses[205] = "Reset Content";
	statuses[206] = "Partial Content";
	statuses[207] = "Multi-Status";
	statuses[208] = "Already Reported";
	statuses[226] = "IM Used";

	statuses[300] = "Multiple Choices";
	statuses[301] = "Moved Permanently";
	statuses[302] = "Found";
	statuses[303] = "See Other";
	statuses[304] = "Not Modified";
	statuses[305] = "Use Proxy";
	statuses[306] = "Switch Proxy";
	statuses[307] = "Temporary Redirect";
	statuses[308] = "Permanent Redirect";

	statuses[400] = "Bad Request";
	statuses[401] = "Unauthorized";
	statuses[402] = "Payment Required";
	statuses[403] = "Forbidden";
	statuses[404] = "Not Found";
	statuses[405] = "Method Not Allowed";
	statuses[406] = "Not Acceptable";
	statuses[407] = "Proxy Authentication Required";
	statuses[408] = "Request Timeout";
	statuses[409] = "Conflict";
	statuses[410] = "Gone";
	statuses[411] = "Length Required";
	statuses[412] = "Precondition Failed";
	statuses[413] = "Content Too Large";
	statuses[414] = "URI Too Long";
	statuses[415] = "Unsupported Media Type";
	statuses[416] = "Range Not Satisfiable";
	statuses[417] = "Expectation Failed";
	statuses[418] = "I'm a teapot";
	statuses[421] = "Misdirected Request";
	statuses[422] = "Unprocessable Content";
	statuses[423] = "Locked";
	statuses[424] = "Failed Dependency";
	statuses[425] = "Too Early";
	statuses[426] = "Upgrade Required";
	statuses[428] = "Precondition Required";
	statuses[429] = "Too Many Requests";
	statuses[431] = "Request Header Fields Too Large";
	statuses[451] = "Unavailable For Legal Reasons";

	statuses[500] = "Internal Server Error";
	statuses[501] = "Not Implemented";
	statuses[502] = "Bad Gateway";
	statuses[503] = "Service Unavailable";
	statuses[504] = "Gateway Timeout";
	statuses[505] = "HTTP Version Not Supported";
	statuses[506] = "Variant Also Negotiates";
	statuses[507] = "Insufficient Storage";
	statuses[508] = "Loop Detected";
	statuses[510] = "Not Extended";
	statuses[511] = "Network Authentication Required";

	return (statuses);
}

std::map<int, std::string>		http::Response::_statuses = Response::_init_statuses();


// canonical class form

http::Response::Response(void) {}

http::Response::Response(Response const & other)
{
	(void)other;

	return ;
}

http::Response &	http::Response::operator=(Response const & rhs)
{
	(void)rhs;

	return (*this);
}
