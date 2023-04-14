#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int	main(int argc, char const *argv[])
{
	int					sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in	addr;
	char *				message = "GET / HTTP/1.1\r\nHost: not.default.server:8080\r\n\r\n";
	char				buffer[1024];

	if (sock < 0)
	{
		printf("\nSocket creation error\n");
		return (-1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);

	if(inet_pton(AF_INET, "10.12.13.71", &addr.sin_addr) <= 0)
	{
		printf("\nInvalid address / Address not supported\n");
		return (-1);
	}

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("\nConnection Failed\n");
		return (-1);
	}
	write(sock, message, strlen(message));
	printf("\nRequest:\n%s\n", message);
	read(sock, buffer, 1024);
	printf("\nResponse:\n%s\n", buffer);
	return (0);
}
