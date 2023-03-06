/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:41:12 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/06 17:49:33 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(size_t client_max_body_size)\
 : _error(0), _method(), _version(-1), _path(), _query(), _host(), _port(-1),\
   _header(), _body(), _client_max_body_size(client_max_body_size)
{
	return ;
}

Request::~Request(void)
{
	return ;
}

void			Request::parse(std::string const & input, ssize_t len)
{
	std::string		line;
	size_t			pos = 0;

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
		_header[_get_key(line)] = _get_value(line);
	}
	_body = _read_body(input, pos);
	_process_header_fields();

	return ;
}

void			Request::_read_first_line(std::string const & input)
{
	size_t			pos;
	std::string		line;

	pos = input.find("\r\n");
	line = input.substr(0, pos);
	pos = line.find_first_of(' ');

	if (pos == std::string::npos)
	{
		std::cerr << "HTTP request header: invalid" << std::endl;
		_error = 400;
		throw Exception();
	}

	_read_method(line, pos);

	return ;
}

void			Request::_read_method(std::string const & line, size_t & pos)
{
	_method.assign(line, 0, pos);

	if (Request::_methods.count(_method) == 0)
	{
		std::cerr << "HTTP request header: invalid method" << std::endl;
		_error = 400;
		throw Exception();
	}

	_read_path(line, pos);

	return ;
}

void			Request::_read_path(std::string const & line, size_t & pos)
{
	pos = line.find_first_not_of(' ', pos);

	if (pos == std::string::npos)
	{
		std::cerr << "HTTP request header: no path / HTTP version" << std::endl;
		_error = 400;
		throw Exception();
	}

	size_t		pos2 = line.find_first_of(' ', pos);

	if (pos2 == std::string::npos)
	{
		std::cerr << "HTTP request header: no HTTP version" << std::endl;
		_error = 400;
		throw Exception();
	}

	_path.assign(line, pos, pos2);

	_read_version(line, pos2);

	return ;
}

void			Request::_read_version(std::string const & line, size_t & pos)
{
	pos = line.find_first_not_of(' ', pos);

	if (pos == std::string::npos)
	{
		std::cerr << "HTTP request header: no HTTP version" << std::endl;
		_error = 400;
		throw Exception();
	}

	if ((line.length() != (pos + 8)) || \
		!(line[pos] == 'H' && line[++pos] == 'T' && line[++pos] == 'T' && line[++pos] == 'P'&&\
		  line[++pos] == '/' && line[++pos] == '1' && line[++pos] == '.') || \
		!(line[++pos] == '0' || line[pos] == '1'))
	{
		std::cerr << "HTTP request header: invalid HTTP version" << std::endl;
		_error = 400;
		throw Exception();
	}

	_version = line[pos] - '0';

	return ;
}

std::string		Request::_get_next_line(std::string const & input, size_t & pos)
{
	std::string		line;
	size_t			pos2;

	if (pos == std::string::npos)
	{
		_error = 400;
		throw Exception();
	}
	pos2 = input.find("\r\n", pos);
	line = input.substr(pos, pos2 - pos);
	if (pos == pos2)
		return ("\0");
	pos = pos2;

	return (line);
}

std::string		Request::_get_key(std::string line)
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
		throw Exception();
	}

	return (line);
}

std::string		Request::_get_value(std::string line)
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
		for (pos = 1; pos < line.length(); ++pos)
		{
			if (_is_whitespace(line[pos]) && _is_whitespace(line[pos - 1]))
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
		throw Exception();
	}

	return (line);
}

std::string		Request::_read_body(std::string input, size_t pos)
{
	input.erase(0, pos);

	if (input.empty())
		return ("");
	else if (input.size() > _client_max_body_size)
	{
		_error = 400;
		throw Exception();
	}

	return (input);
}

void			Request::_process_header_fields(void)
{
	_process_path();
	_process_host();
	return ;
}

void			Request::_process_path(void)
{
	size_t		pos = _path.find_first_of('?');

	if (pos != std::string::npos)
	{
		_query = _path.substr(pos + 1);
		_path.erase(pos);
	}

	return ;
}

void			Request::_process_host(void)
{
	if (_version == 1 && _header.count("Host") == 0)
	{
		std::cerr << "HTTP request header: no host detected" << std::endl;
		_error = 400;
		throw Exception();
	}
	else if (_header.count("Host") == 1)
	{
		size_t		pos = _header["Host"].find_first_of(':');

		if (pos != std::string::npos)
		{
			std::string		port = _header["Host"].substr(pos + 1);

			for (int	i = 0; i < port.length(); ++i)
			{
				if (!isdigit(port[i]))
				{
					std::cerr << "HTTP request header: invalid port" << std::endl;
					_error = 400;
					throw Exception();
				}
			}
			_port = atoi(port.c_str());
			_header["Host"].erase(pos);
		}
		_host = _header["Host"];
		_header.erase("Host");
	}

	return ;
}

bool			Request::_is_token(char c) const
{
	return (c == '!' || c == '#' || c == '$' || c == '%' || c == '&' || \
			c == '\'' || c == '*'|| c == '+' || c == '-' || c == '.' || \
			c == '^' || c == '_'|| c == '|' || c == '~' || isalnum(c));
}

bool			Request::_is_whitespace(char c) const
{
	return (c == ' ' || c == '\t');
}

std::set<std::string>		Request::_init_methods(void)
{
	std::set<std::string>	methods;

	methods.insert("GET");
	methods.insert("POST");
	methods.insert("DELETE");

	return (methods);
}

std::set<std::string>		Request::_methods = Request::_init_methods();


// canonical class form

Request::Request(void)
{
	return ;
}

Request::Request(Request const & other)
{
	(void)other;

	return ;
}

Request &		Request::operator=(Request const & rhs)
{
	(void)rhs;

	return (*this);
}


// exceptions

char const *	Request::Exception::what(void) const throw()
{
	return ("Bad Request");
}
