/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 11:43:34 by nosterme          #+#    #+#             */
/*   Updated: 2023/04/12 15:30:52 by nosterme         ###   ########.fr       */
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

namespace http
{

	class	Request
	{
		public:

			Request(void);
			~Request(void);

			int					get_error(void) const;
			t_request const &	get_conf(void) const;

			void				add_buffer(char const * input, ssize_t bytes);
			bool				parse(void);

		private:

			params_t			_server;

			std::string			_buffer;
			int					_error;

			t_request			_conf;

			bool				_is_complete;

			bool				_is_body;

			int					_read_first_line(size_t & pos);
			int					_read_method(std::string const & line, size_t & pos);
			int					_read_path(std::string const & line, size_t & pos);
			int					_read_version(std::string const & line, size_t & pos);
			std::string			_get_next_line(std::string const & str, size_t & pos);
			std::string			_get_key(std::string line);
			std::string			_get_value(std::string line);
			int					_process_header_fields(void);
			int					_process_path(void);
			int					_process_host(void);
			int					_process_transfer_encoding(void);
			int					_process_content_length(void);
			int					_process_content_type(void);
			int					_read_body(void);
			void				_decode_chunk(std::string const & chunk, size_t & pos);
			int					_read_chunk_size(std::string const & line, size_t & size);
			int					_read_chunk(std::string const & chunk, size_t size, size_t & pos);

			int					_bad_request(std::string const & error_msg);
			int					_URI_too_long(std::string const & error_msg);
			int					_not_implemented(std::string const & error_msg);
			int					_HTTP_version_not_supported(std::string const & error_msg);

			static bool			_binary(std::string const & str);
			static bool			_section_complete(std::string const & str);
			static std::vector<std::string>\
								_split(std::string const & str, char const * delim);
			static bool			_is_token(char c);
			static bool			_is_whitespace(char c);

			static std::set<std::string>		_methods;
			static std::set<std::string>		_init_methods(void);


			// canonical class form

			Request(Request const & other);
			Request &	operator=(Request const & rhs);

	};

}

#endif
