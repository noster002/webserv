/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:41:12 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/12 17:02:57 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

http::Request::Request(void)\
 : _buffer(), _error(0), _conf(), _is_complete(false), _is_body(false)
{
	return ;
}

http::Request::~Request(void)
{
	return ;
}


int					http::Request::get_error(void) const
{
	return (_error);
}

t_request const &	http::Request::get_conf(void) const
{
	return (_conf);
}

void				http::Request::add_buffer(char const * input, ssize_t bytes)
{
	for (ssize_t pos = 0; pos < bytes; ++pos)
		_buffer.push_back(input[pos]);
	return ;
}

bool				http::Request::parse(void)
{
	std::string		line;
	std::string		key;
	size_t			pos = 0;

	if (_is_complete)
		return (EXIT_FAILURE);
	if (_is_body == false)
	{
		if (_binary(_buffer))
		{
			_bad_request("HTTP request: binary data detected");
			return (EXIT_SUCCESS);
		}
		if (_section_complete(_buffer))
			_is_body = true;
		else
			return (EXIT_FAILURE);
		_read_first_line(pos);
		while (!_error)
		{
			if (pos != std::string::npos)
				pos += 2;
			line = _get_next_line(_buffer, pos);
			if (_error)
				return (EXIT_SUCCESS);
			if (line.empty())
			{
				_buffer.erase(0, pos + 2);
				break ;
			}
			key = _get_key(line);
			if (!key.empty())
			{
				if (!_conf.header[key].empty())
					_conf.header[key] += ", ";
				_conf.header[key] += _get_value(line);
			}
		}
		if (_error || _process_header_fields())
			return (EXIT_SUCCESS);
	}
	if (_is_body == true)
		_read_body();
	// std::cout << std::endl;
	// std::cout << YELLOW << "_conf.method:\t(" << BLUE << _conf.method << YELLOW << ")" << std::endl;
	// std::cout << YELLOW << "_conf.protocol:\t(" << BLUE << _conf.protocol << YELLOW << ")" << std::endl;
	// std::cout << YELLOW << "_conf.path:\t(" << BLUE << _conf.path << YELLOW << ")" << std::endl;
	// std::cout << YELLOW << "_conf.query:\t(" << BLUE << _conf.query << YELLOW << ")" << std::endl;
	// std::cout << YELLOW << "_conf.host:\t(" << BLUE << _conf.host << YELLOW << ")" << std::endl;
	// std::cout << YELLOW << "_conf.port:\t(" << BLUE << _conf.port << YELLOW << ")" << std::endl;
	// std::cout << YELLOW << "_conf.header:" << std::endl;
	// for (std::map<std::string, std::string>::iterator it = _conf.header.begin(); it != _conf.header.end(); ++it)
	// 	std::cout << YELLOW << "\tname: " << BLUE << it->first << YELLOW << "\n\t\tvalue: " << BLUE << it->second << std::endl;
	// std::cout << YELLOW << "_conf.transfer_encoding:" << std::endl;
	// for (std::vector<std::vector<std::string> >::iterator x = _conf.transfer_encoding.begin(); x != _conf.transfer_encoding.end(); ++x)
	// {
	// 	std::cout << YELLOW << "\tencoding:\t(" << BLUE << *(x->begin()) << YELLOW << ")" << std::endl;
	// 	for (std::vector<std::string>::iterator y = (x->begin() + 1); y != x->end(); ++y)
	// 		std::cout << YELLOW << "\t\tparam:\t(" << BLUE << *y << YELLOW << ")" << std::endl;
	// }
	// std::cout << YELLOW << "_conf.content_length:\t(" << BLUE << _conf.content_length << YELLOW << ")" << std::endl;
	// std::cout << YELLOW << "_conf.content_type:\t(" << BLUE << _conf.content_type.first << YELLOW << ")" << std::endl;
	// for (std::map<std::string, std::string>::iterator it = _conf.content_type.second.begin(); it != _conf.content_type.second.end(); ++it)
	// 	std::cout << YELLOW << "\tname: " << BLUE  << it->first << YELLOW << "\n\t\tvalue: " << BLUE << it->second << std::endl;
	// std::cout << YELLOW << "_conf.body:\n(" << BLUE << _conf.body << YELLOW << ")" << std::endl;
	// std::cout << YELLOW << "_conf.chunks:" << std::endl;
	// for (std::vector<std::string>::iterator it = _conf.chunks.begin(); it != _conf.chunks.end(); ++it)
	// 	std::cout << "\t(" << BLUE << *it << YELLOW << ")" << std::endl;
	// std::cout << YELLOW << "_conf.trailer:" << std::endl;
	// for (std::map<std::string, std::string>::iterator it = _conf.trailer.begin(); it != _conf.trailer.end(); ++it)
	// 	std::cout << YELLOW << "\tname: " << BLUE << it->first << YELLOW << "\n\t\tvalue: " << BLUE << it->second << std::endl;
	// std::cout << RESET << "_is_complete: " << _is_complete << std::endl;

	if (_error || _is_complete)
		return (EXIT_SUCCESS);
	return (EXIT_FAILURE);
}

