/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:43:02 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/28 13:50:34 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

http::Response::Response(void)\
 : _server(), _buffer(), _protocol("HTTP/1.1"), _status(), _header(), _body(),\
   _is_cgi(false)
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

void				http::Response::set_server(params_t const & server)
{
	_server = server;
	return ;
}

void				http::Response::build(int error, t_request const & request)
{
	_status = error;

	if (request.body.size() > _server.client_max_body_size)
		_content_too_large("HTTP request body: content too large");

	if (_status < 300)
	{
		if (request.method == "GET")
			_serve_get_request(request);
		else if (request.method == "POST")
			_serve_post_request(request);
		else if (request.method == "DELETE")
			_serve_delete_request(request);
	}
	if (_status >= 400)
		_serve_error();
	else if (_is_cgi)
		_set_cgi();
	_set_status_line();
	_set_head();
	_set_body();

	return ;
}

int					http::Response::_serve_get_request(t_request const & request)
{
	std::string			path;
	
	if (_get_path(request, path))
		return (_status);

	std::fstream		file(path.c_str());

	if (file.is_open() == false)
		return (_not_found());

	_set_content_type(path);

	std::stringstream	body;

	body << file.rdbuf();
	_body = body.str();

	std::stringstream	size;

	size << _body.size();
	_header["Content-Length"] = size.str();

	return (_OK());
}

void				http::Response::_serve_post_request(t_request const & request)
{
	(void)request;
}

void				http::Response::_serve_delete_request(t_request const & request)
{
	(void)request;
}

int					http::Response::_get_path(t_request const & request, std::string & path)
{
	std::string		match = request.path;
	size_t			pos = match.size();
	bool			is_dir;

	do
	{
		is_dir = (match.back() == '/');
		if (_server.routes.count(match))
		{
			if (_server.routes[match].method.count(request.method))
			{
				if (_server.routes[match].redirect.empty() == false)
					return (_permanent_redirect(match));
				else if (_server.routes[match].root.empty())
					return (_gone());
				if (is_dir && _server.routes[match].root.back() != '/')
					--pos;
				else if (!is_dir && _server.routes[match].root.back() == '/' && pos < request.path.size())
					++pos;
				path = _server.routes[match].root + request.path.substr(pos);
				if (path.back() == '/')
				{
					if (_server.routes[match].index.empty() == false)
						path += _server.routes[match].index;
					else if (_server.routes[match].directory_listing == false)
						return (_not_found());
					else
						return (_directory_listing(request, path));
				}
				if (_server.routes[match].cgi_pass.empty() == false && \
					_server.routes[match].cgi_ext == ".php")
					_is_cgi = true;
				return (0);
			}
			return (_method_not_allowed(match));
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
			body << "\t\t<p><a href=\"http://" << request.host << ":" << request.port\
				 << request.path << entry->d_name;
			if (entry->d_type == DT_DIR)
				body << "/\">" << entry->d_name << "/</a></p>\n";
			else
				body << "\">" << entry->d_name << "</a></p>\n";
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

	std::stringstream	nbr;

	nbr << _body.size();
	_header["Content-Length"] = nbr.str();

	return (_OK());
}

void				http::Response::_serve_error(void)
{
	if (_server.err_pages.count(_status))
	{
		std::fstream	file(_server.err_pages[_status].c_str());

		if (file.is_open())
		{
			_set_content_type(_server.err_pages[_status].c_str());
			_serve_error_file(file);
			return ;
		}
	}
	std::fstream	file(_default_err_pages[_status].c_str());

	if (file.is_open())
	{
		_set_content_type(_default_err_pages[_status].c_str());
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

void				http::Response::_set_content_type(std::string const & path)
{
	size_t			pos = path.find_last_of('.');

	if (pos != std::string::npos)
		_header["Content-Type"] = _MIME_types[path.substr(pos)];
	else
		_header["Content-Type"] = "application/octet-stream";
}

void				http::Response::_set_status_line(void)
{
	std::stringstream	status_line;

	status_line << _protocol << ' ' << _status << ' ' << _statuses[_status] << "\r\n";

	_buffer = status_line.str();

	return ;
}

void				http::Response::_set_cgi(void)
{
	size_t		pos = _body.find("\r\n\r\n");
	size_t		length = _body.size();

	if (pos != std::string::npos)
		length -= pos;

	std::stringstream	nbr;

	nbr << length;
	_header["Content-Length"] = nbr.str();

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

int					http::Response::_permanent_redirect(std::string const & path)
{
	_header["Location"] = _server.routes[path].redirect;
	_status = 308;
	return (_status);
}

int					http::Response::_not_found(void)
{
	_status = 404;
	return (_status);
}

int					http::Response::_method_not_allowed(std::string const & path)
{
	std::set<std::string>::iterator		it = _server.routes[path].method.begin();
	std::set<std::string>::iterator		end = _server.routes[path].method.end();
	std::set<std::string>::iterator		last = end;

	--last;
	while (it != end)
	{
		_header["Allow"] += *it;
		if (it != last)
			_header["Allow"] += ", ";
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

int					http::Response::_content_too_large(std::string const & error_msg)
{
	std::cerr << error_msg << std::endl;
	_status = 413;
	return (_status);
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
