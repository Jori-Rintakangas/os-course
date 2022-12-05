/* This is a whois client program that can be used to find information of
a specific domain or a website. Whois protocol is specified in RFC3912 standard
and it is very simple: First we open a TCP connection to the whois server
listening on port 43. Then we send a query string that contains the domain
or a website name we are interested in, appended with <CR><LF>. The whois server
will then respond with the information related to the query string if the domain
or website is found. This client uses whois.ficora.fi as whois server.
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>

#define PORT 43 /* whois servers listen on TCP port 43 */
#define BUF_SIZE 1024

char* server = "whois.ficora.fi";
struct sockaddr_in server_addr;


/* print_response() prints the response from the socket file descriptor */
int print_response(int client_socket)
{
	char response_buf[BUF_SIZE];
	int bytes = 0;
	/* Reading bytes from clien_socket file descriptor to response_buf
	and printing the buffer contents. */
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

/* set_socket_info() assigns values of the sever to socket structure */
void set_socket_info()
{
	char ip[INET_ADDRSTRLEN];
	struct hostent* server_info = gethostbyname(server);
	/* Get whois.ficora.fi server ip address and store it to ip */
	inet_ntop(server_info->h_addrtype, server_info->h_addr_list[0], ip, sizeof(ip));

	/* Storing internet address info to socket struct */
	inet_pton(AF_INET, ip, &server_addr.sin_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
}

/* main() estabilishes a TCP connection to whois.ficora.fi server, sends a request
to a server and prints the response. */
int main(int argc, char const* argv[])
{
	if (argc != 2)
	{
		printf("Usage: ./whois <domain>\n");
		return 1;
	}

	/* Creating a socket file descriptor with: AF_INET for IPv4, SOCK_STREAM for TCP and 0
	to use address family default protocol */
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0)
	{
		printf("Socket creation failed\n");
		return -1;
	}

	/* Setting socket structure elements to NULL to remove trash values */
	bzero(&server_addr, sizeof(server_addr));
	set_socket_info();

	/* Connecting a socket file descriptor to the server, i.e. open TCP connection to whois server */
	int connection = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (connection != 0)
	{
		printf("Connection failed\n");
		close(client_socket);
		return -1;
	}

	/* Appending <CR><LF> to the end of request string specified by user.
	The whois protocol (RFC3912) specifies that the query string ends with <CR><LF> */
	char* query = malloc(strlen(argv[1]) + 3);
	strcpy(query, argv[1]);
	strcat(query, "\r\n");

	/* Sending a query message to the file descriptor socket */
	if (send(client_socket, query, strlen(query), 0) < 0)
	{
		printf("Sending request failed\n");
		close(client_socket);
		free(query);
		return -1;
	}

	if (!print_response(client_socket))
	{
		close(client_socket);
		free(query);
		return -1;
	}

	close(client_socket);
	free(query);
	return 0;
}
