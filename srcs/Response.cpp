/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:43:02 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/12 17:00:43 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

http::Response::Response(void)\
 : _server(), _buffer(), _protocol("HTTP/1.1"), _status(), _header(), _body(),\
   _route(), _is_cgi(false), _is_upload(false), _is_autoindex(false),\
   _is_chunk(false), _first_chunk(false)
{
	return ;
}

http::Response::~Response(void)
{
	return ;
}


bool				http::Response::get_buffer(std::string & buffer)
{
	size_t			pos;

	if (_first_chunk)
	{
		pos = _buffer.find("\r\n\r\n");

		if (pos == std::string::npos)
			return (false);
		pos += std::strlen("\r\n\r\n");
		buffer = _buffer.substr(0, pos);
		_buffer.erase(0, pos);
		_first_chunk = false;
	}
	else if (_is_chunk)
	{
		std::stringstream	size;
		pos = 0x10000 - (5 + (2 * (std::strlen("\r\n"))));

		if (_buffer.size() < pos)
		{
			size << std::hex << _buffer.size();
			buffer = size.str() + "\r\n";
			buffer += _buffer + "\r\n";
			buffer += "0\r\n\r\n";
			_buffer.clear();
			_is_chunk = false;
		}
		else
		{
			size << std::hex << pos;
			buffer = size.str() + "\r\n";
			buffer += _buffer.substr(0, pos) + "\r\n";
			_buffer.erase(0, pos);
		}
	}
	else
		buffer = _buffer;

	return (_is_chunk);
}

void				http::Response::set_server(params_t const & server)
{
	_server = server;
	return ;
}

void				http::Response::build(int error, t_request const & request)
{
	std::string		path;

	_status = error;

	if (!_status)
		_get_path(request, path);
	{
		// std::ofstream	file("logfile.txt");

		// if (file.is_open())
		// 	std::cout << "hellO" << std::endl;
		// file << "request.method:\t(" << request.method << ")" << std::endl;
		// file << "request.protocol:\t(" << request.protocol << ")" << std::endl;
		// file << "request.path:\t(" << request.path << ")" << std::endl;
		// file << "request.query:\t(" << request.query << ")" << std::endl;
		// file << "request.host:\t(" << request.host << ")" << std::endl;
		// file << "request.port:\t(" << request.port << ")" << std::endl;
		// file << "request.header:" << std::endl;
		// for (std::map<std::string, std::string>::const_iterator it = request.header.begin(); it != request.header.end(); ++it)
		// 	file << "\tname: " << it->first << "\n\t\tvalue: " << it->second << std::endl;
		// file << "request.transfer_encoding:" << std::endl;
		// for (std::vector<std::vector<std::string> >::const_iterator x = request.transfer_encoding.begin(); x != request.transfer_encoding.end(); ++x)
		// {
		// 	file << "\tencoding:\t(" << *(x->begin()) << ")" << std::endl;
		// 	for (std::vector<std::string>::const_iterator y = (x->begin() + 1); y != x->end(); ++y)
		// 		file << "\t\tparam:\t(" << *y << ")" << std::endl;
		// }
		// file << "request.content_length:\t(" << request.content_length << ")" << std::endl;
		// file << "request.body:\n(" << request.body << ")" << std::endl;
		// file << "request.chunks:" << std::endl;
		// for (std::vector<std::string>::const_iterator it = request.chunks.begin(); it != request.chunks.end(); ++it)
		// 	file << "\t(" << *it << ")" << std::endl;
		// file << "request.trailer:" << std::endl;
		// for (std::map<std::string, std::string>::const_iterator it = request.trailer.begin(); it != request.trailer.end(); ++it)
		// 	file << "\tname: " << it->first << "\n\t\tvalue: " << it->second << std::endl;
		// file.close();
	}

	if (!_status)
	{
		if (_is_cgi)
			_cgi_handler(request, path);
		else if (request.method == "GET" || request.method == "HEAD")
			_serve_get_request(request, path);
		else if (request.method == "POST" || request.method == "PUT")
			_serve_post_request(request, path);
		else if (request.method == "DELETE")
			_serve_delete_request(request, path);
	}
	if (_status >= 400)
		_serve_error();
	else if (_is_cgi)
		_set_cgi(path);
	_set_status_line();
	if (_body.size() > 0x10000)
		_chunk();
	_set_head();
	if (request.method != "HEAD")
		_set_body();

	return ;
}

