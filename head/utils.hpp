/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/13 12:16:41 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/23 10:03:56 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <map>
# include <set>

# define RESET "\033[0m"
# define BLACK "\033[30m"
# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"
# define WHITE "\033[37m"

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
	std::set<std::string>		method;
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
	std::map<int, std::string>			err_pages;
	size_t								client_max_body_size;
	std::map<std::string, route_t>		routes;
	std::vector<std::string>			files_extensions;
	size_t								start_data;
	size_t								end_data;

}				params_t;

typedef struct			s_request
{
	std::string								method;
	int										version;
	std::string								path;
	std::string								query;
	std::map<std::string, std::string>		header;
	std::string								host;
	int										port;
	std::vector<std::vector<std::string> >	transfer_encoding;
	size_t									content_length;
	std::string								body;
	std::vector<std::string>				chunks;
	std::map<std::string, std::string>		trailer;
}						t_request;

#endif
