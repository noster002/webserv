/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 12:35:46 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/17 15:03:24 by nosterme         ###   ########.fr       */
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

			Response(params_t const & conf);
			~Response(void);

			std::string const &					get_buffer(void) const;
			void								build(int error, t_request const & request);
			void								clear(void);

		private:

			params_t							_server;

			std::string							_buffer;

			std::string							_protocol;
			int									_status;
			std::map<std::string, std::string>	_header;
			std::string							_body;

			void								_serve_get_request(t_request const & request);
			void								_serve_post_request(t_request const & request);
			void								_serve_delete_request(t_request const & request);

			void								_serve_request_error(void);
			void								_serve_file_error(void);

			void								_set_status_line(void);
			void								_set_head(void);
			void								_set_body(void);

			static std::map<int, std::string>	_statuses;
			static std::map<int, std::string>	_init_statuses(void);

			// canonical class form

			Response(void);
			Response(Response const & other);
			Response &	operator=(Response const & rhs);

	};
}

# include "Request.hpp"

#endif
