/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:41:12 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/09 14:24:30 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

web::Request::Request(size_t client_max_body_size)\
 : _error(0), _method(), _version(-1), _path(), _query(), _host(), _port(-1),\
   _header(), _body(), _client_max_body_size(client_max_body_size)
{
	return ;
}

web::Request::~Request(void)
{
	return ;
}

void			web::Request::parse(std::string const & input, ssize_t len)
{
	std::string		line;
	std::string		key;
	size_t			pos = 0;

	(void)len;
	_read_first_line(input);
	line = _get_next_line(input, pos);
	while (!line.empty())
	{
		if (pos != std::string::npos)
			pos += 2;
		line = _get_next_line(input, pos);
		if (line[0] == '\0')
		{
			pos += 2;
			break ;
		}
		key = _get_key(line);
		if (!_header[key].empty())
			_header[key] += ", ";
		_header[key] += _get_value(line);
	}
	_body = _read_body(input, pos);
	_process_header_fields();
	std::cout << "_host: " << _host << std::endl;
	std::cout << "_port: " << _port << std::endl;
	std::cout << "_method: " << _method << std::endl;
	std::cout << "_version: " << _version << std::endl;
	std::cout << "_path: " << _path << std::endl;
	std::cout << "_query: " << _query << std::endl;
	std::cout << "_body: " << _body << std::endl;
	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); ++it)
		std::cout << "it->first: " << it->first << " it->second: " << it->second << std::endl;

	return ;
}

void			web::Request::_read_first_line(std::string const & input)
{
	size_t			start = 0;
	size_t			end;
	std::string		line;

	end = input.find("\r\n", start);
	line = input.substr(start, end);
	while (line.empty() && end != std::string::npos && start <= 16)
	{
		start  += 2;
		end = input.find("\r\n", start);
		line = input.substr(start, end);
	}
	end = line.find_first_of(' ');

	if (end == std::string::npos)
	{
		std::cerr << "HTTP request header: invalid" << std::endl;
		_error = 400;
		throw BadRequestException();
	}
	else if (line.length() > REQUEST_LINE_MAX_LEN)
	{
		std::cerr << "HTTP request header: invalid" << std::endl;
		_error = 414;
		throw URITooLongException();
	}

	_read_method(line, end);

	return ;
}

void			web::Request::_read_method(std::string const & line, size_t & pos)
{
	_method.assign(line, 0, pos);

	if (web::Request::_methods.count(_method) == 0)
	{
		std::cerr << "HTTP request header: invalid method" << std::endl;
		_error = 501;
		throw NotImplementedException();
	}

	_read_path(line, pos);

	return ;
}

void			web::Request::_read_path(std::string const & line, size_t & pos)
{
	if (pos == line.length() - 1)
	{
		std::cerr << "HTTP request header: no path / HTTP version" << std::endl;
		_error = 400;
		throw BadRequestException();
	}

	size_t		pos2 = line.find_first_of(' ', ++pos);

	if (pos2 == std::string::npos)
	{
		std::cerr << "HTTP request header: no HTTP version" << std::endl;
		_error = 400;
		throw BadRequestException();
	}

	_path.assign(line, pos, pos2 - pos);

	_read_version(line, pos2);

	return ;
}

void			web::Request::_read_version(std::string const & line, size_t & pos)
{
	size_t	len = std::strlen(" HTTP/1.");

	if (line.length() == (pos + len + 1) && \
		(line.compare(pos, len, " HTTP/1.") == 0 && \
		(line[pos + len] == '0' || line[pos + len] == '1')))
	{
		_version = line[pos + len] - '0';
		return ;
	}

	std::cerr << "HTTP request header: invalid HTTP version" << std::endl;
	_error = 505;
	throw HTTPVersionNotSupportedException();
}

std::string		web::Request::_get_next_line(std::string const & input, size_t & pos)
{
	std::string		line;
	size_t			pos2;

	if (pos == std::string::npos)
	{
		_error = 400;
		throw BadRequestException();
	}
	pos2 = input.find("\r\n", pos);
	line = input.substr(pos, pos2 - pos);
	if (pos == pos2)
		return ("\0");
	pos = pos2;

	return (line);
}

std::string		web::Request::_get_key(std::string line)
{
	size_t		pos = line.find_first_of(':');

	if (pos != std::string::npos)
	{
		line.erase(pos);
		for (pos = 0; pos < line.length(); ++pos)
		{
			if (!_is_token(line[pos]))
			{
				pos = std::string::npos;
				break ;
			}
		}
	}

	if (pos == std::string::npos)
	{
		std::cout << "HTTP request header: invalid field name" << std::endl;
		_error = 400;
		throw BadRequestException();
	}

	return (line);
}

