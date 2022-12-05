/* This is a simple shell program that can be used to run programs
that are found from PATH environment variable. The program uses fork()
to crete a duplicate of itself and that duplicate process will be replaced
with an execution of a program that we want to run using exec().
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


/* find_program() is used to find out if the program that a user
wants to execute exists in PATH */
int find_program(char** parsed_line)
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	char* path = getenv("PATH");
	/* We need to copy path to another array since strtok() modifies the
	array it handles and it would modify the PATH. */
	char temp[strlen(path) + 1];
	strcpy(temp, path);

   	char* part = strtok(temp, ":");
   	while (part != NULL)
	{
		/* Checking if the file exists in one of the directory that is in PATH */
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

/* parse_line() is used to parse the parts from user input to an array
of pointers that can be then used by execvp(). Dynamic memory allocation
is used since there is no way to know the length of the user input. */
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
	/* One more allocation because execvp() requires the array of pointers
	passed to it to be terminated by NULL pointer */
	temp = realloc(parsed_line, sizeof(char*) * part_count);
	if (parsed_line == NULL)
	{
		printf("Memory allocation failure\n");
		free(parsed_line);
		free(line);
		exit(-1);
	}
	parsed_line = temp;
	parsed_line[part_count - 1] = NULL;
	return parsed_line;
}

/* main() function continuously asks command input from a user
and it uses fork() to create a child process that will be replaced
by the program specified in the user input. */
int main(int argc, char* argv[])
{
	printf("******************************************************************\n");
	printf("* This is a simple shell that executes programs found from PATH. *\n");
	printf("* Running a program in the background and piping not supported.  *\n");
	printf("* Enter q to exit shell.                                         *\n");
	printf("******************************************************************\n");
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
		line[bytes_read - 1] = '\0'; /* Removing the newline from command */

		/*parse_line() uses dynamic memory allocation so parsed_line will be
		freed later before it can be assigned a new value*/
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

		/* Creating a child process from the current process using fork() */
		pid_t pid = fork();
		if (pid == -1)
		{
			printf("fork() failed\n");
			free(parsed_line);
			return 2;
		}
		else if (pid == 0)
		{
			/* fork() returns 0 when we are the child process. Here we use execvp()
			to replace the child process with the program that user wants to execute. */
			execvp(parsed_line[0], parsed_line);
			/* execvp() doesn't return, it only comes here if it fails */
			printf("execvp() failed\n");
			free(parsed_line);
			free(line);
			return 2;
		}
		else
		{
			int status;
			/* fork() returns the child process pid when we are the parent process.
			Here we use waitpid() to wait until child process is finished. */
			waitpid(pid, &status, 0);
			free(parsed_line);
		}

	}
	free(parsed_line);
	free(line);
	return 0;
}
