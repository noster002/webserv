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

#include "../head/Helpers.hpp"

std::vector<std::string>	Helpers::get_config_data(const std::string & fileconf)
{
	std::vector<std::string>	data;
	std::ifstream				conf(fileconf);
	std::string					line;

	if (conf.is_open())
	{
		while(getline(conf, line))
		{
			data.push_back(line);
			std::cout << "-" <<line.size() << "-";
			std::cout << line << "\n";
		}
	}
	conf.close();
	return data;
}


void			Helpers::parse_file(ServerConf* servconf,\
	const std::string & fileconf)
{
	int							nb_server;
	std::fstream				conf_file(fileconf);
	std::vector<std::string>	data;

	data = get_config_data(fileconf);
	nb_server = count_servers(data, servconf);
	exit(0);
	size_t j;
	if (nb_server < 0)
	{
		servconf->setValidation(false);
		return ;
	}
	for (size_t i = 0; i < nb_server; ++i)
	{
		std::cout << "**********SERVER[ " << i << " ]*******************\n";
		while (servconf->servers[i].start_data <= servconf->servers[i].end_data)
		{
			j = 0;
			skipe_empty_line(data, &servconf->servers[i].start_data);
			skipe_spaces(data[servconf->servers[i].start_data], &j);
			if (data[servconf->servers[i].start_data].substr(j, 4) == KEY_HOST)
				fill_host_value(data[servconf->servers[i].start_data], servconf, i, &j);
			else if (data[servconf->servers[i].start_data].substr(j, 6) == KEY_LISTEN)
				fill_port_value(data[servconf->servers[i].start_data], servconf, i, &j);
			else if (data[servconf->servers[i].start_data].substr(j, 11) == KEY_SERV_NAME)
				fill_server_name(data[servconf->servers[i].start_data], servconf, i, &j);
			else if (data[servconf->servers[i].start_data].substr(j, 10) == KEY_ERR_PAGE)
				fill_errors_pages(data[servconf->servers[i].start_data], servconf, i, &j);
			else if (data[servconf->servers[i].start_data].substr(j, 20) == KEY_MAX_BODY_SIZE)
				fill_client_max_body_size(data[servconf->servers[i].start_data], servconf, i, &j);
			//else if (data[servconf->servers[i].start_data].substr(j, 5) == KEY_ROUTE)
			//	std::cout << "ICI\n";
			if (!servconf->getValidation())
				return ;
			servconf->servers[i].start_data += 1;
		}
		std::cout << "HOST : " << "  " << servconf->servers[i].host << "\n";
		std::cout << "PORT : " << "  " << servconf->servers[i].port[0] << "\n";
		std::cout << "PORT : " << "  " << servconf->servers[i].port[1] << "\n";
		std::cout << "SERVER NAME : " << "  " << servconf->servers[i].s_names[0] << "\n";
		std::cout << "SERVER NAME : " << "  " << servconf->servers[i].s_names[1] << "\n";
		std::map<std::vector<std::string>, std::string>::iterator err = servconf->servers[i].err_pages.begin();
		while (err != servconf->servers[i].err_pages.end())
		{
			for (size_t key = 0; key < err->first.size(); ++key)
			{
				std::cout << err->first[key] << " ";
			}
			std::cout << " == > " << err->second << "\n";
			++err;
		}
		std::cout << "MAX BODY : " << "  " << servconf->servers[i].client_max_body_size << "\n";
	}

}

