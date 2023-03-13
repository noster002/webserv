/* ************************************************************************** */
/*                                                              */
/*                                                        :::      ::::::::   */
/*   Helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fakouyat <fakouyat@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 17:59:25 by fakouyat          #+#    #+#             */
/*   Updated: 2023/02/20 17:59:25 by fakouyat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Helpers.hpp"

void			Helpers::skipe_spaces(const std::string & str, size_t* cursor)
{
	while (*cursor < str.size() && (str[*cursor] == SPACE || str[*cursor] == TAB))
		*cursor += 1;
}

void			Helpers::skipe_empty_line ( const std::vector<std::string> & confdata,\
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

void			Helpers::fill_port_value( std::string line, web::Config* servconf,\
														size_t i, size_t* cursor)
{
	*cursor += 6;
	std::string port = get_inline_value(servconf, line, cursor);
	if (is_empty(port))
	{
		servconf->setValidation(false);
		return ;
	}
	for (size_t it = 0; it < port.size(); ++it)
	{
		if (!isdigit(port[it]) && (port[it] != TAB && port[it] != SPACE))
		{
			servconf->setValidation(false);
			return;
		}
	}
	servconf->servers[i].port.push_back(port);
}

void			Helpers::fill_server_name( std::string line, web::Config* servconf,\
															size_t i, size_t* cursor)
{
	*cursor += 11;
	std::string names = get_inline_value(servconf, line, cursor);
	servconf->servers[i].s_names = split_by_space_or_tab(
					std::string(names.begin(), names.end())
				);
}

std::string		Helpers::get_inline_value(web::Config* servconf, const std::string & line,\
																			size_t* cursor)
{
	std::string value = "";
	if (line[line.size() - 1] != SEMI_COLUMN)
	{
		servconf->setValidation(false);
		return value;
	}
	skipe_spaces(line, cursor);
	while (*cursor < line.size() - 1)
	{
		value += line[*cursor];
		*cursor += 1;
	}
	return value;
}

std::vector<std::string> Helpers::split_by_space_or_tab(std::string str)
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

void			Helpers::fill_errors_pages( std::string line,web::Config* servconf,\
															size_t i, size_t* cursor)
{
	*cursor += 10;
	std::string tmp = get_inline_value(servconf, line, cursor);
	std::vector<std::string> data = split_by_space_or_tab( std::string(tmp.begin(),\
																		tmp.end()) );
	std::vector<std::string> errors_code(data.begin(), data.end() - 1);
	if (!is_valid_errors_code(errors_code))
	{
		servconf->setValidation(false);
		return ;
	}
	std::string errors_page = *(data.end() - 1);
	servconf->servers[i].err_pages.insert( servconf->servers[i].err_pages.end(),\
		std::pair<std::vector<std::string>, std::string>(errors_code, errors_page) );
}

void					Helpers::fill_client_max_body_size( std::string line,\
									web::Config* servconf, size_t i, size_t* cursor)
{
	*cursor += 20;
	std::string tmp = get_inline_value(servconf, line, cursor);
	servconf->servers[i].client_max_body_size = std::atoi(tmp.substr(0, tmp.size()).c_str());
}

std::string				Helpers::get_route_name(
						web::Config* servconf,\
						std::vector<std::string> & data,\
						size_t i,\
						size_t* cursor
					)
{
	std::string curr_line = data[servconf->servers[i].start_data].substr(*cursor);
	size_t count = 0;
	std::string routes_name;

	skipe_spaces(curr_line, &count);
	std::string::iterator it = curr_line.begin() + count;
	std::string::iterator start_name = it;
	while(it != curr_line.end() && (*it != SPACE && *it != TAB))
	{
		++it;
		++count;
	}
	skipe_spaces(curr_line, &count);
	routes_name = std::string(start_name, it);
	*cursor += count;
	return routes_name;
}

bool	Helpers::is_empty(const std::string & str)
{ 
	size_t i =  (str.size() > 0) ? 1: 0;
	skipe_spaces(str, &i);
	if (str[i] != '\0')
		return false;
	return (true);
}

bool	Helpers::is_route_well_formated(
		std::vector<std::string> & data, \
		web::Config* servconf, \
		size_t i,\
		size_t* cursor )
{
	if (data[servconf->servers[i].start_data][*cursor] != OPEN_SYMBOL)
	{
		if (data[servconf->servers[i].start_data][*cursor] != '\0')
		{
			servconf->setValidation(false);
			return false;
		}
		servconf->servers[i].start_data += 1;
		size_t tmp_cursor = 0;
		skipe_spaces(data[servconf->servers[i].start_data], &tmp_cursor);
		if (data[servconf->servers[i].start_data][tmp_cursor] != OPEN_SYMBOL \
			|| !is_empty(data[servconf->servers[i].start_data].substr(tmp_cursor))
			)
		{
			servconf->setValidation(false);
			return false;
		}
	}
	else if (!is_empty(data[servconf->servers[i].start_data].substr(*cursor)))
	{
		servconf->setValidation(false);
		return false;
	}
	return true;
}

std::vector<std::string>  Helpers::get_methods(web::Config* servconf,
									std::string & str, size_t *cursor)
{
	std::vector<std::string> methods;
	*cursor += 6;
	std::string data = get_inline_value(servconf, str, cursor);
	methods = split_by_space_or_tab(std::string(data.begin(), data.end()));
	size_t count_post = 0, count_get = 0, count_del = 0;
	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it)
	{
		if (*it == "POST" && count_post == 0)
			count_post += 1;
		else if (*it == "GET" && count_get == 0)
			count_get += 1;
		else if (*it == "DELETE" && count_del == 0)
			count_del += 1;
		else
			servconf->setValidation(false);
	}
	return methods;
}

void	Helpers::set_dir_listing_options(std::string & str, web::Config* servconf, 
												route_t* route, size_t* cursor)
{
	*cursor += 17;
	std::string value = get_inline_value(servconf, str, cursor);
	if (value != "on" && value != "off")
	{
		servconf->setValidation(false);
		return ;
	}
	route->directory_listing = (value == "on")? true : false;
}

void	Helpers::set_root(std::string & str, web::Config* servconf, route_t* route, size_t* cursor)
{
	*cursor += 4;
	std::string value = get_inline_value(servconf, str, cursor);
	route->root = value;
}

void	Helpers::set_index(std::string & str, web::Config* servconf, route_t* route, size_t* cursor)
{
	*cursor += 5;
	std::string value = get_inline_value(servconf, str, cursor);
	route->index = value;
}

void	Helpers::set_upload(std::string & str, web::Config* servconf, route_t* route, size_t* cursor)
{
	*cursor += 6;
	std::string value = get_inline_value(servconf, str, cursor);
	route->upload = value;
}

void	Helpers::set_redirect(std::string & str, web::Config* servconf, route_t* route, size_t* cursor)
{
	*cursor += 8;
	std::string value = get_inline_value(servconf, str, cursor);
	route->upload = value;
}

void	Helpers::set_cgi_ext(std::string & str, web::Config* servconf, route_t* route, size_t* cursor)
{
	*cursor += 7;
	std::string value = get_inline_value(servconf, str, cursor);
	route->cgi_ext = value;
}

void	Helpers::set_cgi_pass(std::string & str, web::Config* servconf, route_t* route, size_t* cursor)
{
	*cursor += 8;
	std::string value = get_inline_value(servconf, str, cursor);
	route->cgi_pass = value;
}

bool						Helpers::is_valid_server_nb(web::Config* servconf, int nb_servers)
{
	if (nb_servers < 0)
	{
		servconf->setValidation(false);
		return false;
	}
	return true;
}

void	Helpers::fill_route_params(std::vector<std::string> & data, web::Config* servconf,\
										size_t i, size_t* cursor, std::string & route_name)
{
	skipe_empty_line(data, &servconf->servers[i].start_data);
	*cursor = 0;
	skipe_spaces(data[servconf->servers[i].start_data], cursor);
	if (data[servconf->servers[i].start_data].substr(*cursor, 6) == KEY_METHOD)
	{
		servconf->servers[i].routes[route_name].method = get_methods(servconf, \
									data[servconf->servers[i].start_data], cursor);
	}
	else if (data[servconf->servers[i].start_data].substr(*cursor, 17) == KEY_DIRECTORY_LISTING)
	{
		set_dir_listing_options(data[servconf->servers[i].start_data],\
						servconf, &servconf->servers[i].routes[route_name], cursor);
	}
	else if (data[servconf->servers[i].start_data].substr(*cursor, 4) == KEY_ROOT)
	{
		set_root(data[servconf->servers[i].start_data], servconf, 
				&servconf->servers[i].routes[route_name], cursor);
	}
	else if (data[servconf->servers[i].start_data].substr(*cursor, 5) == KEY_INDEX)
	{
		set_index(data[servconf->servers[i].start_data], servconf, 
				&servconf->servers[i].routes[route_name], cursor);
	}
	else if (data[servconf->servers[i].start_data].substr(*cursor, 6) == KEY_UPLOAD)
	{
		set_upload(data[servconf->servers[i].start_data], servconf, 
				&servconf->servers[i].routes[route_name], cursor);
	}
	else if (data[servconf->servers[i].start_data].substr(*cursor, 8) == KEY_REDIRECT)
	{
		set_redirect(data[servconf->servers[i].start_data], servconf, 
				&servconf->servers[i].routes[route_name], cursor);
	}
	else if (data[servconf->servers[i].start_data].substr(*cursor, 7) == KEY_CGI_EXT)
	{
		set_cgi_ext(data[servconf->servers[i].start_data], servconf, 
				&servconf->servers[i].routes[route_name], cursor);
	}
	else if (data[servconf->servers[i].start_data].substr(*cursor, 8) == KEY_CGI_PASS)
	{
		set_cgi_pass(data[servconf->servers[i].start_data], servconf, 
				&servconf->servers[i].routes[route_name], cursor);
	}
	else
	{
		servconf->setValidation(false);
		return ;
	}
}


bool	Helpers::is_valid_host(std::string host)
{
	if (is_empty(host))
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
	if (!is_valid_ip(all_octes))
		return false;
	return (true);
}

bool		Helpers::is_valid_ip(std::vector<std::string> & all_octes)
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

bool	Helpers::is_valid_errors_code(std::vector<std::string> errors_code)
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
