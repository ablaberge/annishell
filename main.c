/*
 * main.c
 *
 *  Created on: Mar 17 2017
 *      Author: david
 * 	Implemented on: Mar 3 2025
 * 		Author: Annika Laberge
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <string.h>
#include "dsh.h"

#define MAX_PROC 1024

int main(int argc, char *argv[])
{

	// DO NOT REMOVE THE BLOCK BELOW (FORK BOMB PREVENTION) //
	struct rlimit limit;
	limit.rlim_cur = MAX_PROC;
	limit.rlim_max = MAX_PROC;
	setrlimit(RLIMIT_NPROC, &limit);
	// DO NOT REMOVE THE BLOCK ABOVE THIS LINE //

	char *cmdline = (char *)malloc(MAXBUF); // stores user input from commmand line
	while (1)
	{
		printf("dsh> ");
		// reads up to 256 characters into the buffer
		if (fgets(cmdline, 256, stdin) == NULL)
		{
			printf("\n");
			free(cmdline);
			exit(0); // exit the program if EOF is input
		}
		else
		{
			// Remove the newline character
			if (strlen(cmdline) > 0 && cmdline[strlen(cmdline) - 1] == '\n')
				cmdline[strlen(cmdline) - 1] = '\0';

			char **array = trimInput(cmdline);
			if (array[0] == NULL || strcmp(array[0], "") == 0)
			{
				printf("Please enter a command.\n"); // Reprompt if empty
			}
			else if (array[0][0] == '/')
			{

				// Handle file input
				//  Does file exist? N --> file not found error
				//  otherwise fork() and exec()
				//  End with ampersand
				//  If yes, run in background
				printf("Success!\n");
			}
			else
			{
				int builtIn = isBuiltIn(array[0]);
				if (builtIn == 1)
				{
					//exit
					printf("\n");
					free(cmdline);
					exit(0);
				}
				else if (builtIn == 2)
				{
					// call pwd
					char *cwd = (char *)malloc(256 * sizeof(char));
					printf("%s\n",getcwd(cwd, 256));
					free (cwd);
					continue;
				}
				else if (builtIn == 3)
				{
					cd(array[1]);
					continue;
				}
				else
				{
					// Is the command in an executable dir?
					// If yes: fork() and execv() then check for ampersand
					// If no: split path by "." and check doc for next steps
				}
			}

			// Free the allocated memory
			for (int i = 0; array[i] != NULL; i++)
			{
				free(array[i]);
			}
			free(array);
		}
	}
	return 0;
}
