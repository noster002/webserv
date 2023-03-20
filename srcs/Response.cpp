/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:43:02 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/20 18:29:03 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

http::Response::Response(params_t const & server)\
 : _server(server), _buffer(), _protocol("HTTP/1.1"), _status(), _header(), _body()
{
	for (std::map<std::string, route_t>::iterator it = _server.routes.begin(); it != _server.routes.end(); ++it)
		std::cout << "(" << it->first << ")(" << it->second.root << ")" << std::endl;
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

	if (_status < 300)
	{
		if (request.method == "GET")
			_serve_get_request(request);
		else if (request.method == "POST")
			_serve_post_request(request);
		else if (request.method == "DELETE")
			_serve_delete_request(request);
	}
	if (_status >= 300)
		_serve_error();
	_set_status_line();
	_set_head();
	_set_body();

	return ;
}

void				http::Response::_serve_get_request(t_request const & request)
{
	std::string			pathname = _get_path(request);

	if (pathname.empty())
		return ;

	_set_content_type(pathname);

	std::fstream		file(pathname.c_str());

	if (file.is_open() == false)
	{
		_status = 404;
		return ;
	}

	std::stringstream	body;

	body << file.rdbuf();
	_body = body.str();

	std::stringstream	size;

	size << _body.size();
	_header["Content-Length"] = size.str();
	_status = 200;

	return ;
}

void				http::Response::_serve_post_request(t_request const & request)
{
	(void)request;
}

void				http::Response::_serve_delete_request(t_request const & request)
{
	(void)request;
}

std::string			http::Response::_get_path(t_request const & request)
{
	std::string		path;
	std::string		match = request.path;
	size_t			pos = match.find_last_of('/');

	while (pos != std::string::npos)
	{
		match = match.substr(0, ++pos);

		if (_server.routes.count(match) && \
			_server.routes[match].method.count(request.method))
			break ;

		match.pop_back();

		if (_server.routes.count(match) && \
			_server.routes[match].method.count(request.method))
			break ;

		pos = match.find_last_of('/');
	}
	if (pos == std::string::npos)
	{
		_status = 404;
		return ("");
	}

	if ((_server.routes[match].root.empty() == false) && \
		(_server.routes[match].root.back() != '/'))
		--pos;
	path = _server.routes[match].root + request.path.substr(pos);

	return (path);
}

void			http::Response::_set_content_type(std::string const & path)
{
	size_t			pos = path.find_last_of('.');

	if (pos != std::string::npos)
		_header["Content-Type"] = _MIME_types[path.substr(pos)];

	if (_header["Content-Type"].empty())
		_header["Content-Type"] = "application/octet-stream";
}

void				http::Response::_serve_error(void)
{
	if (_server.err_pages.count(_status))
	{
		std::fstream	file(_server.err_pages[_status].c_str());

		if (file.is_open())
		{
			_serve_error_file(file);
			return ;
		}
	}
	std::fstream	file(_default_err_pages[_status].c_str());

	if (file.is_open())
	{
		_serve_error_file(file);
		return ;
	}

	_serve_error_plain();
	return ;
}

void				http::Response::_serve_error_file(std::fstream & file)
{
	std::string			buffer;
	std::stringstream	nbr;

	while (std::getline(file, buffer))
		_body += buffer;

	_header["Content-Type"] = "text/html";
	nbr << _body.size();
	_header["Content-Length"] = nbr.str();

	return ;
}

