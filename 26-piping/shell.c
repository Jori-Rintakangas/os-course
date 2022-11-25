#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int check_input(int argc, char const* argv[])
{
	if (argc != 2 && argc != 4)
	{
		printf("Usage: ./shell <program>\nUsage: ./shell <program1> | <program2>\n");
		return 0;
	}
	else if (argc == 4 && *argv[2] != 'p')
	{
		printf("Usage: ./shell <program>\nUsage: ./shell <program1> | <program2>\n");
		return 0;
	}
	return 1;
}

int main(int argc, char const* argv[])
{
	if (!check_input(argc, argv))
	{
		return 1;
	}
	char* PATH;
	PATH = getenv("PATH");

	const char delim[2] = ":";
	char *part;
   	part = strtok(PATH, delim);
   	while (part != NULL)
	{
		printf("%s\n", part);
    	part = strtok(NULL, delim);
   	}
	return 0;
}