int					http::Response::_serve_get_request(t_request const & request, std::string & path)
{
	if (_is_autoindex)
		return (_directory_listing(request, path));
	if (!_is_file(path, F_OK))
		return (_not_found());
	else if (!_is_file(path, R_OK))
		return (_forbidden());

	std::ifstream		file(path.c_str());
	std::stringstream	body;

	if (file.is_open() == false)
		return (_internal_server_error());

	_set_content_type(path);

	body << file.rdbuf();
	_body = body.str();

	_set_content_length(_body.size());

	file.close();

	return (_OK());
}

int					http::Response::_serve_post_request(t_request const & request, std::string & path)
{
	std::string	file_name, target, file_content, file_type;
	size_t 		cursor = 0, ending, start;
	std::string	created;

	if (!request.transfer_encoding.empty())
	{
		for (size_t i = 0; i < request.chunks.size(); ++i)
		{
			if (request.chunks[i].size() > _route.client_max_body_size)
				return (_content_too_large());
			file_content += request.chunks[i];
		}
		target = path;
		if (target.back() == '/')
			return (_no_content());
		file_type = "application/octet-stream";
		if (!_is_file(target, F_OK))
			created = target;
		else if ((_is_upload == false) ||\
				 (_is_dir(target)) ||\
				 (!_is_file(target, R_OK | W_OK)))
			return (_forbidden());

		_upload(target, file_type, file_content);
	}
	else
	{
		if (request.body.size() > _route.client_max_body_size)
			return (_content_too_large());
		else if (_is_upload == false)
			return (_forbidden());
		if (request.content_type.first.find("multipart") == 0 && request.content_length != 0)
		{
			if (!request.content_type.second.count("boundary") || request.content_type.second.at("boundary").empty())
				return (_bad_request());
			std::string		dir = path.substr(0, path.find_last_of('/') + 1);
			std::string		form_end = "--" + request.content_type.second.at("boundary") + "--\r\n";

			cursor = request.body.find("--" + request.content_type.second.at("boundary"), 0);
			if (cursor != 0)
				return (_bad_request());
			do
			{
				_get_filename_to_upload(request, cursor, file_name);
				_get_file_type(request, cursor, file_type);
				cursor = request.body.find("\r\n\r\n", cursor);
				if (cursor == std::string::npos)
					return (_bad_request());
				start = cursor + std::strlen("\r\n\r\n");
				cursor = request.body.find("\r\n--" + request.content_type.second.at("boundary"), start);
				if (cursor == std::string::npos)
					return (_bad_request());
				ending = cursor;
				file_content = request.body.substr(start, ending - start);
				cursor += std::strlen("\r\n");
				if (file_name.empty() == false)
				{
					target = dir + file_name;
					if (!_is_file(target, F_OK) && created.empty())
						created = target;
					if (!_is_dir(target))
						_upload(target, file_type, file_content);
				}
			}
			while (cursor != request.body.find(form_end));
			if (cursor + form_end.length() != request.content_length)
				return (_bad_request());
		}
		else
		{
			file_content = request.body;
			target = path;
			if (target.back() == '/')
				return (_no_content());
			file_type = "application/octet-stream";
			if (!_is_file(target, F_OK))
				created = target;
			else if ((_is_upload == false) ||\
					(_is_dir(target)) ||\
					(!_is_file(target, R_OK | W_OK)))
				return (_forbidden());

			_upload(target, file_type, file_content);
		}
	}

	if (created.empty() == false)
		_header["Location"] = created;

	if (_is_autoindex)
		return (_directory_listing(request, path));

	std::ifstream	file(path.c_str());
	std::string		buffer;

	if (file.is_open() == false)
		return (_internal_server_error());

	while (std::getline(file, buffer))
		_body += buffer;
	file.close();

	_set_content_type(path);
	_set_content_length(_body.size());

	if (created.empty() == false)
		return (_created());
	return (_OK());
}

