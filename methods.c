#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <errno.h>
#include <stdbool.h>

char* tokens[512];
char** history[MAX_HISTORY_SIZE];
char** alias[MAX_ALIAS];

int historyCounter = 0;
int aliasCounter = 0;

char** parse(char input [512]){
        int i = 1;
        char* token = strtok(input, " \n\t|<>;&");
        tokens[0] = token;  // get the first token
        
        
        while(token != NULL)
        {  // iterate through extracting the token
            token = strtok(NULL, " \n\t|<>;&"); // signal to continue tokenising from the last position
            tokens[i] = token;
            i++;
        }
        tokens[i] = NULL;

        
    return tokens;
}


void clearBuffer(char* userInput){
      
    int len = strlen(userInput);

    if (len > 0 && userInput[len-1] == '\n') {
            userInput[len-1] = '\0';
    } 
        
        // Clear buffer only if fgets didn't read whole line
    else {
          int ch;
          while((ch = getchar()) != '\n' && ch != EOF);
        }
}



void runCommands(char** tokens, bool newCommand){
    // Run custom commands that aren't executables, eg getenv
    // If doesn't work either then print error
    if (isHistoryCommand(tokens)) { 
        runHistoryCommand(tokens);
        newCommand = false;
    }
    else if(strcmp(tokens[0], "cd") == 0){ // change directory
        changeDirectory(tokens);
    }
    else if (strcmp(tokens[0], "getPath") == 0){
        getPath(tokens);
    }

    else if (strcmp(tokens[0], "setPath") == 0){
        setPath(tokens);
    }
    else if(strcmp(tokens[0], "history") == 0){
        printHistory(tokens);
    }
    else if(strcmp(tokens[0], "alias") == 0){
        addAlias(tokens);
    }
    else{
       executeSystemCommand(tokens); 
    }

    if(newCommand) {
        addToHistory(tokens); // everything gets added except for history commands and commands recalled from history
    }
}

int executeSystemCommand(char** tokens){

    // Create a new process
    pid_t pid = fork(); // process id  // parent and child process

    if(pid == -1){
        perror("Process creation failed\n");
        return -1;
    }
    else if(pid == 0){
        // CHILD process
        if (execvp(tokens[0], tokens) == -1) {  // execvp returns -1 on failure
            perror("Error executing command"); // take first token and replace it with the command and take rest of tokens as arguments
            exit(1);  // exit function with status 1 to terminate processes
        }
    }
    else{
        int status;
        waitpid(pid, &status, 0); // load the status returned from the child process into status
        if(WIFEXITED(status)){ // was the child process terminated normally by exit
            return WEXITSTATUS(status); // What was its exit status
        }

    }
    return 0;
}

void getPath(char** tokens){
    if(tokens[1] != NULL){
        fprintf(stderr, "Error: Too many parameters\n");
    }
    else {
        printf("PATH: %s \n", getenv("PATH")); 
    }
}

void setPath(char** tokens){
    if(0) { //
        return; //
    } // COMMENT THIS OUT WHEN WE WANT TO TEST SETPATH - CURRENTLY DISABLED TO AVOID CHANGING PC PATH


    if(tokens[1] == NULL) {
        fprintf(stderr, "Error: Too few parameters - must include a path to set\n");
        return;
    } // sizeof(tokens) > (sizeof(tokens[0])*2
    else if(tokens[2] != NULL) {
        fprintf(stderr, "Error: Too many parameters\n");
        return;
    }
    else {
        setenv("PATH", tokens[1], 1);
    }
}

void changeDirectory(char** tokens){

    if(tokens[1] == NULL) {         // No directory specified, therefore change directory to home
        chdir(getenv("HOME"));              
    }
    else if (tokens[2] != NULL) {
        fprintf(stderr, "Error: Too many parameters\n");
        return;
    } 
    else {    // Once directory is proven to be valid
        if (chdir(tokens[1]) == -1) {
            if(errno == 2) {
                fprintf(stderr, "'%s', no such file or directory\nEnter a valid directory\n", tokens[1]);
            }
            else if(errno == 20){
                fprintf(stderr, "'%s' is not a directory\nenter a valid one\n", tokens[1]);
            }
        }
    }
}

bool isHistoryCommand(char** tokens) {
    if(tokens[0][0] == '!') {
        return true;
    }
    else{
        
    }
    return false;
}

void addToHistory(char** tokens) {
    if (historyCounter < MAX_HISTORY_SIZE) {
        // Allocate memory for a new history entry
        history[historyCounter] = malloc(sizeof(char*) * 512);  // Allocate array of string pointers [[malloc],[malloc],[]]
        
        int i;

        for (i = 0; tokens[i] != NULL; i++) {
            history[historyCounter][i] = strdup(tokens[i]);  // Copy each token
        }
        history[historyCounter][i] = NULL;  // Null-terminate the command
        historyCounter++;
    }
    else {
        // Free the oldest entry before shifting
        for (int i = 0; history[0][i] != NULL; i++) {
            free(history[0][i]);  // Free each token
        }
        free(history[0]); // Free the array of token pointers
        
        for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
            history[i] = history[i + 1];
        }

        // Allocate memory for the new command at the last position
        history[MAX_HISTORY_SIZE - 1] = malloc(sizeof(char*) * 512);
        int i;
        for (i = 0; tokens[i] != NULL; i++) {
            history[MAX_HISTORY_SIZE - 1][i] = strdup(tokens[i]);  // Copy each token
        }
        history[MAX_HISTORY_SIZE - 1][i] = NULL;  // Null-terminate
    }
}

