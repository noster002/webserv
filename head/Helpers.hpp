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
# define OPEN_SYMBOL '{'
# define CLOSE_SYMBOL '}'
# define SEMI_COLUMN ';'
# define KEY_SERVER "server"
# define KEY_SERVER_LEN 6
# define SPACE ' '
# define KEY_LISTEN "listen"
# define KEY_SERV_NAME "server_name"
# define KEY_ERR_PAGE "error_page"
# define KEY_MAX_BODY_SIZE "client_max_body_size"
# define KEY_ROUTE "route"
# define KEY_METHOD "method"
# define KEY_DIRECTORY_LISTING "directory_listing"
# define KEY_ROOT "root"
# define KEY_INDEX "index"
# define KEY_UPLOAD "upload"
# define KEY_REDIRECT "redirect"
# define KEY_CGI_EXT "cgi_ext"
# define KEY_CGI_PASS "cgi_pass"
# define KEY_HOST "host"
# define TAB 9
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
	std::map<std::vector<std::string>,\
			 std::string>				err_pages;
	int									client_max_body_size;
	std::map<std::string, route_t>		routes;
	std::vector<std::string>			files_extensions;
	int									start_data;
	int									end_data;

}				params_t;

# include "ServerConf.hpp"

class ServerConf;

class	Helpers {

	public:
		static void						parse_file(ServerConf* servconf, 
														const std::string & fileconf);
		static std::vector<std::string>	get_config_data(const std::string & fileconf);
		static int						count_servers(const std::vector<std::string> & config_data, ServerConf* servconf);
		static void						skipe_spaces(const std::string & str, size_t* cursor);
		static int						find_close_symbol(const std::vector<std::string> & config_data, 
														  size_t* cursor, int level);
		static	void					skipe_empty_line(const std::vector<std::string> & config_data, size_t* cursor);
		static	int						fill_host_value(std::string line, params_t* serv, size_t* cursor);
};

#endif