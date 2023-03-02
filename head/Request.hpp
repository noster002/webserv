/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 11:43:34 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/02 08:55:42 by nosterme         ###   ########.fr       */
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
# include <string>
# include <iostream>
# include <cstring>
# include <cerrno>

// private

//# include "Response.hpp"

class	Request
{
	public:

		Request(void);
		~Request(void);

		void		parse(std::string const & input, ssize_t len);

		//friend void	Response::build(Request const & request, std::string & output);

	private:

		std::string			_method;
		int					_version;
		std::string			_path;
		std::string			_query;
		std::string			_host;
		int					_port;
		std::map<std::string, std::string>\
							_header;
		std::string			_body;

		std::string			_get_next_line(std::string const & input, size_t & pos);
		std::string			_get_key(std::string const & line);
		std::string			_get_value(std::string const & line);
		int					_read_first_line(std::string const & input);
		int					_read_method(std::string const & line, size_t & pos);
		int					_read_path(std::string const & line, size_t & pos);
		int					_read_version(std::string const & line, size_t & pos);

		static std::set<std::string>		_methods;
		static std::set<std::string>		_init_methods(void);


		// canonical class form

		Request(Request const & other);
		Request &	operator=(Request const & rhs);

};

#endif
