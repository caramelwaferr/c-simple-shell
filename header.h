// REMINDER THAT WE NEED A FULL DESCRIPTION OF EACH METHOD ACCORDING TO THE EXERCISE
//
//
//
//
#include <stdbool.h>
#define MAX_HISTORY_SIZE 20

char** parse(char input [512]); // Tokenises the user input, so that the shell can read the input and decide which actions to take, char* tokens[]

void clearBuffer(char *);

void runCommands(char**, bool); // Runs custom commands that aren't executables.

int executeSystemCommand(char **);

void getPath(char**);   // Prints the current system path. 

void setPath(char**);   // Sets the current system path as specified by the user. 

void changeDirectory(char**);   // Changes the current working directory of the system.

bool isHistoryCommand(char**);

void addToHistory(char**);

void runHistoryCommand(char**);

void printHistory(char**);