int					http::Response::_serve_delete_request(t_request const & request, std::string const & path)
{
	if (_is_autoindex)
		return (_directory_listing(request, path));

	if (std::remove(path.c_str()) == 0)
		return (_no_content());
	return (_not_found());
}

int					http::Response::_get_path(t_request const & request, std::string & path)
{
	std::string		match = request.path;

	if (match.back() != '/')
		match += '/';

	size_t			pos = match.size();
	bool			is_dir;
	int				level = 0;

	do
	{
		is_dir = (match.back() == '/');
		if (_server.routes.count(match))
		{
			_route = _server.routes[match];

			if (_route.method.count(request.method) == 0)
				level += 100;
			else if (_route.redirect.empty() == false)
				level += 10;
			else if (_route.root.empty())
				level += 1;

			if (((level % 100) == 0))
			{
				if (_route.upload && (request.method == "POST" || request.method == "PUT"))
					_is_upload = true;
				if ((is_dir && _route.root.back() != '/') || pos > request.path.length())
					--pos;
				else if (!is_dir && _route.root.back() == '/' && pos < request.path.length())
					++pos;
				path = _route.root + request.path.substr(pos);
				if (_is_dir(path))
				{
					if (path.back() != '/')
							path += '/';
					if (_route.index.empty() == false)
						path += _route.index;
					else if (_route.directory_listing)
						_is_autoindex = true;
				}
				size_t	ext = path.find_last_of('.');

				if (ext != std::string::npos && \
					_route.cgi_methods.count(request.method) && \
					_route.cgi_pass.empty() == false && \
					_route.cgi_ext == path.substr(ext))
				{
					if (level >= 100)
						level -= 100;
					_is_cgi = true;
				}
			}

			if (level >= 100)
				return (_method_not_allowed());
			else if (level >= 10)
				return (_permanent_redirect());
			else if (level == 1)
				return (_gone());
			return (_status);
		}
		if (is_dir)
			--pos;
		else
		{
			pos = match.find_last_of('/');
			if (pos == std::string::npos)
				break ;
			++pos;
		}
		match.erase(pos);
	}
	while (pos != 0);

	return (_not_found());
}

int					http::Response::_directory_listing(t_request const & request, std::string const & path)
{
	DIR *	dir = opendir(path.c_str());

	if (dir == NULL)
		return (_not_found());
	std::stringstream	body;
	std::string			entryname;

	body <<"\
<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
	<title>" << request.path << "</title>\n\
</head>\n\
<body>\n\
	<h1>Index of " << request.path << "</h1>\n\
	<hr>\n\
	<p>\n";

	for (struct dirent * entry = readdir(dir); entry; entry = readdir(dir))
	{
		if (entry->d_type == DT_DIR || entry->d_type == DT_REG)
		{
			entryname = entry->d_name;
			if (request.path.back() != '/')
				entryname.insert(entryname.begin(), '/');

			body << "\t\t<p><a href=\"http://" << request.host << ":" << request.port
				 << request.path << entryname;
			if (entry->d_type == DT_DIR)
				body << "/\">" << entryname << "/</a></p>\n";
			else
				body << "\">" << entryname << "</a></p>\n";
		}
	}

	body <<"\
	</p>\n\
	<hr>\n\
</body>\n\
</html>\n";
	closedir(dir);

	_header["Content-Type"] = "text/html";
	_body = body.str();

	_set_content_length(_body.size());

	return (_OK());
}

