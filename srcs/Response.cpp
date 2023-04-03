/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:43:02 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/03 19:33:55 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

http::Response::Response(void)\
 : _server(), _buffer(), _protocol("HTTP/1.1"), _status(), _header(), _body(),\
   _is_cgi(false), _is_upload(false)
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
	std::string		path;

	_status = error;

	if (!_status && request.body.size() > _server.client_max_body_size)
		_content_too_large("HTTP request body: content too large");
	if (!_status)
		_get_path(request, path);
	if (!_status)
	{
		if (_is_cgi)
			_cgi_handler(request, path);
		else if (request.method == "GET" || request.method == "HEAD")
			_serve_get_request(request, path);
		else if (request.method == "POST" || request.method == "PUT")
			_serve_post_request(request, path);
		else if (request.method == "DELETE")
			_serve_delete_request(path);
	}
	if (_status >= 400)
		_serve_error();
	else if (_is_cgi)
		_set_cgi();
	_set_status_line();
	_set_head();
	if (request.method != "HEAD")
		_set_body();

	return ;
}

int					http::Response::_serve_get_request(t_request const & request, std::string & path)
{
	std::fstream		file(path.c_str());

	if (file.is_open() == false)
	{
		t_request	modified(request);

		if (_check_directory(modified))
		{
			std::cout << "hl" << std::endl;
			_get_path(modified, path);
			std::cout << path << std::endl;
			_serve_get_request(modified, path);
			return (0);
		}
		else
			return (_not_found());
	}

	_set_content_type(path);

	std::stringstream	body;

	body << file.rdbuf();
	_body = body.str();

	_set_content_length(_body.size());

	return (_OK());
}

int					http::Response::_check_directory(t_request & request)
{
	if (request.path.back() == '/')
		return (0);

	request.path += '/';
	return (1);
}

int					http::Response::_serve_post_request(t_request const & request, std::string const & path)
{
	std::string	file_name, target, file_content, file_type;
	size_t 		cursor = 0, ending, start, pos;


	if (_is_upload == false)
		return (_forbidden());
	if (!request.chunks.empty())
	{
		for (size_t i = 0; i < request.chunks.size(); ++i)
		{
			file_content += request.chunks[i];
		}
		_upload(path, "", file_content);
		_created();
		return (0);
	}

	if (_get_filename_to_upload(request, cursor, file_name))
		return (_no_content());

	cursor = request.body.find("Content-Type: ");
	start = request.body.find_first_of("\n", cursor) + 3;
	ending = request.body.find("------WebKit", start) - 2;
	file_content = request.body.substr(start, ending - start);
	_get_file_type(request, cursor, file_type);
	pos = path.find_last_of('/');
	target = path.substr(0, ++pos) + file_name;
	_upload(target, file_type, file_content);
	_continue_to_next_field(request, ending, path);

	_body += "<h2>Request successfully performed</h2>\n";
	_header["Content-Type"] = "text/html";
	_header["Location"] = target;

	return (_created());
}