void				http::Response::_serve_error_plain(void)
{
	std::stringstream	body;
	std::stringstream	nbr;

	body << "ERROR" << std::endl << _status << ' ' <<_statuses[_status] << std::endl;
	_body = body.str();

	_header["Content-Type"] = "text/plain";
	nbr << _body.size();
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

	default_err_pages[300] = "http/html/error/300.html";
	default_err_pages[301] = "http/html/error/301.html";
	default_err_pages[302] = "http/html/error/302.html";
	default_err_pages[303] = "http/html/error/303.html";
	default_err_pages[304] = "http/html/error/304.html";
	default_err_pages[305] = "http/html/error/305.html";
	default_err_pages[306] = "http/html/error/306.html";
	default_err_pages[307] = "http/html/error/307.html";
	default_err_pages[308] = "http/html/error/308.html";

	default_err_pages[400] = "http/html/error/400.html";
	default_err_pages[401] = "http/html/error/401.html";
	default_err_pages[402] = "http/html/error/402.html";
	default_err_pages[403] = "http/html/error/403.html";
	default_err_pages[404] = "http/html/error/404.html";
	default_err_pages[405] = "http/html/error/405.html";
	default_err_pages[406] = "http/html/error/406.html";
	default_err_pages[407] = "http/html/error/407.html";
	default_err_pages[408] = "http/html/error/408.html";
	default_err_pages[409] = "http/html/error/409.html";
	default_err_pages[410] = "http/html/error/410.html";
	default_err_pages[411] = "http/html/error/411.html";
	default_err_pages[412] = "http/html/error/412.html";
	default_err_pages[413] = "http/html/error/413.html";
	default_err_pages[414] = "http/html/error/414.html";
	default_err_pages[415] = "http/html/error/415.html";
	default_err_pages[416] = "http/html/error/416.html";
	default_err_pages[417] = "http/html/error/417.html";
	default_err_pages[418] = "http/html/error/418.html";
	default_err_pages[421] = "http/html/error/421.html";
	default_err_pages[422] = "http/html/error/422.html";
	default_err_pages[423] = "http/html/error/423.html";
	default_err_pages[424] = "http/html/error/424.html";
	default_err_pages[425] = "http/html/error/425.html";
	default_err_pages[426] = "http/html/error/426.html";
	default_err_pages[428] = "http/html/error/428.html";
	default_err_pages[429] = "http/html/error/429.html";
	default_err_pages[431] = "http/html/error/431.html";
	default_err_pages[451] = "http/html/error/451.html";

	default_err_pages[500] = "http/html/error/500.html";
	default_err_pages[501] = "http/html/error/501.html";
	default_err_pages[502] = "http/html/error/502.html";
	default_err_pages[503] = "http/html/error/503.html";
	default_err_pages[504] = "http/html/error/504.html";
	default_err_pages[505] = "http/html/error/505.html";
	default_err_pages[506] = "http/html/error/506.html";
	default_err_pages[507] = "http/html/error/507.html";
	default_err_pages[508] = "http/html/error/508.html";
	default_err_pages[510] = "http/html/error/510.html";
	default_err_pages[511] = "http/html/error/511.html";

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
	MIME_types[".rss"] = "text/xml";
	MIME_types[".gif"] = "image/gif";
	MIME_types[".jpeg"] = "image/jpeg";
	MIME_types[".jpg"] = "image/jpeg";
	MIME_types[".js"] = "application/x-javascript";
	MIME_types[".txt"] = "text/plain";
	MIME_types[".htc"] = "text/x-component";
	MIME_types[".mml"] = "text/mathml";
	MIME_types[".png"] = "image/png";
	MIME_types[".ico"] = "image/x-icon";
	MIME_types[".jng"] = "image/x-jng";
	MIME_types[".wbmp"] = "image/vnd.wap.wbmp";
	MIME_types[".jar"] = "application/java-archive";
	MIME_types[".war"] = "application/java-archive";
	MIME_types[".ear"] = "application/java-archive";
	MIME_types[".hqx"] = "application/mac-binhex40";
	MIME_types[".pdf"] = "application/pdf";
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
	MIME_types[".zip"] = "application/zip";
	MIME_types[".deb"] = "application/octet-stream";
	MIME_types[".bin"] = "application/octet-stream";
	MIME_types[".exe"] = "application/octet-stream";
	MIME_types[".dll"] = "application/octet-stream";
	MIME_types[".dmg"] = "application/octet-stream";
	MIME_types[".eot"] = "application/octet-stream";
	MIME_types[".iso"] = "application/octet-stream";
	MIME_types[".img"] = "application/octet-stream";
	MIME_types[".msi"] = "application/octet-stream";
	MIME_types[".msp"] = "application/octet-stream";
	MIME_types[".msm"] = "application/octet-stream";
	MIME_types[".mp3"] = "audio/mpeg";
	MIME_types[".ra"] = "audio/x-realaudio";
	MIME_types[".mpeg"] = "video/mpeg";
	MIME_types[".mpg"] = "video/mpeg";
	MIME_types[".mov"] = "video/quicktime";
	MIME_types[".flv"] = "video/x-flv";
	MIME_types[".avi"] = "video/x-msvideo";
	MIME_types[".wmv"] = "video/x-ms-wmv";
	MIME_types[".asx"] = "video/x-ms-asf";
	MIME_types[".asf"] = "video/x-ms-asf";
	MIME_types[".mng"] = "video/x-mng";

	return (MIME_types);
}

std::map<int, std::string>		http::Response::_statuses = Response::_init_statuses();

std::map<int, std::string>		http::Response::_default_err_pages = Response::_init_default_err_pages();

std::map<std::string, std::string>		http::Response::_MIME_types = Response::_init_MIME_types();


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
