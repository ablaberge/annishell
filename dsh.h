#define MAXBUF 256  // max number of characteres allowed on command line

// TODO: Any global variables go below


// TODO: Function declarations go below
char** split(char *str, char *delim);
char** trimInput (char *str);
int isBuiltIn(char *str);
void cd(char *dir);
void runExecutable(char **array);
void freeArray(char **array);
