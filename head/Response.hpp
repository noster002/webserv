/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:35:46 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/23 14:39:56 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

// C POSIX

# include <sys/socket.h>
# include <sys/types.h>

// C++98

# include <string>
# include <iostream>
# include <cstring>
# include <cerrno>

class	Request;

class	Response
{
	public:

		Response(void);
		~Response(void);

		void		build(Request const & request, std::string & output);

	private:

		struct				_header;
		std::string			_body;

		static std::map<int, std::string>	_status;
		void								_initstatus(void);

		// canonical class form

		Response(Response const & other);
		Response &	operator=(Response const & rhs);

};

struct	Response::_header
{
	std::string		protocol;
	int				status;
	std::string		location;
};

#endif
