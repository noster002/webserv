/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 11:43:34 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/28 13:07:23 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

// C POSIX

# include <sys/socket.h>
# include <sys/types.h>

// C++98

# include <string>
# include <iostream>
# include <cstring>
# include <cerrno>

// private

# include "Response.hpp"

enum e_method
{
	GET, POST, DELETE
};

class	Request
{
	public:

		Request(void);
		~Request(void);

		void		parse(std::string const & input, ssize_t len);

		friend void	Response::build(Request const & request, std::string & output);

	private:

		int					_method;
		std::string			_version;
		std::string			_path;
		std::string			_query;
		std::string			_host;
		int					_port;
		std::map<std::string, std::string>\
							_header;
		std::string			_body;


		// canonical class form

		Request(Request const & other);
		Request &	operator=(Request const & rhs);

};

#endif