void runHistoryCommand(char** tokens) {
    memmove(tokens[0], tokens[0] + sizeof(char), strlen(tokens[0]));  
    char* command = malloc(512);
    command[0] = '\0';
    if(tokens[0][0] == '!' && historyCounter > 0) {
        
        for(int i = 0; history[historyCounter-1][i] != NULL; i++){
            strcat(command, history[historyCounter-1][i]);
            strcat(command, " ");
        } 
        printf("%s\n", command);
        
        runCommands(history[historyCounter-1], false); // !15
    }
    else if(tokens[0][0] == '-') {
        memmove(tokens[0], tokens[0] + sizeof(char), strlen(tokens[0]));
        int num = atoi(tokens[0]);
        // printf("%d", num);
        if(num >= 1 && num <= historyCounter) {
            for(int i = 0; history[historyCounter-num][i] != NULL; i++){
                strcat(command, history[historyCounter-num][i]);
                strcat(command, " ");
            }
            if (history[historyCounter-num] != NULL) {
                runCommands(history[historyCounter-num], false);
            }
            else {
                fprintf(stderr, "You have %d history commands in history. Please enter a valid history number.", historyCounter);
            }
            
        }
        else {
            fprintf(stderr, "You have %d history commands in history. Please enter a valid history number.", historyCounter);
        }
    }
    else{ 
        int num = atoi(tokens[0]);
        if (num >= 1 && num <= historyCounter) {
            for(int i = 0; history[num-1][i] != NULL; i++){
                strcat(command, history[num-1][i]);
                strcat(command, " ");
            } 
            if (history[num-1] != NULL) {
                runCommands(history[num-1], false);
            }
            else {
                fprintf(stderr, "You have %d history commands in history. Please enter a valid history number.", historyCounter);
            }
        }
        else {
            fprintf(stderr, "You have %d history commands in history. Please enter a valid history number.", historyCounter);
        }
    }
    free(command);
}

void printHistory(char** tokens){
    char* command = malloc(512);
    for(int i = 0; i < historyCounter; i++){
    command[0] = '\0';
        for(int j = 0; history[i][j] != NULL; j++){
            strcat(command, history[i][j]);
            strcat(command, " ");
        }
        printf("%d: %s\n", i + 1, command);
        
    }
    free(command);
}

void save_history(const char* filename) {

    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error - could not open history file");
        return;
    }
    
    // Write the number of history entries at the top of the file
    fprintf(file, "%d\n", historyCounter);
    
    // Write each command as a line in the file
    for (int i = 0; i < historyCounter; i++) {
        // Convert tokens back to a command line
        char command[512] = "";
        for (int j = 0; history[i][j] != NULL; j++) {
            if (j > 0) {
                strcat(command, " ");
            }
            strcat(command, history[i][j]);
        }
        // Write the command to the file
        fprintf(file, "%s\n", command);
    }
    
    fclose(file);
    printf("History saved to %s\n", filename);
}


void load_history(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error - could not open history file");
        return;
    }
    
    // Free existing history if any
    for (int i = 0; i < historyCounter; i++) {
        for (int j = 0; (history)[i][j] != NULL; j++) {
            free((history)[i][j]);
        }
        free((history)[i]);
    }

    // Read the number of history entries
    int numEntries;
    if (fscanf(file, "%d\n", &numEntries) != 1) {
        perror("Error reading history count");
        fclose(file);
        return;
    }
    
    historyCounter = 0;
    char line[512];
    
    // Read each line and add it to history
    while (historyCounter < numEntries && historyCounter < MAX_HISTORY_SIZE && fgets(line, sizeof(line), file)) {
        // Remove newline if present
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        // Parse the line into tokens
        char lineCopy[512];
        strcpy(lineCopy, line);
        char** parsed = parse(lineCopy);
        
        // Create a new history entry
        (history)[historyCounter] = malloc(sizeof(char*) * 512);
        
        // Copy each token
        int i;
        for (i = 0; parsed[i] != NULL; i++) {
            (history)[historyCounter][i] = strdup(parsed[i]);
        }
        (history)[historyCounter][i] = NULL;  // Null-terminate
        
        (historyCounter)++;
    }
    
    fclose(file);
    printf("Loaded %d commands from history file %s\n", historyCounter, filename);
}

void addAlias(char** tokens){
    alias[aliasCounter] = malloc(sizeof(char*) * 512);  // Allocate array of string pointers [[malloc],[malloc],[]]

    int existsAlias = 0;
    
    if (aliasCounter = 10){
        fprintf(stderr, "No more aliases can be set.");
    }

    int i, j;

    for(i = 0; i < aliasCounter || existsAlias = 1; i++){
        if (tokens[1] == alias[i][0]){      // Check if alias already exists
            int existsAlias = 1;
            for (j = 2; tokens[j] != NULL; j++) {           // Everything after the alias will be considered the aliased command
                alias[i][1] = strcat(tokens[j]);  // Copy each token, overwrite previous alias with new command
            }
            alias[i][j] = NULL;  // Null-terminate the command
        }
    }

    if(existsAlias = 1){
        printf("Previous alias %s has been overwritten by new command.", tokens[1]);
        return;
    }

    // Code should only get here if alias does not already exist

    int i;
    for (i = 2; tokens[i] != NULL; i++) {           // Everything after the alias will be considered the aliased command
        alias[aliasCounter][1] = strcat(tokens[i]);     // Copy each token after the alias name as the aliased command
    }

    alias[aliasCounter][i] = NULL;  // Null-terminate the command
    aliasCounter++;

}
