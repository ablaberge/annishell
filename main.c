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
    if (cmdline == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

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
            if (array == NULL) {
                printf("Memory allocation failed\n");
                continue;
            }

            if (array[0] == NULL || strcmp(array[0], "") == 0)
            {
                printf("Please enter a command.\n"); // Reprompt if empty
                freeArray(array);
                continue;
            }
            else if (array[0][0] == '/')
            {
                // Handle file input
                //  Does file exist? N --> file not found error
                //  otherwise fork() and exec()
                if (access(array[0], F_OK | X_OK) == 0)
                {
                    runExecutable(array);
                    freeArray(array); // Free array after use
                    continue;
                }
                else
                {
                    // No good! File doesn't exist or is not executable!
                    // Alert the user and re-prompt
                    printf("%s does not exist or is not executable!\n", array[0]);
                    freeArray(array); 
                    continue;
                }
            }
            else
            {
                int builtIn = isBuiltIn(array[0]);
                if (builtIn == 1)
                {
                    printf("\n");
                    freeArray(array);
                    free(cmdline);
                    exit(0);
                }
                else if (builtIn == 2)
                {
                    // call pwd
                    char cwd[MAXBUF]; 
                    if (getcwd(cwd, MAXBUF) != NULL) {
                        printf("%s\n", cwd);
                    } else {
                        perror("getcwd");
                    }
                    freeArray(array); 
                    continue;
                }
                else if (builtIn == 3)
                {
                    cd(array[1]);
                    freeArray(array); 
                    continue;
                }
                else
                {
                    // Fix path resolution approach
                    char full_path[MAXBUF];
                    int found = 0;
                    
                    // First try to find in HOME directory
                    const char *home_dir = getenv("HOME");
                    if (home_dir != NULL) {
                        snprintf(full_path, MAXBUF, "%s/%s", home_dir, array[0]);
                        if (access(full_path, F_OK | X_OK) == 0) {
                            // We found the executable in HOME
                            free(array[0]);
                            array[0] = strdup(full_path);
                            runExecutable(array);
                            found = 1;
                        }
                    }
                    
                    // If not found in HOME, search in PATH
                    if (!found) {
                        char *path_env = getenv("PATH");
                        if (path_env != NULL) {
                            char **paths = split(path_env, ":");
                            if (paths != NULL) {
                                for (int i = 0; paths[i] != NULL; i++) {
                                    snprintf(full_path, MAXBUF, "%s/%s", paths[i], array[0]);
                                    if (access(full_path, F_OK | X_OK) == 0) {
                                        // We found the executable in PATH
                                        free(array[0]);
                                        array[0] = strdup(full_path);
                                        runExecutable(array);
                                        found = 1;
                                        break;
                                    }
                                }
                                freeArray(paths); 
                            }
                        }
                    }
                    
                    if (!found) {
                        printf("%s: command not found\n", array[0]);
                    }
                    
                    freeArray(array);
                    continue;
                }
            }
        }
    }
    return 0;
}