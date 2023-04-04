/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:52 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/04 09:34:28 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

// constructor & destructor

http::Config::Config(void) {}

http::Config::~Config(void) {}

// member functions

void			http::Config::parse(std::string const & filename)
{
	this->_file = filename;
	this->validation = true;
	this->parse_config_file();
	return ;
}

bool			http::Config::getValidation() const { return this->validation; }

void			http::Config::setValidation(bool status)
{
	this->validation = status;
}

int				http::Config::get_nbr_servers(void) const { return this->nb_servers; }

params_t &		http::Config::get_server_conf(int i) 
{
	if (i < 0 || i >= this->nb_servers)
		throw std::out_of_range("server doesn't exist");
	return this->servers[i];
}


// private

void			http::Config::parse_config_file(void)
{
	std::vector<std::string>	data;
	size_t j;

	data = this->get_confdata();
	this->nb_servers  = this->count_servers(data);
	if (!this->is_valid_server_nb(this->nb_servers))
		return ;
	std::cout << servers[4].start_data << "\n";
	for (int i = 0; i < this->nb_servers; ++i)
	{
		std::cout << RED << "**********SERVER[ " << i + 1 << " ]*******************" << RESET << "\n";
		this->servers[i].client_max_body_size = 1048576;
		while (this->servers[i].start_data <= this->servers[i].end_data)
		{
			j = 0;
			http::Config::skipe_empty_line(data, &this->servers[i].start_data);
			http::Config::skipe_spaces(data[this->servers[i].start_data], &j);
			if (data[this->servers[i].start_data].substr(j, 4) == KEY_HOST)
				this->fill_host_value(data[this->servers[i].start_data], i, &j);
			else if (data[this->servers[i].start_data].substr(j, 6) == KEY_LISTEN)
				this->fill_port_value(data[this->servers[i].start_data], i, &j);
			else if (data[this->servers[i].start_data].substr(j, 11) == KEY_SERV_NAME)
				this->fill_server_name(data[this->servers[i].start_data], i, &j);
			else if (data[this->servers[i].start_data].substr(j, 10) == KEY_ERR_PAGE)
				this->fill_errors_pages(data[this->servers[i].start_data], i, &j);
			else if (data[this->servers[i].start_data].substr(j, 20) == KEY_MAX_BODY_SIZE)
				this->fill_client_max_body_size(data[this->servers[i].start_data], i, &j);
			else if (data[this->servers[i].start_data].substr(j, 5) == KEY_ROUTE)
				this->fill_routes(data, i, &j);
			else
				this->setValidation(false);
			if (!this->getValidation())
				return ;
			this->servers[i].start_data += 1;
		}
		std::cout << "HOST :\t" << this->servers[i].host << "\n";
		for (size_t k = 0; k < this->servers[i].port.size(); ++k)
			std::cout << "PORT " << k + 1 << ":\t" << this->servers[i].port[k] << "\n";
		for (size_t l = 0; l < this->servers[i].s_names.size(); ++l)
			std::cout << "SERVER NAME " << l + 1 << ":\t" << this->servers[i].s_names[l] << "\n";
		std::cout << "ERROR PAGE" << "\n";
		std::map<int, std::string>::iterator err = this->servers[i].err_pages.begin();
		while (err != this->servers[i].err_pages.end())
		{
			std::cout << err->first << " == > " << err->second << "\n";
			++err;
		}
		std::cout << "MAX BODY : " << "  " << this->servers[i].client_max_body_size << "\n";
		std::map<std::string, route_t>::iterator routes = this->servers[i].routes.begin();
		while (routes != this->servers[i].routes.end())
		{
			std::cout << "--ROUTE =>  " << routes->first << "  Methods:  ";
			for (std::set<std::string>::iterator set = routes->second.method.begin(); set != routes->second.method.end(); ++set)
			{
				std::cout << *set << " ";
			}
			std::cout << " LISTING : " << ((routes->second.directory_listing) ? "on" : "of");
			std::cout << " ROOT : " << routes->second.root;
			std::cout << " INDEX : " << routes->second.index;
			std::cout << " UPLOAD : " << ((routes->second.upload) ? "on" : "of");
			std::cout << " REDIRECT : " << routes->second.redirect;
			std::cout << " CGI_EXT : " << routes->second.cgi_ext;
			std::cout << " CGI_PASS : " << routes->second.cgi_pass;
			std::cout << "--\n";
			++routes;
		}
	}

}