void				http::Response::_serve_delete_request(std::string const & path)
{
	if (remove(path.c_str()) == 0)
		_status = 204;
	else
		_status = 404;
	_body = "";
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
				if (_server.routes[match].upload)
					_is_upload = true;
				if (path.back() == '/')
				{
					if (_is_upload && request.chunks.empty() == false)
						return (_not_found());
					else if (_server.routes[match].index.empty() == false)
						path += _server.routes[match].index;
					else if ((_is_upload == false || \
							 request.method == "GET" || request.method == "HEAD") &&\
							 _server.routes[match].directory_listing)
						return (_directory_listing(request, path));
					else if (_is_upload == false || \
							 request.method == "GET" || request.method == "GET")
						return (_not_found());
				}
				pos = path.find_last_of('.');

				if (pos != std::string::npos && \
					_server.routes[match].cgi_pass.empty() == false && \
					_server.routes[match].cgi_ext == path.substr(pos))
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

	_set_content_length(_body.size());

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

void				http::Response::_set_cgi(void)
{
	size_t		length = _body.size();

	_set_content_length(length);

	return ;
}

void				http::Response::_set_content_type(std::string const & path)
{
	if (_header["Content-Type"].empty() == false)
		return ;

	size_t			pos = path.find_last_of('.');

	if (pos != std::string::npos)
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

int					http::Response::_permanent_redirect(std::string const & path)
{
	_header["Location"] = _server.routes[path].redirect;
	_status = 308;
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
	int 		fds[2];
	char		body[100000];
	pid_t 		pid;

	pipe(fds);
	std::map<std::string, std::string>::const_iterator it = request.header.begin();
	for (; it != request.header.end(); ++it)
		_header[it->first] = it->second;
	pid = fork();
	if (pid < 0)
		std::cout << "Error: starting process\n";
	else if (pid == 0)
		_exec_cgi(request, path, fds);
	else
	{
		waitpid(pid, NULL, -1);
		close(fds[1]);
		_get_cgi_response(fds, body, request.method);
	}
}

void	http::Response::_init_cgi_env( char *args[], char *env[], t_request const & request,\
									   const std::string path )
{
	std::stringstream ss;

	ss << request.content_length;
	args[0] = strdup(path.c_str());
	args[1] = NULL;
	env[0] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[1] = strdup("REDIRECT_STATUS=200");
	env[2] = strdup(("SCRIPT_FILENAME=" + path).c_str());
	env[3] = strdup(("CONTENT_LENGTH=" + ss.str()).c_str());
	env[4] = strdup(("REQUEST_METHOD=" + request.method).c_str());
	env[5] = strdup("CONTENT_TYPE=text/html"); // !!!should be dynamic
	env[6] = strdup(("QUERY_STRING=" + request.query).c_str());
	env[7] = NULL;
}


void	http::Response::_exec_cgi(t_request const & request, std::string const & path, int fds[])
{
	char* args[2];
	char* env[8];
	std::string php_cgi_bin = CGI;
	_init_cgi_env(args, env, request, path);
	close(fds[0]);
	dup2(fds[1], STDOUT_FILENO);
	if (execve(php_cgi_bin.c_str(), args, env) < 0)
		std::cerr << "execve: " << std::strerror(errno) << std::endl;
	std::cout << "QUOI ?\n";
	exit(0);
}


void	http::Response::_get_cgi_response(int fds[], char body[], const std::string method)
{
	int 		bytes;
	int 		count = 0;
	std::string tmp_body;

	do 
	{
		bytes = read(fds[0], &body[count], 1);
		count++;
	}
	while(bytes > 0);
	close(fds[0]);
	_header["Content-type"] = "text/html";
	tmp_body = body;
	_body = tmp_body.substr(tmp_body.find("\r\n\r\n", tmp_body.find("Content-type")) + 4);
	_status = 200;
	(void)method;
}

int		http::Response::_get_filename_to_upload( t_request const & request, size_t & cursor,\
												 std::string & file_name )
{
	cursor = request.body.find("filename=\"");
	if (cursor == std::string::npos)
		return (_no_content());

	cursor += 10;
	file_name = request.body.substr(cursor, request.body.find("\"", cursor) - cursor);
	if (file_name.empty())
		return (_no_content());

	return (0);
}

void	http::Response::_get_file_type(t_request const & request, size_t cursor, std::string & file_type)
{
	size_t cursor_to_content_type = cursor + 14;
	size_t end_of_the_line = request.body.find("\n", cursor + 14);
	size_t len = end_of_the_line - cursor - 15;
	file_type = request.body.substr(cursor_to_content_type, len);
}

void	http::Response::_upload( std::string const & target, std::string const & file_type,\
								 std::string const & file_content )
{
	FILE* 			res;
	std::fstream	target_stream;
	if (!file_type.compare("text/plain"))
	{
		res = fopen(target.c_str(), "w");
		fclose(res);
		target_stream.open(target, std::ios::out);
	}
	else
	{
		res = fopen(target.c_str(), "wb");
		fclose(res);
		target_stream.open(target, std::ios::out | std::ios::binary);
	}
	target_stream << file_content;
	target_stream.close();
}

void	http::Response::_continue_to_next_field(t_request const & request, size_t ending, std::string const & path)
{
	std::string	tmp_body = request.body.substr(ending + 2);

	if (tmp_body.substr(tmp_body.find("\n") + 2) != "")
	{
		t_request sub_req(request);
		sub_req.body = tmp_body;
		_serve_post_request(sub_req, path);
	}
}
