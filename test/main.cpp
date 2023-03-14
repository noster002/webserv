#include "../head/Helpers.hpp"

int	main()
{

	ServerConf serv("../conf/server.conf");
	if (serv.getValidation())
		std::cout << "VALID\n";
	else
		std::cout << "INVALID\n";
	return (0);
}