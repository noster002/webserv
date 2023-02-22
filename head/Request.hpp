/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/22 11:43:34 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/22 15:00:23 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>

class	Response;

class	Request
{
	public:

		Request(void);
		~Request(void);

		void		parse(std::string const & input, ssize_t len);

		friend void	Response::build(Request const & request, std::string & output);

	private:

		// canonical class form

		Request(Request const & other);
		Request &	operator=(Request const & rhs);

};

#endif
