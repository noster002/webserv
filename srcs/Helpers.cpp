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
		}
	}
	conf.close();

	return data;
}


void	Helpers::parse_file(ServerConf* servconf,\
	const std::string & fileconf)
{
	int							nb_server;
	std::fstream				conf_file(fileconf);
	std::vector<std::string>	data;

	data = get_config_data(fileconf);
	nb_server = count_servers(data, servconf);
	size_t j;
	if (nb_server < 0)
	{
		servconf->setValidation(false);
		return ;
	}
	for (size_t i = 0; i < nb_server; i++)
	{
		std::cout << "**********SERVER*******************\n";
		while (servconf->servers[i].start_data <= servconf->servers[i].end_data)
		{
			j = 0;
			skipe_spaces(data[servconf->servers[i].start_data], &j);
			if (data[servconf->servers[i].start_data].substr(j, 4) == KEY_HOST)
			{
				if (fill_host_value(data[servconf->servers[i].start_data], &servconf->servers[i], &j) < 0)
				{
					servconf->setValidation(false);
					return ;
				}
			}
			servconf->servers[i].start_data += 1;
		}
		std::cout << "HOST : " << servconf->servers[i].host << "\n";
		std::cout << "*************************************\n";
	}

}

int  Helpers::count_servers(const std::vector<std::string> & config_data, ServerConf* servconf)
{
	int				nb = 0;
	int				serv_open = -1;
	size_t			data_len = config_data.size(), j;

	for (size_t i = 0; i < data_len; i++)
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
				params_t param;
				param.start_data = i += 1;
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

void	Helpers::skipe_spaces(const std::string & str, size_t* cursor)
{
	while (str[*cursor] == SPACE || str[*cursor] == TAB)
		*cursor += 1;
}

int Helpers::find_close_symbol(const std::vector<std::string> & config_data,\
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


void	Helpers::skipe_empty_line(const std::vector<std::string> & config_data, size_t* cursor)
{
	size_t len = config_data.size(), j;

	while (*cursor < len)
	{
		if (config_data[*cursor] == "")
			*cursor += 1;
		else 
		{
			j = 0;
			while (config_data[*cursor][j] == SPACE)
				j++;
			if (config_data[*cursor][j] != '\0')
				return ;
			*cursor += 1;
		}
	}
}

int	Helpers::fill_host_value(std::string line, params_t* serv, size_t* cursor)
{
	std::string	host = "";
	*cursor += 4;
	if (line[line.size() - 1] != SEMI_COLUMN)
		return (-1);
	skipe_spaces(line, cursor);
	while (*cursor < line.size())
	{
		host += line[*cursor];
		*cursor += 1;
	}
	std::cout << "HERE : " << host << "\n";
	return(0);
}