std::vector<std::string>	http::Config::get_confdata()
{
	std::vector<std::string>	data;
	std::ifstream				conf(_file);
	std::string					line = "";

	if (conf.is_open())
	{
		while(std::getline(conf, line))
		{
			data.push_back(line);
		}
	}
	conf.close();
	return data;
}

int  			http::Config::count_servers(const std::vector<std::string> & confdata)
{
	int				nb = 0;
	size_t			data_len = confdata.size(), j;

	for (size_t i = 0; i < data_len; ++i)
	{
		j = KEY_SERVER_LEN;
		http::Config::skipe_empty_line(confdata, &i);
		if (confdata[i] == "" && nb >= 1)
			return (nb);
		if (confdata[i].substr(0, j) == KEY_SERVER)
		{
			http::Config::skipe_spaces(confdata[i], &j);
			if (confdata[i][j] == '\0')
			{
				i += 1;
				j = 0;
			}
			if (confdata[i][j] == OPEN_SYMBOL)
			{
				params_t param;
				param.start_data = i + 1;
				if (find_close_symbol(confdata, &i, 0) >= 0)
				{
					nb += 1;
					param.end_data = i - 1;
					this->servers.push_back(param);
				}
				else
					return (-1);
			}
			else
				return (-1);
		}
		else
			return (-1);
	}
	return (nb);
}

int 			http::Config::find_close_symbol( const std::vector<std::string> & confdata,\
												size_t* cursor, int level )
{
	size_t				data_len = confdata.size(), j;
	*cursor += 1;
	while (*cursor < data_len)
	{
		j = 0;
		if (confdata[*cursor].substr(0, KEY_SERVER_LEN) == KEY_SERVER)
			return (-1);
		if (level > 0)
		{
			http::Config::skipe_spaces(confdata[*cursor], &j);
			if (confdata[*cursor].substr(j, 5) == KEY_ROUTE)
				return (-1);
		}
		if (confdata[*cursor][j] == CLOSE_SYMBOL)
		{
			if (j + 1 == confdata[*cursor].size())
				return (j);
			j += 1;
			http::Config::skipe_spaces(confdata[*cursor], &j);
			if (confdata[*cursor][j] == '\0')
				return (j);
			return (-1);
		}
		*cursor += 1;
	}
	return (-1);
}


void					http::Config::fill_routes( std::vector<std::string> & data, size_t i,\
                                                  size_t* cursor )
{
	*cursor += 5;
	route_t routes;
	std::string route_name = this->get_route_name(data, i, cursor);
	this->servers[i].routes.insert(this->servers[i].routes.end(),
								std::pair<std::string, route_t>(route_name, routes));
	if (!this->is_route_well_formated(data, i, cursor))
		return ;
	this->servers[i].start_data += 1;
	size_t end_route = this->servers[i].start_data;
	if (this->find_close_symbol(data, &end_route, 1) < 0)
	{
		this->setValidation(false);
		return ;
	}
	this->servers[i].routes[route_name].directory_listing = false;
	this->servers[i].routes[route_name].upload = false;
	while (this->servers[i].start_data < end_route)
	{
		this->fill_route_params(data, i, cursor, route_name);
		if (!this->getValidation())
			return ;
		this->servers[i].start_data += 1;
	}
}

void			http::Config::fill_host_value( std::string line, size_t i, size_t* cursor)
{
	*cursor += 4;
	
	std::string host = this->get_inline_value(line, cursor);
	if (this->is_valid_host(host))
		this->servers[i].host = host;
	else
	{
		this->setValidation(false);
	}
}

