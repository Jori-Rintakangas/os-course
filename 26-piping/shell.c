#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int find_program(char** parsed_line)
{
	char cwd[1024];
    getcwd(cwd, sizeof(cwd));

	char* path = getenv("PATH");
	char temp[strlen(path) + 1];
	strcpy(temp, path);

   	char* part = strtok(temp, ":");
   	while (part != NULL)
	{
		chdir(part);
		if (access(parsed_line[0], F_OK) == 0)
		{
			chdir(cwd);
			return 1;
		}
		part = strtok(NULL, ":");
   	}
	chdir(cwd);
	printf("%s not found from PATH\n", parsed_line[0]);
	return 0;
}

char** parse_line(char* line)
{
	char** parsed_line = NULL;
	char** temp = NULL;
   	char* part = strtok(line, " ");
	int part_count = 1;
   	while (part != NULL)
	{
		temp = realloc(parsed_line, sizeof(char*) * part_count);
		if (temp == NULL)
		{
			printf("Memory allocation failure\n");
			free(parsed_line);
			free(line);
			exit(-1);
		}
		parsed_line = temp;
		parsed_line[part_count - 1] = part; 
		part = strtok(NULL, " ");
		part_count++;
   	}

	temp = realloc(parsed_line, sizeof(char*) * part_count);
	if (parsed_line == NULL)
	{
		printf("Memory allocation failure\n");
		free(parsed_line);
		free(line);
		exit(-1);
	}
	parsed_line = temp;
	parsed_line[part_count - 1] = '\0';
	return parsed_line;
}

int main(int argc, char* argv[])
{	
	int bytes_read;
	size_t size = 20;
	char* line = NULL;
	char** parsed_line = NULL;
	while (1)
	{
		printf("> ");
		bytes_read = getline(&line, &size, stdin);
		if (line[0] == '\n')
		{
			continue;
		}
		line[bytes_read - 1] = '\0';
		parsed_line = parse_line(line);
		if (strcmp(parsed_line[0], "q") == 0)
		{
			break;
		}

		if (!find_program(parsed_line))
		{
			free(parsed_line);
			continue;
		}

		pid_t pid = fork();
		if (pid == -1)
		{
			printf("fork() failed\n");
			free(parsed_line);
			return 2;
		}
		else if (pid == 0)
		{
			execvp(parsed_line[0], parsed_line);
			printf("execvp() failed\n");
			free(parsed_line);
			free(line);
			return 2;
		}
		else
		{
			int status;
			waitpid(pid, &status, 0);
			free(parsed_line);
		}

	}
	free(parsed_line);
	free(line);
	return 0;
}
