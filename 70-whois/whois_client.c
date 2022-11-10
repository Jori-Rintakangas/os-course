#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define PORT 43

char* server = "whois.ficora.fi";

char response[1024];
struct sockaddr_in server_addr;

int main(int argc, char const* argv[])
{
    struct hostent *server_info = gethostbyname(server);
	const char* target = argv[1];

	char ip[INET_ADDRSTRLEN];
	inet_ntop(server_info->h_addrtype, server_info->h_addr_list[0], ip, sizeof(ip));

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0)
    {
		printf("Socket creation failed\n");
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
    {
		printf("Invalid address\n");
		return -1;
	}

    int connection = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (connection != 0)
    {
		printf("Connection failed\n");
		return -1;
	}

	int bytes_sent = send(client_socket, target, strlen(target), 0);
    if (bytes_sent < 0)
    {
		printf("Sending request data failed\n");
        close(connection);
		return -1;
	}

	int r = read(client_socket, response, 2000);
    if (r < 0) 
    {
		printf("Reading response data failed\n");
        close(connection);
		return -1;
	}
	printf("%s\n", response);

	close(connection);
	return 0;
}
