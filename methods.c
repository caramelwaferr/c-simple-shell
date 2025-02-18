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
int historyCounter = 0;

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

// void addToHistory(char** tokens) {

//     if(historyCounter < MAX_HISTORY_SIZE) {
//         history[historyCounter] = tokens;
//         historyCounter++;
//     }
//     else{
//         for(int i = 0; i < MAX_HISTORY_SIZE; i++){
//             history[i] = history[i + 1];
//         }
//         history[MAX_HISTORY_SIZE - 1] = tokens; // tokens history[20] = tokens 
//     }
// }

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
    if(tokens[0][0] == '!') {
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
        if(num >= 1 && num <=20) {
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
        if(num >= 1 && num <=20) {
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
