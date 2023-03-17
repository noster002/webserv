/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 11:43:34 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/17 11:46:58 by nosterme         ###   ########.fr       */
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
			void				parse(void);
			void				clear(void);

		private:

			params_t			_server;

			std::string			_buffer;
			int					_error;

			t_request			_conf;

			bool				_is_body;

			bool				_header_complete(void) const;

			void				_read_first_line(size_t & pos);
			void				_read_method(std::string const & line, size_t & pos);
			void				_read_path(std::string const & line, size_t & pos);
			void				_read_version(std::string const & line, size_t & pos);
			std::string			_get_next_line(size_t & pos);
			std::string			_get_key(std::string line);
			std::string			_get_value(std::string line);
			std::string			_read_body(size_t pos);
			void				_process_header_fields(void);
			void				_process_path(void);
			void				_process_host(void);

			bool				_is_token(char c) const;
			bool				_is_whitespace(char c) const;

			static std::set<std::string>		_methods;
			static std::set<std::string>		_init_methods(void);

			class				BadRequestException;
			class				URITooLongException;
			class				NotImplementedException;
			class				ContentTooLargeException;
			class				HTTPVersionNotSupportedException;


			// canonical class form

			Request(void);
			Request(Request const & other);
			Request &	operator=(Request const & rhs);

	};

	class		Request::BadRequestException : public std::exception
	{
		public:

			char const *	what(void) const throw();

	};

	class		Request::URITooLongException : public std::exception
	{
		public:

			char const *	what(void) const throw();

	};

	class		Request::NotImplementedException : public std::exception
	{
		public:

			char const *	what(void) const throw();

	};

	class		Request::ContentTooLargeException : public std::exception
	{
		public:

			char const *	what(void) const throw();

	};

	class		Request::HTTPVersionNotSupportedException : public std::exception
	{
		public:

			char const *	what(void) const throw();

	};
}

#endif