void				http::Response::_serve_error(void)
{
	std::string		name;

	if (_server.err_pages.count(_status))
		name = _server.err_pages[_status];
	else
		name = _default_err_pages[_status];

	if (_is_file(name, F_OK | R_OK))
	{
		std::ifstream	file(name.c_str());

		if (file.is_open())
		{
			_set_content_type(name.c_str());
			_serve_error_file(file);
			file.close();
			return ;
		}
	}
	_serve_error_plain();

	return ;
}

void				http::Response::_serve_error_file(std::ifstream & file)
{
	std::string			buffer;

	_body = "";

	while (std::getline(file, buffer))
		_body += buffer;

	_set_content_length(_body.size());

	return ;
}

void				http::Response::_serve_error_plain(void)
{
	std::stringstream	body;

	body << "ERROR" << std::endl << _status << ' ' <<_statuses[_status] << std::endl;
	_body = body.str();

	_header["Content-Type"] = "text/plain";

	_set_content_length(_body.size());

	return ;
}

void				http::Response::_set_cgi(std::string const & path)
{
	size_t		length = _body.size();

	_set_content_type(path);
	_set_content_length(length);

	return ;
}

void				http::Response::_chunk(void)
{
	std::map<std::string, std::string>::iterator	it = _header.find("Content-Length");

	_header.erase(it);
	_header["Transfer-Encoding"] = "chunked";
	_is_chunk = true;
	_first_chunk = true;
}

void				http::Response::_set_content_type(std::string const & path)
{
	if (_header["Content-Type"].empty() == false)
		return ;

	size_t			pos = path.find_last_of('.');

	if (pos != std::string::npos && _MIME_types.count(path.substr(pos)))
		_header["Content-Type"] = _MIME_types[path.substr(pos)];
	else
		_header["Content-Type"] = "application/octet-stream";

	return ;
}

