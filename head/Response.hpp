/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:35:46 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/09 13:39:40 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

// C POSIX

# include <sys/socket.h>
# include <sys/types.h>

// C++98

# include <map>
# include <string>
# include <iostream>
# include <cstring>
# include <cerrno>

namespace web
{
	class	Request;

	class	Response
	{
		public:

			Response(void);
			~Response(void);

			void		build(Request const * request, std::string & output);

		private:

			std::string			_protocol;
			int					_status;
			std::map<std::string, std::string>\
								_header;
			std::string			_body;

			static std::map<int, std::string>	_statuses;
			static std::map<int, std::string>	_init_statuses(void);

			// canonical class form

			Response(Response const & other);
			Response &	operator=(Response const & rhs);

	};
}

#endif