void			http::Config::skipe_spaces(const std::string & str, size_t* cursor)
{
	while (*cursor < str.size() && (str[*cursor] == SPACE || str[*cursor] == TAB))
		*cursor += 1;
}

void			http::Config::skipe_empty_line ( const std::vector<std::string> & confdata,\
																		size_t* cursor )
{
	size_t len = confdata.size(), j;

	while (*cursor < len)
	{
		if (confdata[*cursor] == "")
			*cursor += 1;
		else
		{
			j = 0;
			while ( j < confdata[*cursor].size() \
					&& (confdata[*cursor][j] == SPACE\
					|| confdata[*cursor][j] == TAB) )
			{
				j++;
			}
			if (confdata[*cursor][j] != '\0')
				return ;
			*cursor += 1;
		}
	}
}

std::string		http::Config::get_inline_value(const std::string & line, size_t* cursor)
{
	std::string value = "";
	if (line[line.size() - 1] != SEMI_COLUMN)
	{
		this->setValidation(false);
		return value;
	}
	this->skipe_spaces(line, cursor);
	while (*cursor < line.size() - 1)
	{
		value += line[*cursor];
		*cursor += 1;
	}
	return value;
}

void			http::Config::fill_port_value( std::string line, size_t i, size_t* cursor)
{
	*cursor += 6;
	std::string port = this->get_inline_value(line, cursor);
	if (this->is_empty(port))
	{
		this->setValidation(false);
		return ;
	}
	for (size_t it = 0; it < port.size(); ++it)
	{
		if (!isdigit(port[it]) && (port[it] != TAB && port[it] != SPACE))
		{
			this->setValidation(false);
			return;
		}
	}
	this->servers[i].port.push_back(port);
}

std::string		http::Config::get_route_name( std::vector<std::string> & data, size_t i, \
												 size_t* cursor )
{
	std::string curr_line = data[this->servers[i].start_data].substr(*cursor);
	size_t count = 0;
	std::string routes_name;

	this->skipe_spaces(curr_line, &count);
	std::string::iterator it = curr_line.begin() + count;
	std::string::iterator start_name = it;
	while(it != curr_line.end() && (*it != SPACE && *it != TAB))
	{
		++it;
		++count;
	}
	this->skipe_spaces(curr_line, &count);
	routes_name = std::string(start_name, it);
	*cursor += count;
	return routes_name;
}

void			http::Config::fill_server_name(std::string line, size_t i, size_t* cursor)
{
	*cursor += 11;
	std::string names = this->get_inline_value(line, cursor);
	this->servers[i].s_names = this->split_by_space_or_tab( std::string(names.begin(), names.end()));
}


std::vector<std::string>	http::Config::split_by_space_or_tab(std::string str)
{
	std::vector<std::string> splited;
	std::string::iterator it = str.begin();
	std::string::iterator it_end = str.end();
	std::string::iterator tmp;

	while (it != it_end)
	{
		if (*it != SPACE && *it != TAB)
		{
			tmp = it;
			while (it != it_end && (*it != SPACE && *it != TAB))
				++it;
			splited.push_back(std::string(tmp, it));
		}
		else
			++it;
	}
	return (splited);
}

void			http::Config::fill_errors_pages(std::string line, size_t i, size_t* cursor)
{
	*cursor += 10;
	std::string tmp = this->get_inline_value(line, cursor);
	std::vector<std::string> data = this->split_by_space_or_tab( std::string(tmp.begin(), tmp.end()));
	std::vector<std::string> errors_code_str(data.begin(), data.end() - 1);
	if (!this->is_valid_errors_code(errors_code_str))
	{
		this->setValidation(false);
		return ;
	}
	std::string errors_page = *(data.end() - 1);
	int errors_code;
	for (size_t j = 0; j < errors_code_str.size(); ++j)
	{
		errors_code = std::atoi(errors_code_str[j].c_str());
		this->servers[i].err_pages.insert(this->servers[i].err_pages.end(),\
			std::pair<int, std::string>(errors_code, errors_page));
	}
}

