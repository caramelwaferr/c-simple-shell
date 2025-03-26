#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // getcwd function
#include "header.h"
#include "methods.c"




int main(int argc, char *argv[])
{
    
    char *userInput; 
    userInput = malloc(sizeof(char) * 512); // Allocate some memory for the user's command line input
    char cwd[256];                          // Current working directory
    char *home = getenv("HOME"); // Home directory, Can reduce in future (give straight as argument)
    char *path[3] = {NULL, getenv("PATH")}; // Gets the system path
    chdir(home);                 // Set current directory to home, so home is the default directory for the shell
    getcwd(cwd, sizeof(cwd));
    loadHistory("./history.txt");
    loadAlias("./alias.txt");
    printf("$%s> ", cwd);  // Display prompt, displays current directory
    
    while(fgets(userInput, 512, stdin)) // Main program loop
    {
        clearBuffer(userInput); // Clear buffer of excess input and remove newline character at the 1234
        
        char** tokens = parse(userInput);
        
        if(tokens[0] != NULL && strcmp(tokens[0], "exit") == 0) { // first condition checks that we haven't got an empty input
            if(tokens[1] != NULL){ // If exit is in the first token, but we have anoter invalid command
                printf("Too many parameters. Please just type 'exit'\n");
            }
        printf("Exiting shell...\n");
        break;
        }
    
        
        if(tokens[0] != NULL){
            runCommands(tokens, true, 0);
        }
        
        getcwd(cwd, sizeof(cwd));
        printf("$%s> ", cwd);
    }

    
   if (feof(stdin)) { // Handle Ctrl + D exit
    printf("\nExiting shell...\n");
}
    free(userInput); // Free allocated memory
    setPath(path); /* MAKE SURE TO TEST SETPATH - can come back and apply the strict "ONE PURPOSE" rule later*/
    saveHistory("./history.txt");
    saveAlias("./alias.txt");
    printf("%s", cwd);
}
