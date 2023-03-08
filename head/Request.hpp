/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 11:43:34 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/08 17:21:30 by nosterme         ###   ########.fr       */
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

// private

namespace web
{
	//# include "Response.hpp"

	class	Request
	{
		public:

			Request(size_t client_max_body_size);
			~Request(void);

			void		parse(std::string const & input, ssize_t len);

			//friend void	Response::build(Request const & request, std::string & output);

		private:

			int					_error;

			std::string			_method;
			int					_version;
			std::string			_path;
			std::string			_query;
			std::string			_host;
			int					_port;
			std::map<std::string, std::string>\
								_header;
			std::string			_body;

			size_t				_client_max_body_size;

			void				_read_first_line(std::string const & input);
			void				_read_method(std::string const & line, size_t & pos);
			void				_read_path(std::string const & line, size_t & pos);
			void				_read_version(std::string const & line, size_t & pos);
			std::string			_get_next_line(std::string const & input, size_t & pos);
			std::string			_get_key(std::string line);
			std::string			_get_value(std::string line);
			std::string			_read_body(std::string input, size_t pos);
			void				_process_header_fields(void);
			void				_process_path(void);
			void				_process_host(void);

			bool				_is_token(char c) const;
			bool				_is_whitespace(char c) const;

			static std::set<std::string>		_methods;
			static std::set<std::string>		_init_methods(void);

			class				BadRequestException;
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
