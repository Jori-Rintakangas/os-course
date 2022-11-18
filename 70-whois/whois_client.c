#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>

#define PORT 43
#define BUF_SIZE 10

char* server = "whois.ficora.fi";
struct sockaddr_in server_addr;

int print_response(int client_socket)
{
	char response_buf[BUF_SIZE];
	int bytes = 0;
	while (bytes = read(client_socket, response_buf, sizeof(response_buf) - 1))
	{
		if (bytes < 0)
		{
			printf("Reading response data failed\n");
			return 0;
		}
		response_buf[bytes] = '\0';
		printf("%s", response_buf);
	}
	return 1;
}

void init_server_info()
{
	char ip[INET_ADDRSTRLEN];
	struct hostent* server_info = gethostbyname(server);
	// Get whois.ficora.fi server ip address and store it to ip
	inet_ntop(server_info->h_addrtype, server_info->h_addr_list[0], ip, sizeof(ip));

	// Storing internet address info to socket struct
	inet_pton(AF_INET, ip, &server_addr.sin_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
}

int main(int argc, char const* argv[])
{
	if (argc != 2)
	{
		printf("Usage: ./whois <domain>\n");
		return 1;
	}

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0)
    {
		printf("Socket creation failed\n");
		return -1;
	}

	init_server_info();

    int connection = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (connection != 0)
    {
		printf("Connection failed\n");
		close(client_socket);
		return -1;
	}

	const char* domain = argv[1];
    if (send(client_socket, domain, strlen(domain), 0) < 0)
    {
		printf("Sending request failed\n");
		close(connection);
		close(client_socket);
		return -1;
	}

	if (!print_response(client_socket))
	{
		close(connection);
		close(client_socket);
		return -1;
	}

	close(connection);
	close(client_socket);
	return 0;
}
