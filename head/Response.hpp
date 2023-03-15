/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:35:46 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/15 11:55:37 by nosterme         ###   ########.fr       */
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

namespace http
{
	class	Request;

	class	Response
	{
		public:

			Response(void);
			~Response(void);

			std::string const &					get_buffer(void) const;
			void								build(Request const & request);
			void								clear(void);

		private:

			std::string							_buffer;

			std::string							_protocol;
			int									_status;
			std::map<std::string, std::string>	_header;
			std::string							_body;

			static std::map<int, std::string>	_statuses;
			static std::map<int, std::string>	_init_statuses(void);

			// canonical class form

			Response(Response const & other);
			Response &	operator=(Response const & rhs);

	};
}

#endif