int					http::Request::_read_first_line(size_t & pos)
{
	size_t			start = 0;
	size_t			end;
	std::string		line;

	end = _buffer.find("\r\n", start);
	line = _buffer.substr(start, end);
	if (line.empty() && end != std::string::npos)
	{
		start += 2;
		end = _buffer.find("\r\n", start);
		line = _buffer.substr(start, end);
	}
	end = line.find_first_of(' ');

	if (end == std::string::npos)
		return (_bad_request("HTTP request header: invalid request line"));
	else if (line.length() > REQUEST_LINE_MAX_LEN)
		return (_URI_too_long("HTTP request header: request line too long"));
	pos = end;

	return (_read_method(line, pos));
}

int					http::Request::_read_method(std::string const & line, size_t & pos)
{
	_conf.method.assign(line, 0, pos);

	if (http::Request::_methods.count(_conf.method) == 0)
		return (_not_implemented("HTTP request header: invalid method"));

	return (_read_path(line, pos));
}

int					http::Request::_read_path(std::string const & line, size_t & pos)
{
	if (pos == line.length() - 1)
		return (_bad_request("HTTP request header: no path / HTTP version"));

	size_t		pos2 = line.find_first_of(' ', ++pos);

	if (pos2 == std::string::npos)
		return (_bad_request("HTTP request header: no HTTP version"));

	_conf.path.assign(line, pos, pos2 - pos);

	pos = pos2;

	return (_read_version(line, pos));
}

int					http::Request::_read_version(std::string const & line, size_t & pos)
{
	size_t	len = std::strlen(" HTTP/1.1");

	if (line.length() == (pos + len) && \
		line.compare(pos, len, " HTTP/1.1") == 0)
	{
		_conf.protocol = line.substr(pos + 1, len);
		pos += len;
		return (0);
	}

	return (_HTTP_version_not_supported("HTTP request header: invalid HTTP version"));
}

std::string			http::Request::_get_next_line(std::string const & str, size_t & pos)
{
	std::string		line;
	size_t			pos2;

	if (pos == std::string::npos)
	{
		_bad_request("HTTP request header: no CRLF");
		return ("");
	}
	pos2 = str.find("\r\n", pos);
	line = str.substr(pos, pos2 - pos);
	pos = pos2;

	return (line);
}

std::string			http::Request::_get_key(std::string line)
{
	size_t		pos = line.find_first_of(':');

	if (pos == 0 || pos == std::string::npos)
	{
		_bad_request("HTTP request header: invalid field name");
		return ("");
	}
	line.erase(pos);
	for (pos = 0; pos < line.length(); ++pos)
	{
		if (!_is_token(line[pos]))
		{
			_bad_request("HTTP request header: invalid field name");
			return ("");
		}
		if (pos == 0 || \
			std::isalpha(static_cast<unsigned char>(line[pos - 1])) == false)
			line[pos] = static_cast<unsigned char>(std::toupper(static_cast<unsigned char>(line[pos])));
		else if (std::isalpha(static_cast<unsigned char>(line[pos - 1])) == true)
			line[pos] = static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(line[pos])));
	}

	return (line);
}

