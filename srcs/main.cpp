/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fakouyat <fakouyat@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 10:31:09 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/13 12:08:25 by fakouyat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../head/Server.hpp"
#include "../head/Config.hpp"
#include "../head/Error.hpp"

int		main(int argc, char** argv)
{
	if (argc != 2)
		return (Error::arg());

	web::Config	servers(argv[1]);

	servers.run();

	return (0);
}