std::string		web::Request::_get_value(std::string line)
{
	size_t		pos = line.find_first_of(':');

	if (pos != std::string::npos)
		pos = line.find_first_not_of(" \t", ++pos);

	line.erase(0, pos);
	pos = line.length();

	if (pos > 0)
	{
		while (_is_whitespace(line[pos - 1]))
			--pos;
		line.erase(pos);
		for (pos = 0; pos < line.length(); ++pos)
		{
			if (std::iscntrl(line[pos]) && !_is_whitespace(line[pos]))
			{
				pos = 0;
				break ;
			}
		}
	}

	if (pos == 0)
	{
		std::cerr << "HTTP request header: invalid field value" << std::endl;
		_error = 400;
		throw BadRequestException();
	}

	return (line);
}

std::string		web::Request::_read_body(std::string input, size_t pos)
{
	input.erase(0, pos);

	if (_header.count("Transfer-Encoding") == 0 && \
		_header.count("Content-Length") == 0)
		return ("");
	else if (input.size() > _client_max_body_size)
	{
		_error = 413;
		throw ContentTooLargeException();
	}

	if (_header.count("Transfer-Encoding"))
	{
		
	}
	else if (_header.count("Content-Length"))
	{
		
	}

	return (input);
}

void			web::Request::_process_header_fields(void)
{
	_process_path();
	_process_host();
	return ;
}

void			web::Request::_process_path(void)
{
	size_t		pos;
	std::string	lower(_path.length(), '\0');


	for (pos = 0; pos < _path.length(); ++pos)
	{
		if (std::iscntrl(_path[pos]) || _path[pos] == ' ')
		{
			_path.clear();
			break ;
		}
		lower[pos] = std::tolower(_path[pos]);
	}
	if (lower.compare(0, std::strlen("/"), "/") && \
		lower.compare(0, std::strlen("http://"), "http://") && \
		lower.compare(0, std::strlen("https://"), "https://"))
	{
		_path.clear();
	}

	if (_path.empty())
	{
		std::cerr << "HTTP request header: invalid path" << std::endl;
		_error = 400;
		throw BadRequestException();
	}

	pos = _path.find_first_of('?');

	if (pos != std::string::npos)
	{
		_query = _path.substr(pos + 1);
		_path.erase(pos);
	}

	return ;
}

void			web::Request::_process_host(void)
{
	if (_version == 1 && _header.count("Host") == 0)
	{
		std::cerr << "HTTP request header: no host detected" << std::endl;
		_error = 400;
		throw BadRequestException();
	}
	else if (_header.count("Host") == 1)
	{
		size_t		pos = _header["Host"].find_first_of(':');

		if (pos != std::string::npos)
		{
			std::string		port = _header["Host"].substr(pos + 1);

			for (size_t	i = 0; i < port.length(); ++i)
			{
				if (!std::isdigit(port[i]))
				{
					std::cerr << "HTTP request header: invalid port" << std::endl;
					_error = 400;
					throw BadRequestException();
				}
			}
			_port = std::atoi(port.c_str());
			_header["Host"].erase(pos);
		}
		_host = _header["Host"];
		_header.erase("Host");
	}

	return ;
}

bool			web::Request::_is_token(char c) const
{
	return (c == '!' || c == '#' || c == '$' || c == '%' || c == '&' || \
			c == '\'' || c == '*'|| c == '+' || c == '-' || c == '.' || \
			c == '^' || c == '_'|| c == '|' || c == '~' || isalnum(c));
}

bool			web::Request::_is_whitespace(char c) const
{
	return (c == ' ' || c == '\t');
}

std::set<std::string>		web::Request::_init_methods(void)
{
	std::set<std::string>	methods;

	methods.insert("GET");
	methods.insert("POST");
	methods.insert("DELETE");

	return (methods);
}

std::set<std::string>		web::Request::_methods = Request::_init_methods();


// canonical class form

web::Request::Request(void)
{
	return ;
}

web::Request::Request(Request const & other)
{
	(void)other;

	return ;
}

web::Request &		web::Request::operator=(Request const & rhs)
{
	(void)rhs;

	return (*this);
}


// exceptions

char const *	web::Request::BadRequestException::what(void) const throw()
{
	return ("Bad Request");
}

char const *	web::Request::URITooLongException::what(void) const throw()
{
	return ("URI Too Long");
}

char const *	web::Request::NotImplementedException::what(void) const throw()
{
	return ("Not Implemented");
}

char const *	web::Request::ContentTooLargeException::what(void) const throw()
{
	return ("Content Too Large");
}

char const *	web::Request::HTTPVersionNotSupportedException::what(void) const throw()
{
	return ("HTTP Version Not Supported");
}
