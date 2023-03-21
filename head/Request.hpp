/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 11:43:34 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/21 19:05:59 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

// C POSIX

# include <sys/socket.h>
# include <sys/types.h>

// C++98

# include <map>
# include <set>
# include <exception>
# include <string>
# include <iostream>
# include <cstdlib>
# include <cctype>
# include <cstring>
# include <cerrno>

// http

# include "utils.hpp"

# define REQUEST_LINE_MAX_LEN 8000

namespace http
{

	class	Request
	{
		public:

			Request(params_t const & server);
			~Request(void);

			int					get_error(void) const;
			t_request const &	get_conf(void) const;

			void				add_buffer(std::string const & input);
			int					parse(void);

		private:

			params_t			_server;

			std::string			_buffer;
			int					_error;

			t_request			_conf;

			bool				_is_complete;

			bool				_is_body;

			bool				_header_complete(void) const;

			int					_read_first_line(size_t & pos);
			int					_read_method(std::string const & line, size_t & pos);
			int					_read_path(std::string const & line, size_t & pos);
			int					_read_version(std::string const & line, size_t & pos);
			std::string			_get_next_line(size_t & pos);
			std::string			_get_key(std::string line);
			std::string			_get_value(std::string line);
			int					_process_header_fields(void);
			int					_process_path(void);
			int					_process_host(void);
			int					_process_transfer_encoding(void);
			int					_process_content_length(void);
			std::string			_read_body(void);

			int					_bad_request(std::string const & error_msg);
			int					_content_too_large(std::string const & error_msg);
			int					_URI_too_long(std::string const & error_msg);
			int					_not_implemented(std::string const & error_msg);
			int					_HTTP_version_not_supported(std::string const & error_msg);

			static std::vector<std::string>\
								_split(std::string str, char delim);
			static bool			_is_token(char c);
			static bool			_is_whitespace(char c);

			static std::set<std::string>		_methods;
			static std::set<std::string>		_init_methods(void);


			// canonical class form

			Request(void);
			Request(Request const & other);
			Request &	operator=(Request const & rhs);

	};

}

#endif
