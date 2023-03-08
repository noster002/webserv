/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:47 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/08 17:59:01 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP
# include <string>
# include "Helpers.hpp"
# include "Server.hpp"

namespace web
{
	class	ServerConf
	{
		public:
			ServerConf(char const * filename);
			~ServerConf(void);

			int						parse(void);
			bool					getValidation() const;
			void					setValidation(bool status);
			std::vector<params_t>	servers;	

			void					run(void);

		private:
			bool 					validation;
			std::string				_file;

			// canonical class form
			ServerConf(void);
			ServerConf(ServerConf const & other);
			ServerConf &			operator=(ServerConf const & rhs);
	};
}


#endif
