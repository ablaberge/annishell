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

// TODO: Your function definitions below (declarations in dsh.h)


/**
 * Works similarly to Java String's split(...). Takes a string and splits
 * it by a given delimiter.
 * 
 * @param *str      Pointer to string to be split
 * @param *delim    Pointer to the string delimiter
 * @return          The new array of substrings created
*/
char** split(char *str, char *delim)
{

    // First, figure out the number of tokens that you will end up with
    int numTokens = 0; 
    char *temp = str;
    while ((temp = strstr(temp, delim)) != NULL) {
        numTokens++;
        temp += strlen(delim); // Move on to the next occurence of delim
    }

    // This creates num pointers to strings
    char **array = (char **)malloc((numTokens+1) * sizeof(char *));


    // This loops through each array element and instantiates
    // an array of chars of length MAXBUF
    for (int i = 0; i < numTokens; i++)
    {
        array[i] = (char *)malloc(MAXBUF * sizeof(char));
    }

    // Tokenize substrings and add them to array
    char *token;
    token = strtok(str, delim); 
    int i = 0;
    while (token != NULL && i < numTokens+1) {
        array[i] = (char *)malloc((strlen(token) + 1) * sizeof(char));
        strcpy(array[i], token);
        token = strtok(NULL, delim);
        i++;
    }

    array[i] = NULL; // Set final element of array to NULL
    return array;
}