std::string			http::Request::_get_value(std::string line)
{
	size_t		pos = line.find_first_of(':');

	if (pos != std::string::npos)
		pos = line.find_first_not_of(" \t", ++pos);

	line.erase(0, pos);
	pos = line.length();

	if (line.empty())
	{
		_bad_request("HTTP request header: invalid field value");
		return ("");
	}

	while (_is_whitespace(line[pos - 1]))
		--pos;
	line.erase(pos);
	for (pos = 0; pos < line.length(); ++pos)
	{
		if (std::iscntrl(line[pos]) && !_is_whitespace(line[pos]))
		{
			_bad_request("HTTP request header: invalid field value");
			return ("");
		}
	}

	return (line);
}

int					http::Request::_process_header_fields(void)
{
	if (_process_path())
		return (_error);
	if (_process_host())
		return (_error);
	
	_conf.content_length = 0;
	_conf.body = "";

	if (_conf.header.count("Transfer-Encoding") && \
		_conf.header["Transfer-Encoding"].empty() == false)
		_process_transfer_encoding();
	else if (_conf.header.count("Content-Length"))
		_process_content_length();
	if (_conf.header.count("Content-Type"))
		_process_content_type();

	return (_error);
}

int					http::Request::_process_path(void)
{
	size_t		pos;
	size_t		pos2;
	std::string	lower(_conf.path.length(), '\0');

	if (_conf.path.length() == 0)
		return (_bad_request("HTTP request header: no path"));
	for (pos = 0; pos < _conf.path.length(); ++pos)
	{
		if (std::iscntrl(_conf.path[pos]) || _conf.path[pos] == ' ')
			return (_bad_request("HTTP request header: invalid path"));
		lower[pos] = std::tolower(_conf.path[pos]);
	}
	if (lower.compare(0, std::strlen("http://"), "http://") == 0)
	{
		pos2 = std::strlen("http://");

		pos = _conf.path.find_first_of('/', pos2);

		_conf.header["Host"] = _conf.path.substr(pos2, pos);
		_conf.path.erase(0, pos);

		if (pos == std::string::npos)
			_conf.path = "/";
	}

	if (_conf.path[0] != '/')
		return (_bad_request("HTTP request header: invalid path"));

	pos = _conf.path.find_first_of('?');

	if (pos != std::string::npos)
	{
		_conf.query = _conf.path.substr(pos + 1);
		_conf.path.erase(pos);
	}

	return (0);
}

int					http::Request::_process_host(void)
{
	if (_conf.header.count("Host") == 0)
		return (_bad_request("HTTP request header: no host detected"));

	size_t		pos = _conf.header["Host"].find_first_of(':');

	if (pos != std::string::npos)
	{
		std::string		port = _conf.header["Host"].substr(pos + 1);

		for (size_t	i = 0; i < port.length(); ++i)
		{
			if (!std::isdigit(port[i]))
				return (_bad_request("HTTP request header: invalid port"));
		}
		_conf.port = std::atoi(port.c_str());
		_conf.host = _conf.header["Host"].substr(0, pos);
	}
	else
	{
		_conf.port = 80;
		_conf.host = _conf.header["Host"];
	}

	return (0);
}

