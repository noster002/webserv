/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/06 14:42:55 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/06 15:01:43 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
# define ERROR_HPP
# include <iostream>

class	Error
{
	public:

		static int		arg(void);
		static int		conf_file(void);
		static int		socket(void);
		static int		bind(void);
		static int		listen(void);
		static int		accept(void);

};

#endif
