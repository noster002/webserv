/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:41:12 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/02 14:43:46 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void)\
 : _method(), _version(-1), _path(), _query(), _host(), _port(-1),\
   _header(), _body()
{
	_init_methods();

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

	if (_read_first_line(input))
		return ;
	line = _get_next_line(input, pos);
	while (!line.empty())
	{
		if (pos != std::string::npos)
			++pos;
		line = _get_next_line(input, pos);
		if (line[0] != '\n')
			_header[_get_key(line)] = _get_value(line);
	}

	return ;
}

std::string		Request::_get_next_line(std::string const & input, size_t & pos)
{
	std::string		line;
	size_t			pos2;

	if (pos == std::string::npos)
		return ("\n");
	pos2 = input.find_first_of('\n', pos);
	line = input.substr(pos, pos2);
	pos = pos2;

	return (line);
}

std::string		Request::_get_key(std::string const & line)
{
	size_t		pos = line.find_first_of(':');

	if (pos == std::string::npos)
	{
		std::cout << "invalid field name syntax" << std::endl;
		return ("\n");
	}

	return (line.substr(0, pos));
}

std::string		Request::_get_value(std::string const & line)
{
	size_t		pos;
	size_t		pos2;
	std::string	value;

	pos = line.find_first_of(':');
	if (pos != std::string::npos)
		pos = line.find_first_not_of(" \t", ++pos);

	if (pos != std::string::npos)
	{
		do
		{
			pos2 = line.find_first_of(" \t\n", pos);
			value += line.substr(pos, pos2);
			pos = pos2;
			pos2 = line.find_first_not_of(" \t", pos);
		}
		while (pos2 != std::string::npos && line[pos2] != '\n');
	}

	if (pos == std::string::npos || pos2 == std::string::npos)
	{
		std::cout << "invalid field value syntax" << std::endl;
		return ("\n");
	}

	return (value);
}

int				Request::_read_first_line(std::string const & input)
{
	size_t			pos;
	std::string		line;

	pos = input.find_first_of('\n');
	line = input.substr(0, pos);
	pos = line.find_first_of(' ');

	if (pos == std::string::npos)
	{
		std::cerr << "invalid HTTP request" << std::endl;
		return (400);
	}

	return (_read_method(line, pos));
}

int				Request::_read_method(std::string const & line, size_t & pos)
{
	_method.assign(line, 0, pos);

	if (Request::_methods.count(_method) == 0)
	{
		std::cerr << "invalid method" << std::endl;
		return (400);
	}

	return (_read_path(line, pos));
}

int				Request::_read_path(std::string const & line, size_t & pos)
{
	pos = line.find_first_not_of(' ', pos);

	if (pos == std::string::npos)
	{
		std::cerr << "no path / HTTP version" << std::endl;
		return (400);
	}

	size_t		pos2 = line.find_first_of(' ', pos);

	if (pos2 == std::string::npos)
	{
		std::cerr << "no HTTP version" << std::endl;
		return (400);
	}

	_path.assign(line, pos, pos2);

	return (_read_version(line, pos2));
}

int				Request::_read_version(std::string const & line, size_t & pos)
{
	pos = line.find_first_not_of(' ', pos);

	if (pos == std::string::npos)
	{
		std::cerr << "no HTTP version" << std::endl;
		return (400);
	}

	if ((line.length() != (pos + 8)) || \
		!(line[pos] == 'H' && line[++pos] == 'T' && line[++pos] == 'T' && line[++pos] == 'P'&&\
		  line[++pos] == '/' && line[++pos] == '1' && line[++pos] == '.') || \
		!(line[++pos] == '0' || line[pos] == '1'))
	{
		std::cerr << "invalid HTTP version" << std::endl;
		return (400);
	}

	_version = line[pos] - '0';


	std::cout << "good config" << std::endl;

	return (0);
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