int					http::Request::_process_transfer_encoding(void)
{
	std::vector<std::string>			codings = _split(_conf.header["Transfer-Encoding"], ",");
	std::vector<std::string>			parameter;
	std::vector<std::string>::iterator	it = codings.begin();
	size_t								pos = 0;
	bool								is_quote = false;

	while (it != codings.end())
	{
		pos = it->find_first_not_of(" \t");

		it->erase(0, pos);
		if (it->empty())
			it = codings.erase(it);
		else
		{
			pos = it->size();
			while (_is_whitespace((*it)[pos - 1]))
				--pos;
			it->erase(pos);

			parameter = _split(*it, ";");
			for (std::vector<std::string>::iterator	ite = parameter.begin(); ite != parameter.end(); ++ite)
			{
				pos = ite->find_first_not_of(" \t");

				if (pos == std::string::npos)
					return (_bad_request("HTTP request header: invalid transfer encoding"));
				ite->erase(0, pos);
				pos = 0;
				while (_is_token((*ite)[pos]))
					++pos;
				while (_is_whitespace((*ite)[pos]))
					++pos;
				if ((ite == parameter.begin() && pos != ite->size()) || \
					(ite != parameter.begin() && (*ite)[pos] != '='))
				{
					return (_bad_request("HTTP request header: invalid transfer coding"));
				}
				else if (ite != parameter.begin() && (*ite)[pos] == '=')
					++pos;
				while (_is_whitespace((*ite)[pos]))
					++pos;
				while (pos < ite->size())
				{
					if ((*ite)[pos] == '"')
					{
						if (is_quote && (*ite)[pos - 1] != '\\')
							is_quote = false;
						else
							is_quote = true;
						++pos;
					}
					else if (_is_token((*ite)[pos]) || is_quote)
						++pos;
					else
						return (_bad_request("HTTP request header: invalid transfer parameter"));
				}
				if (is_quote == true)
					return (_bad_request("HTTP request header: transfer parameter dquote not terminated"));
			}
			_conf.transfer_encoding.push_back(std::vector<std::string>(parameter.begin(), parameter.end()));
			++it;
		}
	}
	if (codings.empty())
		return (_bad_request("HTTP request header: expected transfer encoding"));

	if (_conf.transfer_encoding.back()[0] == "chunked")
		return (0);

	_conf.transfer_encoding.clear();

	return (_bad_request("HTTP request header: no chunked transfer coding"));
}

int					http::Request::_process_content_length(void)
{
	std::vector<std::string>			lengths = _split(_conf.header["Content-Length"], ",");
	std::vector<std::string>::iterator	it = lengths.begin();
	size_t								pos;
	size_t								tmp;
	bool								is_first = true;

	while (it != lengths.end())
	{
		if (it->empty())
			return (_bad_request("HTTP request header: empty content length"));

		for (pos = 0; pos < it->size(); ++pos)
		{
			if (!std::isdigit(static_cast<unsigned char>((*it)[pos])))
				return (_bad_request("HTTP request header: invalid content length"));
		}
		tmp = static_cast<size_t>(std::atol(it->c_str()));
		if (is_first == false && \
			_conf.content_length != tmp)
		{
			return (_bad_request("HTTP request header: different values for content length"));
		}
		_conf.content_length = tmp;
		is_first = false;
		++it;
	}

	return (0);
}

int					http::Request::_process_content_type(void)
{
	std::string							type;
	std::map<std::string, std::string>	parameter;
	std::vector<std::string>			tmp = _split(_conf.header["Content-Type"], ";");
	std::vector<std::string>::iterator	it = tmp.begin();
	std::string							key;
	size_t								pos;
	bool								is_quote = false;

	if (tmp.empty())
		type = "application/octet-stream";
	else
	{
		pos = it->find_first_of(" \t");

		if (pos != std::string::npos)
			it->erase(pos);
		pos = 0;
		while (_is_token((*it)[pos]))
			++pos;
		if ((*it)[pos] != '/')
			return (_bad_request("HTTP request header: invalid media type"));
		++pos;
		while (_is_token((*it)[pos]))
			++pos;
		if (pos != it->length())
			return (_bad_request("HTTP request header: invalid media subtype"));

		type = *it;

		while (++it != tmp.end())
		{
			pos = it->find_first_not_of(" \t");

			if (pos == std::string::npos)
				continue ;
			it->erase(0, pos);
			pos = it->length();
			while (_is_whitespace((*it)[pos - 1]))
				--pos;
			it->erase(pos);
			pos = 0;
			while (_is_token((*it)[pos]))
				++pos;
			if ((*it)[pos] != '=')
				return (_bad_request("HTTP request header: invalid type parameter name"));
			key = it->substr(0, pos);
			it->erase(0, ++pos);
			for (pos = 0; pos < it->length(); ++pos)
			{
				if ((*it)[pos] == '"')
				{
					if (is_quote && (*it)[pos - 1] != '\\')
						is_quote = false;
					else
						is_quote = true;
				}
				else if (!_is_token((*it)[pos]) && !is_quote)
					return (_bad_request("HTTP request header: invalid type parameter value"));
			}
			if (is_quote == true)
				return (_bad_request("HTTP request header: type parameter dquote not terminated"));
			if (parameter.count(key))
				parameter[key] += ", ";
			parameter[key] += *it;
		}
	}

	_conf.content_type = std::make_pair(type, parameter);

	return (0)
;}