int  			Helpers::count_servers(	const std::vector<std::string> & config_data,
										ServerConf* servconf )
{
	int				nb = 0;
	int				serv_open = -1;
	size_t			data_len = config_data.size(), j;
	for (size_t i = 0; i < data_len; ++i)
	{
		j = KEY_SERVER_LEN;
		skipe_empty_line(config_data, &i);
		if (config_data[i] == "" && nb >= 1)
			return (nb);
		if (config_data[i].substr(0, j) == KEY_SERVER)
		{
			skipe_spaces(config_data[i], &j);
			if (config_data[i][j] == '\0')
			{
				i += 1;
				j = 0;
			}
			if (config_data[i][j] == OPEN_SYMBOL)
			{
				if (j = 0)
					std::cout << "NO\n";
				params_t param;
				param.start_data = i + 1;
				if (find_close_symbol(config_data, &i, 0) >= 0)
				{
					nb += 1;
					param.end_data = i - 1;
					servconf->servers.push_back(param);
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

void			Helpers::skipe_spaces(const std::string & str, size_t* cursor)
{
	while (*cursor < str.size() && (str[*cursor] == SPACE || str[*cursor] == TAB))
		*cursor += 1;
}

int 			Helpers::find_close_symbol(const std::vector<std::string> & config_data,\
	size_t* cursor, int level)
{
	size_t				data_len = config_data.size(), j;
	*cursor += 1;
	while (*cursor < data_len)
	{
		j = 0;
		if (config_data[*cursor].substr(0, KEY_SERVER_LEN) == KEY_SERVER)
			return (-1);
		if (level > 0)
			skipe_spaces(config_data[*cursor], &j);
		if (config_data[*cursor][j] == CLOSE_SYMBOL)
		{
			if (j + 1 == config_data[*cursor].size())
				return (j);
			j += 1;
			skipe_spaces(config_data[*cursor], &j);
			if (config_data[*cursor][j] == '\0')
				return (j);
			return (-1);
		}
		*cursor += 1;
	}
	return (-1);
}


void			Helpers::skipe_empty_line(const std::vector<std::string> & config_data, size_t* cursor)
{
	size_t len = config_data.size(), j;

	while (*cursor < len)
	{
		if (config_data[*cursor] == "")
			*cursor += 1;
		else
		{
			j = 0;
			while (config_data[*cursor][j] == SPACE || config_data[*cursor][j] == TAB)
				j++;
			if (config_data[*cursor][j] != '\0')
				return ;
			*cursor += 1;
		}
	}
}

void			Helpers::fill_host_value(std::string line, ServerConf* servconf, size_t i, size_t* cursor)
{
	*cursor += 4;
	servconf->servers[i].host = get_inline_value(servconf, line, cursor);
}

void			Helpers::fill_port_value(std::string line, ServerConf* servconf, size_t i, size_t* cursor)
{
	*cursor += 6;
	std::string str_port = get_inline_value(servconf, line, cursor);
	for (size_t it = 0; it < str_port.size() - 1; ++it)
	{
		if (!isdigit(str_port[it]) && (str_port[it] != TAB && str_port[it] != SPACE))
		{
			servconf->setValidation(false);
			return;
		}
	}
	int port = std::atoi(str_port.c_str());
	servconf->servers[i].port.push_back(port);
}

void			Helpers::fill_server_name(std::string line, ServerConf* servconf, size_t i, size_t* cursor)
{
	*cursor += 11;
	size_t 	first = 0;
	bool	status = false;
	std::string names = get_inline_value(servconf, line, cursor);
	servconf->servers[i].s_names = split_by_space_or_tab(
					std::string(names.begin(), names.end() - 1)
				);
}

std::string		Helpers::get_inline_value(ServerConf* servconf, const std::string & line, size_t* cursor)
{
	std::string value = "";
	if (line[line.size() - 1] != SEMI_COLUMN)
	{
		servconf->setValidation(false);
		return value;
	}
	skipe_spaces(line, cursor);
	while (*cursor < line.size())
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

void			Helpers::fill_errors_pages(
					std::string line,ServerConf* servconf,
					size_t i,
					size_t* cursor
				)
{
	*cursor += 10;
	std::string tmp = get_inline_value(servconf, line, cursor);
	std::vector<std::string> data = split_by_space_or_tab(
					std::string(tmp.begin(), tmp.end() - 1)
				);
	std::vector<std::string> errors_code(data.begin(), data.end() - 1);
	std::string errors_page = *(data.end() - 1);
	servconf->servers[i].err_pages.insert(
					servconf->servers[i].err_pages.end(),
					std::pair<std::vector<std::string>, std::string>(errors_code, errors_page)
				);
}

void					Helpers::fill_client_max_body_size(
		std::string line,ServerConf* servconf, size_t i,
		size_t* cursor
		)
{
	*cursor += 20;
	std::string tmp = get_inline_value(servconf, line, cursor);
	servconf->servers[i].client_max_body_size = std::atoi(tmp.substr(0, tmp.size() - 1).c_str());
}

void					Helpers::fill_routes(std::vector<std::string> & data,
							ServerConf* servconf,
							size_t i,
							size_t* cursor
						)
{
	*cursor += 5;
	std::string route_name = get_route_name(servconf, data, i, cursor);
	exit(0);
}

std::string				Helpers::get_route_name(
						ServerConf* servconf,
						std::vector<std::string> & data,
						size_t i,
						size_t* cursor
					)
{
	return "";
}