/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: David Chiu
 *  Implemented on: Mar 3, 2025
 *      Author: Annika Laberge
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

/**
 * Works similarly to Java String's split(...). Takes a string and splits
 * it by a given delimiter.
 *
 * @param *str      Pointer to string to be split
 * @param *delim    Pointer to the string delimiter
 * @return          The new array of substrings created
 */
char **split(char *str, char *delim)
{
    // First, figure out the number of tokens that you will end up with
    int numTokens = 0;
    char *temp = str;
    while ((temp = strstr(temp, delim)) != NULL)
    {
        numTokens++;
        temp += strlen(delim); // Move on to the next occurence of delim
    }
    numTokens++; // Account for the last token

    // Create a copy of the input string since strtok modifies it
    char *strCopy = strdup(str);
    if (strCopy == NULL) {
        return NULL;
    }

    // This creates pointers to strings with space for NULL terminator
    char **array = (char **)malloc((numTokens + 1) * sizeof(char *));
    if (array == NULL) {
        free(strCopy);
        return NULL;
    }

    // Tokenize substrings and add them to array
    char *token;
    token = strtok(strCopy, delim);
    int i = 0;
    while (token != NULL && i < numTokens)
    {
        array[i] = strdup(token);
        if (array[i] == NULL) {
            // Clean up on error
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            free(strCopy);
            return NULL;
        }
        token = strtok(NULL, delim);
        i++;
    }

    array[i] = NULL; // Set final element of array to NULL
    free(strCopy);
    return array;
}

char **trimInput(char *str)
{
    char **array = split(str, " ");
    return array;
}

// Returns 1 for exit, 2 for pwd, 3 for cd, and 0 if command is not built in
int isBuiltIn(char *str)
{
    if (strcmp(str, "exit") == 0)
    {
        return 1;
    }
    else if (strcmp(str, "pwd") == 0)
    {
        return 2;
    }
    else if (strcmp(str, "cd") == 0)
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

void cd(char *dir)
{
    if (dir == NULL)
    { // no directory specified, use HOME
        const char *home_dir = getenv("HOME");
        if (home_dir != NULL) {
            chdir(home_dir);
        } else {
            printf("HOME environment variable not set\n");
        }
    }
    else if (chdir(dir) != 0)
    {
        printf("%s: no such file or directory\n", dir);
    }
}

void runExecutable(char **array)
{
    pid_t pid;
    int background = 0;
    
    // Count the number of elements in the array
    int size = 0;
    while (array[size] != NULL) {
        size++;
    }
    
    if (size > 0 && strcmp(array[size - 1], "&") == 0)
    { // Must run in background
        background = 1;
        array[size - 1] = NULL; 
        size--;
    }
    
    char **args = (char **)malloc(size * sizeof(char *));
    if (!args) {
        printf("Memory allocation failed\n");
        return;
    }
    
    // Copy command arguments to args array
    for (int i = 1; i < size; i++)
    {
        args[i - 1] = array[i];
    }
    args[size - 1] = NULL; 

    if (background == 0) // Run in foreground
    {
        pid = fork();
        if (pid == 0) // Child
        {
            execv(array[0], args);
            // If execv returns, there was an error
            perror("execv");
            free(args);
            exit(1);
        }
        else if (pid < 0) // Fork failed
        {
            perror("fork");
        }
        else // Parent
        {
            wait(NULL);
        }
    }
    else // Run in background
    {
        pid = fork();
        if (pid == 0) // Child
        {
            execv(array[0], args);
            // If execv returns, there was an error
            perror("execv");
            free(args);
            exit(1);
        }
        else if (pid < 0) 
        {
            perror("fork");
        }
        // Parent continues without waiting
    }
    
   
    free(args);
}

// Helper function to free split array
void freeArray(char **array)
{
    if (array == NULL) {
        return;
    }
    
    for (int i = 0; array[i] != NULL; i++) {
        free(array[i]);
    }
    free(array);
}