int					http::Request::_read_body(void)
{
	size_t		pos = 0;

	if (_conf.transfer_encoding.empty() == false)
	{
		std::string	chunk = _conf.body + _buffer;

		_decode_chunk(chunk, pos);

		if (_error)
			return (_error);
		if (pos != std::string::npos)
		{
			pos -= _conf.body.size();
			_is_complete = true;
		}
		else
			_conf.chunks.clear();
	}
	else if (_conf.content_length)
	{
		size_t		size = _conf.body.size();

		pos = _conf.content_length - size;

		if (pos <= _buffer.size())
			_is_complete = true;
	}
	else
		_is_complete = true;

	_conf.body += _buffer.substr(0, pos);

	_buffer.clear();

	return (0);
}

void				http::Request::_decode_chunk(std::string const & chunk, size_t & pos)
{
	std::string		line = _get_next_line(chunk, pos);
	std::string		key;
	size_t			size = 0;

	if (pos == std::string::npos)
		return ;
	pos += 2;

	_read_chunk_size(line, size);

	while (size > 0 && !_error)
	{
		if (_read_chunk(chunk, size, pos))
			return ;
		line = _get_next_line(chunk, pos);
		if (pos == std::string::npos)
			return ;
		pos += 2;
		_read_chunk_size(line, size);
	}
	while (!_error)
	{
		line = _get_next_line(chunk, pos);
		if (pos == std::string::npos)
			return ;
		pos += 2;
		if (line.empty())
			break ;
		key = _get_key(line);
		if (!key.empty())
		{
			if (!_conf.trailer[key].empty())
				_conf.trailer[key] += ", ";
			_conf.trailer[key] += _get_value(line);
		}
	}

	return ;
}

int					http::Request::_read_chunk_size(std::string const & line, size_t & size)
{
	std::vector<std::string>	extensions = _split(line, ";");
	size_t						pos = 0;
	char *						last_nbr;
	bool						is_quote = false;

	if (extensions.empty())
		return (_bad_request("HTTP request body: CRLF when chunk size expected"));

	while (std::isxdigit(static_cast<unsigned char>(extensions[0][pos])))
		++pos;
	while (_is_whitespace(extensions[0][pos]))
		++pos;
	if (pos != extensions[0].size())
		return (_bad_request("HTTP request body: invalid chunk size"));

	size = static_cast<size_t>(std::strtol(extensions[0].c_str(), &last_nbr, 16));

	for (std::vector<std::string>::iterator	it = (extensions.begin() + 1); it != extensions.end(); ++it)
	{
		pos = it->find_first_not_of(" \t");

		if (pos == std::string::npos)
			return (_bad_request("HTTP request body: empty chunk extension"));
		it->erase(0, pos);
		pos = 0;
		while (_is_token((*it)[pos]))
			++pos;
		while (_is_whitespace((*it)[pos]))
			++pos;
		if (pos < it->size())
		{
			if ((*it)[pos] != '=')
				return (_bad_request("HTTP request body: chunk extensions not separated"));
			++pos;
			while (_is_whitespace((*it)[pos]))
				++pos;
			while (pos < it->size())
			{
				if ((*it)[pos] == '"')
				{
					if (is_quote && (*it)[pos - 1] != '\\')
						is_quote = false;
					else
						is_quote = true;
					++pos;
				}
				else if (_is_token((*it)[pos]))
					++pos;
				else
					return (_bad_request("HTTP request body: invalid chunk extension"));
			}
			if (is_quote == true)
				return (_bad_request("HTTP request body: chunk extension dquote not terminated"));
		}
	}

	return (0);
}