void			http::Config::fill_client_max_body_size( std::string line, size_t i,\
												size_t* cursor )
{
	*cursor += 20;
	std::string tmp = this->get_inline_value(line, cursor);
	this->servers[i].client_max_body_size = std::atoi(tmp.substr(0, tmp.size()).c_str());
}

bool			http::Config::is_empty(const std::string & str)
{ 
	size_t i =  (str.size() > 0) ? 1: 0;
	this->skipe_spaces(str, &i);
	if (str[i] != '\0')
		return false;
	return (true);
}


bool	http::Config::is_route_well_formated( std::vector<std::string> & data, size_t i,\
										 size_t* cursor )
{
	if (data[this->servers[i].start_data][*cursor] != OPEN_SYMBOL)
	{
		if (data[this->servers[i].start_data][*cursor] != '\0')
		{
			this->setValidation(false);
			return false;
		}
		this->servers[i].start_data += 1;
		size_t tmp_cursor = 0;
		this->skipe_spaces(data[this->servers[i].start_data], &tmp_cursor);
		if (data[this->servers[i].start_data][tmp_cursor] != OPEN_SYMBOL \
			|| !this->is_empty(data[this->servers[i].start_data].substr(tmp_cursor))
			)
		{
			this->setValidation(false);
			return false;
		}
	}
	else if (!this->is_empty(data[this->servers[i].start_data].substr(*cursor)))
	{
		this->setValidation(false);
		return false;
	}
	return true;
}

std::set<std::string>  http::Config::get_methods(std::string & str, size_t *cursor)
{
	std::vector<std::string> methods;
	*cursor += 6;
	std::string data = this->get_inline_value(str, cursor);
	methods = this->split_by_space_or_tab(std::string(data.begin(), data.end()));
	size_t count_post = 0, count_get = 0, count_del = 0;
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it)
	{
		if (*it == "POST" && count_post == 0)
			count_post += 1;
		else if (*it == "GET" && count_get == 0)
			count_get += 1;
		else if (*it == "HEAD" && count_get == 0)
			count_get += 1;
		else if (*it == "PUT" && count_del == 0)
			count_del += 1;
		else if (*it == "DELETE" && count_del == 0)
			count_del += 1;
		else
			this->setValidation(false);
	}
	std::set<std::string>	method_set(methods.begin(), methods.end());
	return method_set;
}

void			http::Config::set_dir_listing_options( std::string & str, route_t* route,\
													  size_t* cursor )
{
	*cursor += 17;
	std::string value = this->get_inline_value(str, cursor);
	if (value != "on" && value != "off")
	{
		this->setValidation(false);
		return ;
	}
	route->directory_listing = (value == "on")? true : false;
}

void			http::Config::set_root(std::string & str, route_t* route, size_t* cursor)
{
	*cursor += 4;
	std::string value = this->get_inline_value(str, cursor);
	route->root = value;
}

void			http::Config::set_index(std::string & str, route_t* route, size_t* cursor)
{
	*cursor += 5;
	std::string value = this->get_inline_value(str, cursor);
	route->index = value;
}

void			http::Config::set_upload(std::string & str, route_t* route, size_t* cursor)
{
	*cursor += 6;
	std::string value = this->get_inline_value(str, cursor);
	if (value != "on" && value != "off")
	{
		this->setValidation(false);
		return ;
	}
	route->upload = (value == "on")? true : false;
}

void			http::Config::set_redirect(std::string & str, route_t* route, size_t* cursor)
{
	*cursor += 8;
	std::string value = this->get_inline_value(str, cursor);
	route->redirect = value;
}

void			http::Config::set_cgi_ext(std::string & str, route_t* route, size_t* cursor)
{
	*cursor += 7;
	std::string value = this->get_inline_value(str, cursor);
	route->cgi_ext = value;
}

void			http::Config::set_cgi_pass(std::string & str, route_t* route, size_t* cursor)
{
	*cursor += 8;
	std::string value = this->get_inline_value(str, cursor);
	route->cgi_pass = value;
}

