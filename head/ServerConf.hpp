/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:47 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/06 13:34:42 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP
# include <string>

class	ServerConf
{
	public:

		ServerConf(char const * filename);
		~ServerConf(void);

		int		parse(void);

	private:

		std::string		_file;

		// canonical class form
		ServerConf(void);
		ServerConf(ServerConf const & other);
		ServerConf &	operator=(ServerConf const & rhs);

};

#endif
