/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helpers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fakouyat <fakouyat@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 17:59:20 by fakouyat          #+#    #+#             */
/*   Updated: 2023/02/20 17:59:20 by fakouyat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HELPERS_HPP
# define HELPERS_HPP
# include <iostream>
# include <fstream>
# include <vector>
# include <map>

typedef struct	s_route {
	std::vector<std::string> 	method;
	bool						directory_listing;
	std::string					root;
	std::string					index;
	std::string					upload;
	std::string					redirect;
	std::string					cgi_ext;
	std::string					cgi_pass;				
}				route_t;

typedef struct s_params {
	std::string							host;
	int 								port;
	std::vector<std::string>			names;
	std::map<std::string, std::string>	err_pages;	
	int									client_max_body_size;
	std::map<std::string, route_t>		routes;
	std::vector<std::string>			files_extensions;
	
	
}				params_t;

# include "ServerConf.hpp"

class ServerConf;

class	Helpers {

	public:
		static void						parse_file(ServerConf* servconf, 
														const std::string & fileconf);
		static std::vector<std::string>	get_config_data(const std::string & fileconf);
		static int						count_servers(const std::vector<std::string> & config_data);
		static void						skipe_spaces(const std::string & str, size_t* cursor);
		static int						find_close_symbol(const std::vector<std::string> & config_data, 
														  size_t* cursor, int ni);
};

#endif