int					http::Request::_read_chunk(std::string const & chunk, size_t size, size_t & pos)
{
	if (pos + size + std::strlen("\r\n") > chunk.size())
	{
		pos = std::string::npos;
		return (1);
	}
	_conf.chunks.push_back(chunk.substr(pos, size));
	pos += size;
	if (chunk.compare(pos, 2, "\r\n"))
		return (_bad_request("HTTP request body: no CRLF after chunk content"));
	pos += 2;
	return (0);
}

int					http::Request::_bad_request(std::string const & error_msg)
{
	std::cerr << error_msg << std::endl;
	_error = 400;
	return (_error);
}

int					http::Request::_URI_too_long(std::string const & error_msg)
{
	std::cerr << error_msg << std::endl;
	_error = 414;
	return (_error);
}

int					http::Request::_not_implemented(std::string const & error_msg)
{
	std::cerr << error_msg << std::endl;
	_error = 501;
	return (_error);
}

int					http::Request::_HTTP_version_not_supported(std::string const & error_msg)
{
	std::cerr << error_msg << std::endl;
	_error = 505;
	return (_error);
}

bool				http::Request::_binary(std::string const & str)
{
	unsigned char	c;
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		c = static_cast<unsigned char>(*it);
		if (!std::isprint(c) && !std::iscntrl(c))
			return (true);
		if (c == '\n')
			break ;
	}
	return (false);
}

bool				http::Request::_section_complete(std::string const & str)
{
	return (str.find("\r\n\r\n") != std::string::npos);
}

std::vector<std::string>	http::Request::_split(std::string const & str, char const * delim)
{
	std::vector<std::string>	split;
	size_t						pos = 0;
	size_t						pos2 = str.find_first_of('"');

	if (pos2 == std::string::npos)
	{
		while (pos < str.size())
		{
			pos2 = str.find(delim, pos);

			if (pos2 == std::string::npos)
			{
				split.push_back(str.substr(pos));
				return (split);
			}
			split.push_back(str.substr(pos, pos2 - pos));
			pos = pos2 + 1;
		}
		return (split);
	}
	size_t						pos3 = pos2 + 1;

	do
	{
		pos = str.find_first_of('"', pos3);
		if (pos == std::string::npos)
			return (split);
		++pos3;
	}
	while (str[pos - 1] == '\\');


	std::vector<std::string>	tmp1 = _split(str.substr(0, pos2), delim);
	std::vector<std::string>	tmp2 = _split(str.substr(++pos), delim);

	for (std::vector<std::string>::iterator it = tmp1.begin(); it != tmp1.end(); ++it)
		split.push_back(*it);
	if (tmp2.empty() == false)
	{
		split.back() += str.substr(pos2, pos - pos2);
		split.back() += tmp2[0];
		for (std::vector<std::string>::iterator it = (tmp2.begin() + 1); it != tmp2.end(); ++it)
			split.push_back(*it);
	}

	return (split);
}

bool				http::Request::_is_token(char c)
{
	return (c == '!' || c == '#' || c == '$' || c == '%' || c == '&' || \
			c == '\'' || c == '*'|| c == '+' || c == '-' || c == '.' || \
			c == '^' || c == '_'|| c == '|' || c == '~' || isalnum(c));
}

bool				http::Request::_is_whitespace(char c)
{
	return (c == ' ' || c == '\t');
}

std::set<std::string>		http::Request::_init_methods(void)
{
	std::set<std::string>	methods;

	methods.insert("GET");
	methods.insert("POST");
	methods.insert("HEAD");
	methods.insert("PUT");
	methods.insert("DELETE");

	return (methods);
}

std::set<std::string>		http::Request::_methods = Request::_init_methods();


// canonical class form

http::Request::Request(Request const & other)
{
	(void)other;

	return ;
}

http::Request &		http::Request::operator=(Request const & rhs)
{
	(void)rhs;

	return (*this);
}
