/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 10:31:09 by nosterme          #+#    #+#             */
/*   Updated: 2023/02/23 12:20:20 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../head/Server.hpp"
#include "../head/ServerConf.hpp"
#include "../head/Error.hpp"

int		main(int argc, char** argv)
{
	if (argc != 2)
		return (Error::arg());

	ServerConf	servers(argv[1]);

	servers.run();

	return (0);
}
