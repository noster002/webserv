/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:35:46 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/24 15:40:42 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

// C POSIX

# include <sys/socket.h>
# include <sys/types.h>
# include <dirent.h>

// C++98

# include <map>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <cstring>
# include <cerrno>

// http

# include "utils.hpp"

namespace http
{
	class	Request;

	class	Response
	{
		public:

			Response(void);
			~Response(void);

			std::string const &					get_buffer(void) const;
			void								set_server(params_t const & server);
			void								build(int error, t_request const & request);

		private:

			params_t							_server;

			std::string							_buffer;

			std::string							_protocol;
			int									_status;
			std::map<std::string, std::string>	_header;
			std::string							_body;

			int									_serve_get_request(t_request const & request);
			void								_serve_post_request(t_request const & request);
			void								_serve_delete_request(t_request const & request);

			int									_get_path(t_request const & request, std::string & path);
			int									_directory_listing(t_request const & request, std::string const & path);

			void								_serve_error(void);
			void								_serve_error_file(std::fstream & file);
			void								_serve_error_plain(void);

			void								_set_content_type(std::string const & path);

			void								_set_status_line(void);
			void								_set_head(void);
			void								_set_body(void);

			int									_OK(void);
			int									_permanent_redirect(std::string const & path);
			int									_not_found(void);
			int									_method_not_allowed(std::string const & path);
			int									_gone(void);
			int									_content_too_large(std::string const & error_msg);

			static std::map<int, std::string>	_statuses;
			static std::map<int, std::string>	_init_statuses(void);
			static std::map<int, std::string>	_default_err_pages;
			static std::map<int, std::string>	_init_default_err_pages(void);
			static std::map<std::string, std::string>	_MIME_types;
			static std::map<std::string, std::string>	_init_MIME_types(void);

			// canonical class form

			Response(Response const & other);
			Response &	operator=(Response const & rhs);

	};
}

# include "Request.hpp"

#endif
