// REMINDER THAT WE NEED A FULL DESCRIPTION OF EACH METHOD ACCORDING TO THE EXERCISE
//
//
//
//
#include <stdbool.h>
#define MAX_HISTORY_SIZE 20
#define MAX_ALIAS 10

char** parse(char input [512]); // Tokenises the user input, so that the shell can read the input and decide which actions to take, char* tokens[]

void clearBuffer(char *);

void runCommands(char**, bool, int); // Runs custom commands that aren't executables.

int executeSystemCommand(char **);

void getPath(char**);   // Prints the current system path. 

void setPath(char**);   // Sets the current system path as specified by the user. 

void changeDirectory(char**);   // Changes the current working directory of the system.

bool isHistoryCommand(char**);

void addToHistory(char**);

void runHistoryCommand(char**);

void printHistory(char**);

void saveHistory(const char* filename);

void loadHistory(const char* filename);

void addAlias(char**);

bool removeAlias(char**);

void printAliases(void);

bool isAlias(char**);

bool runAlias(char**, int);

void saveAlias(const char* filename);

void loadAlias(const char* filename);
