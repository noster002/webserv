/* ************************************************************************** */
/*                                                                            */
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

std::vector<std::string>    Helpers::get_config_data(const std::string & fileconf)
{
    std::vector<std::string>    data;
    std::ifstream               conf(fileconf);
    std::string                 line;

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


void                        Helpers::parse_file(ServerConf* servconf,
                                                    const std::string & fileconf)
{
    int                      nb_server;
    std::fstream             conf_file(fileconf);
    std::vector<std::string> data;

    data = get_config_data(fileconf);
    nb_server = count_servers(data);
    std::cout << "COUNT SERVER" << nb_server << "\n";
    if (nb_server < 0)
    {
        servconf->setValidation(false);
        return ;
    }

}

int                         Helpers::count_servers(const std::vector<std::string> & config_data)
{
    int nb = 0;
    const std::string key = "server";
    const char        open_symbol = '{';
    int               serv_open = -1;
    size_t data_len = config_data.size(), j;

    for (size_t i = 0; i < data_len; i++)
    {
        j = key.size();
        if (config_data[i].substr(0, j) == key)
        {
            skipe_spaces(config_data[i], &j);
            if (config_data[i][j] == open_symbol)
            {
                if (find_close_symbol(config_data, &i, 0) >= 0)
                    nb += 1;
                else
                    return (-1);
            }
            else
                return (-1);
        }
    }
    return (nb);
}

void                        Helpers::skipe_spaces(const std::string & str, size_t* cursor)
{
    while (str[*cursor] == ' ')
        *cursor += 1;
}

int                        Helpers::find_close_symbol(const std::vector<std::string> & config_data,
                                                        size_t* cursor, int level)
{
    size_t              data_len = config_data.size(), j;
    const char          close_symbole = '}';
    const std::string   key = "server";

    *cursor += 1;
    while (*cursor < data_len)
    {
        j = 0;
        if (config_data[*cursor].substr(0, key.size()) == key)
            return (-1);
        if (level > 0)
            skipe_spaces(config_data[*cursor], &j);
        if (config_data[*cursor][j] == close_symbole)
        {
            if (j + 1 == config_data[*cursor].size())
                return (j);
            return (-1);
        }
        *cursor += 1;
    }
    return (-1);
}