void				http::Response::_set_content_length(size_t length)
{
	if (_header["Content-Length"].empty() == false)
		return ;

	std::stringstream	nbr;

	nbr << length;
	_header["Content-Length"] = nbr.str();

	return ;
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

int					http::Response::_OK(void)
{
	_status = 200;
	return (_status);
}

int					http::Response::_created(void)
{
	_status = 201;
	return (_status);
}

int					http::Response::_no_content(void)
{
	_status = 204;
	return (_status);
}

int					http::Response::_permanent_redirect(void)
{
	_header["Location"] = _route.redirect;
	_status = 308;
	return (_status);
}

int					http::Response::_bad_request(void)
{
	_status = 400;
	return (_status);
}

int					http::Response::_forbidden(void)
{
	_status = 403;
	return (_status);
}

int					http::Response::_not_found(void)
{
	_status = 404;
	return (_status);
}

int					http::Response::_method_not_allowed(void)
{
	std::set<std::string>::iterator		it = _route.method.begin();

	_header["Allow"];
	while (it != _route.method.end())
	{
		if (it != _route.method.begin())
			_header["Allow"] += ", ";
		_header["Allow"] += *it;
		++it;
	}
	_status = 405;
	return (_status);
}

int					http::Response::_gone(void)
{
	_status = 410;
	return (_status);
}

int					http::Response::_content_too_large(void)
{
	_status = 413;
	return (_status);
}

int					http::Response::_internal_server_error(void)
{
	_status = 500;
	return (_status);
}

bool				http::Response::_is_file(std::string const & path, int mode)
{
	if (::access(path.c_str(), mode))
		return (false);

	DIR *	dir = opendir(path.c_str());

	if (dir == NULL)
		return (true);

	closedir(dir);

	return (false);
}

bool				http::Response::_is_dir(std::string const & path)
{
	if (::access(path.c_str(), F_OK))
		return (false);

	DIR *	dir = opendir(path.c_str());

	if (dir == NULL)
		return (false);

	closedir(dir);

	return (true);
}

char **				http::Response::_vector_to_array(std::vector<std::string> vec)
{
	char **		array;

	array = static_cast<char **>(std::malloc((vec.size() + 1) * sizeof(char *)));

	if (array != NULL)
	{
		for (size_t i = 0; i < vec.size(); ++i)
			array[i] = strdup(vec[i].c_str());
		array[vec.size()] = NULL;
	}

	return (array);
}

char **				http::Response::_map_to_array(std::map<std::string, std::string> map)
{
	char **		array;
	ssize_t		i = 0;

	array = static_cast<char **>(std::malloc((map.size() + 1) * sizeof(char *)));

	if (array != NULL)
	{
		std::map<std::string, std::string>::iterator	it;

		for (it = map.begin(); it != map.end(); ++it)
		{
			array[i] = strdup((it->first + "=" + it->second).c_str());
			++i;
		}
		array[i] = NULL;
	}

	return (array);
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

std::map<int, std::string>		http::Response::_init_default_err_pages(void)
{
	std::map<int, std::string>	default_err_pages;

	default_err_pages[300] = "error/html/300.html";
	default_err_pages[301] = "error/html/301.html";
	default_err_pages[302] = "error/html/302.html";
	default_err_pages[303] = "error/html/303.html";
	default_err_pages[304] = "error/html/304.html";
	default_err_pages[305] = "error/html/305.html";
	default_err_pages[306] = "error/html/306.html";
	default_err_pages[307] = "error/html/307.html";
	default_err_pages[308] = "error/html/308.html";

	default_err_pages[400] = "error/html/400.html";
	default_err_pages[401] = "error/html/401.html";
	default_err_pages[402] = "error/html/402.html";
	default_err_pages[403] = "error/html/403.html";
	default_err_pages[404] = "error/html/404.html";
	default_err_pages[405] = "error/html/405.html";
	default_err_pages[406] = "error/html/406.html";
	default_err_pages[407] = "error/html/407.html";
	default_err_pages[408] = "error/html/408.html";
	default_err_pages[409] = "error/html/409.html";
	default_err_pages[410] = "error/html/410.html";
	default_err_pages[411] = "error/html/411.html";
	default_err_pages[412] = "error/html/412.html";
	default_err_pages[413] = "error/html/413.html";
	default_err_pages[414] = "error/html/414.html";
	default_err_pages[415] = "error/html/415.html";
	default_err_pages[416] = "error/html/416.html";
	default_err_pages[417] = "error/html/417.html";
	default_err_pages[418] = "error/html/418.html";
	default_err_pages[421] = "error/html/421.html";
	default_err_pages[422] = "error/html/422.html";
	default_err_pages[423] = "error/html/423.html";
	default_err_pages[424] = "error/html/424.html";
	default_err_pages[425] = "error/html/425.html";
	default_err_pages[426] = "error/html/426.html";
	default_err_pages[428] = "error/html/428.html";
	default_err_pages[429] = "error/html/429.html";
	default_err_pages[431] = "error/html/431.html";
	default_err_pages[451] = "error/html/451.html";

	default_err_pages[500] = "error/html/500.html";
	default_err_pages[501] = "error/html/501.html";
	default_err_pages[502] = "error/html/502.html";
	default_err_pages[503] = "error/html/503.html";
	default_err_pages[504] = "error/html/504.html";
	default_err_pages[505] = "error/html/505.html";
	default_err_pages[506] = "error/html/506.html";
	default_err_pages[507] = "error/html/507.html";
	default_err_pages[508] = "error/html/508.html";
	default_err_pages[510] = "error/html/510.html";
	default_err_pages[511] = "error/html/511.html";

	return (default_err_pages);
}

std::map<std::string, std::string>		http::Response::_init_MIME_types(void)
{
	std::map<std::string, std::string>	MIME_types;

	MIME_types[".html"] = "text/html";
	MIME_types[".htm"] = "text/html";
	MIME_types[".shtml"] = "text/html";
	MIME_types[".css"] = "text/css";
	MIME_types[".xml"] = "text/xml";
	MIME_types[".gif"] = "image/gif";
	MIME_types[".jpeg"] = "image/jpeg";
	MIME_types[".jpg"] = "image/jpeg";
	MIME_types[".js"] = "application/javascript";
	MIME_types[".atom"] = "application/atom+xml";
	MIME_types[".rss"] = "application/rss+xml";

	MIME_types[".mml"] = "text/mathml";
	MIME_types[".txt"] = "text/plain";
	MIME_types[".jad"] = "text/vnd.sun.j2me.app-descriptor";
	MIME_types[".wml"] = "text/vnd.wap.wml";
	MIME_types[".htc"] = "text/x-component";

	MIME_types[".png"] = "image/png";
	MIME_types[".tif"] = "image/tiff";
	MIME_types[".tiff"] = "image/tiff";
	MIME_types[".wbmp"] = "image/vnd.wap.wbmp";
	MIME_types[".ico"] = "image/x-icon";
	MIME_types[".jng"] = "image/x-jng";
	MIME_types[".bmp"] = "image/x-ms-bmp";
	MIME_types[".svg"] = "image/svg+xml";
	MIME_types[".svgz"] = "image/svg+xml";
	MIME_types[".webp"] = "image/webp";

	MIME_types[".woff"] = "application/font-woff";
	MIME_types[".jar"] = "application/java-archive";
	MIME_types[".war"] = "application/java-archive";
	MIME_types[".ear"] = "application/java-archive";
	MIME_types[".json"] = "application/json";
	MIME_types[".hqx"] = "application/mac-binhex40";
	MIME_types[".doc"] = "application/msword";
	MIME_types[".pdf"] = "application/pdf";
	MIME_types[".ps"] = "application/postscript";
	MIME_types[".eps"] = "application/postscript";
	MIME_types[".ai"] = "application/postscript";
	MIME_types[".rtf"] = "application/rtf";
	MIME_types[".m3u8"] = "application/vnd.apple.mpegurl";
	MIME_types[".xls"] = "application/vnd.ms-excel";
	MIME_types[".eot"] = "application/vnd.ms-fontobject";
	MIME_types[".ppt"] = "application/vnd.ms-powerpoint";
	MIME_types[".wmlc"] = "application/vnd.map.wmlc";
	MIME_types[".kml"] = "application/vnd.google-earth.kml+xml";
	MIME_types[".mkz"] = "application/vnd.google-earth.kmz";
	MIME_types[".7z"] = "application/x-7z-compressed";
	MIME_types[".cco"] = "application/x-cocoa";
	MIME_types[".jardiff"] = "application/x-java-archive-diff";
	MIME_types[".jnlp"] = "application/x-java-jnlp-file";
	MIME_types[".run"] = "application/x-makeself";
	MIME_types[".pl"] = "application/x-perl";
	MIME_types[".pm"] = "application/x-perl";
	MIME_types[".prc"] = "application/x-pilot";
	MIME_types[".pdb"] = "application/x-pilot";
	MIME_types[".rar"] = "application/x-rar-compressed";
	MIME_types[".rpm"] = "application/x-redhat-package-manager";
	MIME_types[".sea"] = "application/x-sea";
	MIME_types[".swf"] = "application/x-shockwave-flash";
	MIME_types[".sit"] = "application/x-stuffit";
	MIME_types[".tcl"] = "application/x-tcl";
	MIME_types[".tk"] = "application/x-tcl";
	MIME_types[".der"] = "application/x-x509-ca-cert";
	MIME_types[".pem"] = "application/x-x509-ca-cert";
	MIME_types[".crt"] = "application/x-x509-ca-cert";
	MIME_types[".xpi"] = "application/x-xpinstall";
	MIME_types[".xhtml"] = "application/xhtml+xml";
	MIME_types[".xspf"] = "application/xspf+xml";
	MIME_types[".zip"] = "application/zip";

	MIME_types[".bin"] = "application/octet-stream";
	MIME_types[".exe"] = "application/octet-stream";
	MIME_types[".dll"] = "application/octet-stream";
	MIME_types[".deb"] = "application/octet-stream";
	MIME_types[".dmg"] = "application/octet-stream";
	MIME_types[".iso"] = "application/octet-stream";
	MIME_types[".img"] = "application/octet-stream";
	MIME_types[".msi"] = "application/octet-stream";
	MIME_types[".msp"] = "application/octet-stream";
	MIME_types[".msm"] = "application/octet-stream";

	MIME_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordpressingml.document";
	MIME_types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	MIME_types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";

	MIME_types[".mid"] = "audio/midi";
	MIME_types[".midi"] = "audio/midi";
	MIME_types[".kar"] = "audio/midi";
	MIME_types[".mp3"] = "audio/mpeg";
	MIME_types[".ogg"] = "audio/ogg";
	MIME_types[".m4a"] = "audio/x-m4a";
	MIME_types[".ra"] = "audio/x-realaudio";

	MIME_types[".3gpp"] = "video/3ggp";
	MIME_types[".3gp"] = "video/3ggp";
	MIME_types[".ts"] = "video/mp2t";
	MIME_types[".mp4"] = "video/mp4";
	MIME_types[".mpeg"] = "video/mpeg";
	MIME_types[".mpg"] = "video/mpeg";
	MIME_types[".mov"] = "video/quicktime";
	MIME_types[".webm"] = "video/webm";
	MIME_types[".flv"] = "video/x-flv";
	MIME_types[".m4v"] = "video/x-m4v";
	MIME_types[".mng"] = "video/x-mng";
	MIME_types[".asx"] = "video/x-ms-asf";
	MIME_types[".asf"] = "video/x-ms-asf";
	MIME_types[".wmv"] = "video/x-ms-wmv";
	MIME_types[".avi"] = "video/x-msvideo";

	return (MIME_types);
}

std::map<int, std::string>		http::Response::_statuses = Response::_init_statuses();

std::map<int, std::string>		http::Response::_default_err_pages = Response::_init_default_err_pages();

std::map<std::string, std::string>		http::Response::_MIME_types = Response::_init_MIME_types();


// canonical class form

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


void				http::Response::_cgi_handler(t_request const & request, std::string const & path)
{
	pid_t	pid;
	FILE *	tmp_in = std::tmpfile();
	FILE *	tmp_out = std::tmpfile();

	std::string		buffer;

	if (request.transfer_encoding.empty())
		buffer = request.body;
	else
	{
		for (size_t	i = 0; i < request.chunks.size(); ++i)
			buffer += request.chunks[i];
	}

	for (size_t	i = 0; i < buffer.size(); ++i)
	{
		if (std::fputc(buffer[i], tmp_in) == EOF)
			break ;
	}
	std::fseek(tmp_in, 0, SEEK_SET);

	int		fd_in = fileno(tmp_in);
	int		fd_out = fileno(tmp_out);

	pid = fork();
	if (pid < 0)
		std::cout << "Error: starting process\n";
	else if (pid == 0)
	{
		dup2(fd_in, STDIN_FILENO);
		dup2(fd_out, STDOUT_FILENO);
		_exec_cgi(request, path);
	}
	else
	{
		waitpid(-1, NULL, 0);
		_get_cgi_response(tmp_out);
		fclose(tmp_in);
		fclose(tmp_out);
		close(fd_in);
		close(fd_out);
	}
}

void	http::Response::_init_cgi_env( std::vector<std::string> & argv, \
									   std::map<std::string, std::string> & env, \
									   t_request const & request,\
									   const std::string & path )
{
	std::stringstream	content_length;
	std::stringstream	port;
	size_t				size = 0;

	if (request.transfer_encoding.empty())
		content_length << request.content_length;
	else
	{
		for (size_t	i = 0; i < request.chunks.size(); ++i)
			size += request.chunks[i].length();
		content_length << size;
	}

	argv.push_back(path);

	env["REDIRECT_STATUS"] = "200";
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SERVER_NAME"] = request.host;
	env["SERVER_PROTOCOL"] = request.protocol;
	port << request.port;
	env["SERVER_PORT"] = port.str();
	env["REQUEST_METHOD"] = request.method;
	env["PATH_INFO"] = request.path;
	env["PATH_TRANSLATED"] = path;
	env["QUERY_STRING"] = request.query;
	env["CONTENT_LENGTH"] = content_length.str();

	std::map<std::string, std::string>::const_iterator	it;
	std::string		key;
	unsigned char	tmp;

	for (it = request.header.begin(); it != request.header.end(); ++it)
	{
		if (it->first != "Transfer-Encoding" && it->first != "Content-Length")
		{
			key = "";
			if (it->first != "Content-Type")
				key = "HTTP_";
			for (size_t i = 0; i < it->first.size(); ++i)
			{
				tmp = static_cast<unsigned char>(it->first[i]);
				if (std::isalpha(tmp))
					key.push_back(static_cast<char>(std::toupper(tmp)));
				else if (it->first[i] == '-')
					key.push_back('_');
			}
			env[key] = it->second;
		}
	}
}


void	http::Response::_exec_cgi(t_request const & request, std::string const & path)
{
	std::vector<std::string>			argv;
	std::map<std::string, std::string>	env;

	_init_cgi_env(argv, env, request, path);

	execve(_route.cgi_pass.c_str(), _vector_to_array(argv), _map_to_array(env));
	std::cerr << "execve: " << std::strerror(errno) << std::endl;
	std::cout << "Status: 500\r\n\r\n";
	exit(EXIT_FAILURE);
}

void	http::Response::_get_cgi_response(FILE * tmp_out)
{
	std::fseek(tmp_out, 0, SEEK_SET);

	int			tmp = std::fgetc(tmp_out);

	while (tmp != EOF)
	{
		_body += tmp;
		tmp = std::fgetc(tmp_out);
	}

	size_t		head_end = _body.find("\r\n\r\n");
	size_t		line_end = _body.find("\r\n");
	size_t		pos = 0;

	if (head_end != std::string::npos)
	{
		while (pos != head_end + std::strlen("\r\n"))
		{
			if (_body.substr(pos, line_end - pos).find("Content-type: ") == 0)
			{
				pos += std::strlen("Content-type: ");
				_header["Content-Type"] = _body.substr(pos, line_end - pos);
			}
			else if (_body.substr(pos, line_end - pos).find("Status: ") == 0)
			{
				pos += std::strlen("Status: ");
				_status = std::atoi(_body.substr(pos, 3).c_str());
			}
			pos = line_end + std::strlen("\r\n");
			line_end = _body.find("\r\n", pos);
		}
		_body.erase(0, head_end + std::strlen("\r\n\r\n"));
	}

	if (!_status)
		_OK();
}

void	http::Response::_get_filename_to_upload( t_request const & request, size_t & cursor,\
												 std::string & file_name )
{
	size_t	tmp = request.body.find("filename=\"");
	if (tmp == std::string::npos)
		return ;

	cursor += tmp + std::strlen("filename=\"");
	file_name = request.body.substr(cursor, request.body.find("\"", cursor) - cursor);

	return ;
}

void	http::Response::_get_file_type(t_request const & request, size_t cursor, std::string & file_type)
{
	cursor = request.body.find("Content-Type:", cursor);

	if (cursor == std::string::npos)
		return ;
	cursor = request.body.find(':', cursor);

	size_t	start = request.body.find_first_not_of(" \t", ++cursor);
	if (start == std::string::npos)
		return ;
	size_t end = request.body.find("\r\n", start);
	if (end == std::string::npos)
		return ;
	file_type = request.body.substr(start, end - start);
}

void	http::Response::_upload( std::string const & target, std::string const & file_type,\
								 std::string const & file_content )
{
	FILE* 			res;
	std::ofstream	target_stream;
	if (!file_type.compare("text/plain"))
	{
		res = std::fopen(target.c_str(), "w");
		std::fclose(res);
		target_stream.open(target.c_str());
	}
	else
	{
		res = std::fopen(target.c_str(), "wb");
		std::fclose(res);
		target_stream.open(target.c_str(), std::ios::out | std::ios::binary);
	}
	target_stream << file_content;
	target_stream.close();
}
