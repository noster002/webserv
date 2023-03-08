/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fakouyat <fakouyat@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:47 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/23 22:51:11 by fakouyat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP
# include <string>
# include "../head/Helpers.hpp"

class	ServerConf
{
	public:
		ServerConf(char const * filename);
		~ServerConf(void);

		int						parse(void);
		bool					getValidation() const;
		void					setValidation(bool status);
		std::vector<params_t>	servers;	

	private:
		bool 					validation;
		std::string				_file;

		// canonical class form
		ServerConf(void);
		ServerConf(ServerConf const & other);
		ServerConf &			operator=(ServerConf const & rhs);
};

#endif
