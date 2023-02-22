/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fakouyat <fakouyat@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 17:54:06 by fakouyat          #+#    #+#             */
/*   Updated: 2023/02/20 17:54:06 by fakouyat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include "../head/Helpers.hpp"
#include <string>

int main(int argc, char *argv[])
{
    std::string filename = "../conf/server.conf";
    ServerConf servconf(filename.c_str());
    if (servconf.getValidation())
        std::cout << "VALID CONFIG\n";
    else
        std::cout << "INVALID CONFIG\n";
}