bool			http::Config::is_valid_server_nb(int nb_servers)
{
	if (nb_servers < 0)
	{
		this->setValidation(false);
		return false;
	}
	return true;
}

void			http::Config::fill_route_params( std::vector<std::string> & data, size_t i,\
											    size_t* cursor, std::string & route_name )
{
	this->skipe_empty_line(data, &this->servers[i].start_data);
	*cursor = 0;
	this->skipe_spaces(data[this->servers[i].start_data], cursor);
	if (data[this->servers[i].start_data].substr(*cursor, 6) == KEY_METHOD)
	{
		this->servers[i].routes[route_name].method = \
			this->get_methods(data[this->servers[i].start_data], cursor);
	}
	else if (data[this->servers[i].start_data].substr(*cursor, 17) == KEY_DIRECTORY_LISTING)
	{
		this->set_dir_listing_options( data[this->servers[i].start_data],\
									   &this->servers[i].routes[route_name], cursor );
	}
	else if (data[this->servers[i].start_data].substr(*cursor, 4) == KEY_ROOT)
	{
		this->set_root( data[this->servers[i].start_data], &this->servers[i].routes[route_name],\
					    cursor );
	}
	else if (data[this->servers[i].start_data].substr(*cursor, 5) == KEY_INDEX)
	{
		this->set_index( data[this->servers[i].start_data], &this->servers[i].routes[route_name],\
						 cursor );
	}
	else if (data[this->servers[i].start_data].substr(*cursor, 6) == KEY_UPLOAD)
	{
		this->set_upload( data[this->servers[i].start_data], &this->servers[i].routes[route_name],\
		                  cursor );
	}
	else if (data[this->servers[i].start_data].substr(*cursor, 8) == KEY_REDIRECT)
	{
		this->set_redirect( data[this->servers[i].start_data], &this->servers[i].routes[route_name],\
							cursor );
	}
	else if (data[this->servers[i].start_data].substr(*cursor, 7) == KEY_CGI_EXT)
	{
		this->set_cgi_ext( data[this->servers[i].start_data], &this->servers[i].routes[route_name],\
		                   cursor );
	}
	else if (data[this->servers[i].start_data].substr(*cursor, 8) == KEY_CGI_PASS)
	{
		this->set_cgi_pass( data[this->servers[i].start_data], &this->servers[i].routes[route_name],\
							cursor );
	}
	else
	{
		this->setValidation(false);
		return ;
	}
}

bool			http::Config::is_valid_host(std::string host)
{
	if (this->is_empty(host))
		return false;
	std::string octet = "";
	std::vector<std::string> all_octes;
	size_t cursor = 0, i = 0;
	while (i < host.size())
	{
		if (!isdigit(host[i]))
		{
			if ((host[i] == DOT) && cursor > 0)
			{
				all_octes.push_back(octet);
				octet = "";
				cursor = 0;
			}
			else
				return (false);
		}
		else
		{
			cursor += 1;
			octet += host[i];
		}
		++i;
	}
	all_octes.push_back(octet);
	if (!this->is_valid_ip(all_octes))
		return false;
	return (true);
}

bool			http::Config::is_valid_ip(std::vector<std::string> & all_octes)
{
	int octet;

	for(size_t i = 0; i < all_octes.size(); ++i)
	{
		octet = std::atoi(all_octes[i].c_str());
		if (octet > 255)
			return false;
	}
	return true;
}

bool			http::Config::is_valid_errors_code(std::vector<std::string> errors_code)
{
	for (std::vector<std::string>::iterator it = errors_code.begin(); it != errors_code.end(); ++it)
	{
		if (it->size() != 3)
			return false ;
		for (size_t i = 0; i < it->size(); ++i)
		{
			if (!isdigit((*it)[i]))
				return false ;
		}
	}
	return true ;
}


// canonical class form

http::Config::Config(Config const & other) : _file(other._file) {}

http::Config &	http::Config::operator=(Config const & rhs)
{
	this->_file = rhs._file;
	return (*this);
}
