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
# include <cstdlib>
# include <ctype.h>
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
# define DOT '.'
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
	std::vector<std::string>			port;
	std::vector<std::string>			s_names;
	std::map<std::vector<std::string>,\
			std::string>				err_pages;
	int									client_max_body_size;
	std::map<std::string, route_t>		routes;
	std::vector<std::string>			files_extensions;
	size_t								start_data;
	size_t								end_data;

}				params_t;

namespace	web
{
	class	Config;
}

class	Helpers {

public:																	
	static void						skipe_spaces(const std::string & str, size_t* cursor);

	static	void					skipe_empty_line(const std::vector<std::string> & confdata,\
																				size_t* cursor);
	static	void					fill_host_value(std::string line, web::Config* servconf,\
																		size_t i, size_t* cursor);
	static	std::string				get_inline_value(web::Config* servconf,\
														const std::string & line, size_t* cursor);
	static	void					fill_port_value(std::string line, web::Config* servconf,\
																		size_t i, size_t* cursor);
	static	void					fill_server_name(std::string line, web::Config* servconf,\
																		size_t i,size_t* cursor);
	static	void					fill_errors_pages(std::string line, web::Config* servconf,\
																		size_t i,size_t* cursor);
	static	void					fill_client_max_body_size(std::string line,\
												web::Config* servconf, size_t i, size_t* cursor);
	static std::vector<std::string> split_by_space_or_tab(std::string str);
	static std::string				get_route_name(web::Config* servconf,\
									std::vector<std::string> & data, size_t i, size_t* cursor);
																				
	static bool						is_empty(const std::string & str);
	static bool						is_route_well_formated(std::vector<std::string> & data,\
												web::Config* servconf, size_t i, size_t* cursor);
	static std::vector<std::string> get_methods(web::Config* servconf,
											std::string & str, size_t *cursor);
	static void						set_dir_listing_options(std::string & str,\
									web::Config* servconf, route_t* route, size_t* cursor);
	static void						set_root(std::string & str, web::Config* servconf,\
															route_t* route, size_t* cursor);
	static void						set_index(std::string & str, web::Config* servconf,\
															route_t* route, size_t* cursor);	
	static void						set_upload(std::string & str, web::Config* servconf,\
															route_t* route, size_t* cursor);
	static void						set_redirect(std::string & str, web::Config* servconf,\
																route_t* route, size_t* cursor);
	static void						set_cgi_ext(std::string & str, web::Config* servconf,\
																route_t* route, size_t* cursor);
	static void						set_cgi_pass(std::string & str, web::Config* servconf,\
																route_t* route, size_t* cursor);
	static bool						is_valid_server_nb(web::Config* servconf, int nb_servers);
	static void						fill_route_params(std::vector<std::string> & data,\
					web::Config* servconf, size_t i, size_t* cursor, std::string & route_name);
	static bool						is_valid_host(std::string host);
	static bool						is_valid_ip(std::vector<std::string> & all_octes);
	static bool						is_valid_errors_code(std::vector<std::string> errors_code);
};

# include "Config.hpp"

#endif
