/****************************************************************
 * Name        :  Jonathon Knack                                *
 * Class       :  CSC 415                                       *
 * Date        :  7/8/2019                                      *
 * Description :  Writting a simple bash shell program          *
 *                that will execute simple commands. The main   *
 *                goal of the assignment is working with        *
 *                fork, pipes and exec system calls.            *
 ****************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

/* CANNOT BE CHANGED */
#define BUFFERSIZE 256
/* --------------------*/
#define PROMPT "myShell >> "
#define PROMPTSIZE sizeof(PROMPT)
#define COMMAND_EXIT "exit"
#define COMMAND_CWD "cwd"
#define COMMAND_CD "cd"
#define COMMAND_REDIRECT_OUTPUT ">"
#define COMMAND_REDIRECT_OUTPUT_APPEND ">>"
#define COMMAND_REDIRECT_INPUT "<"

bool containsCharacter(char *myargv[], int myargc, char stringToCheck[]);
int positionOfCharacter(char *myargv[], int myargc, char *stringToCheck);



int main(int argc, char** argv) {
    bool isBackground = false;
    char cwdBuffer[BUFFERSIZE];
    char inputCommand[BUFFERSIZE];
    char *token;
    char *myargv[BUFFERSIZE];
    int myargc; 
    int status;
    pid_t c_pid, pid;

    // Program loop
    while(true) {
        
        // Print the prompt
        printf(PROMPT);

        // Get command from user
        fgets(inputCommand, BUFFERSIZE, stdin);

        // Null terminate string
        inputCommand[strlen(inputCommand) - 1] = '\0';
        
        // Split the user command into tokens and have argv point to them
        token = strtok(inputCommand, " ");
        
        // Exit condition
        if(!strcmp(token, COMMAND_EXIT)) {
            return 0;
        }
        myargc = 0;
        while (token) {
            myargv[myargc] = token;
            token = strtok(NULL, " ");
            myargc++;
        }

        // NULL terminate
        myargv[myargc] = NULL;

        // Get current working directory
        if(!strcmp(*myargv, COMMAND_CWD)) {
            printf("%s\n",getcwd(cwdBuffer, BUFFERSIZE));
            continue;
        }

        // Change directory
        if(!strcmp(*myargv, COMMAND_CD)) {
            chdir(myargv[1]);
            continue;
        }

        // Redirect Output
        if(containsCharacter(myargv, myargc, COMMAND_REDIRECT_OUTPUT)) {
            int positionOfCommand = positionOfCharacter(myargv, myargc, COMMAND_REDIRECT_OUTPUT);
            int leftargc = positionOfCommand;
            char *leftargv[leftargc];
            char *outputFilename = myargv[myargc - 1];
            
            // Loop through copying the program instructions to a new pointer array
            for(int i = 0; i < leftargc; i++) {
                leftargv[i] = myargv[i];
            }

            // Null terminate
            leftargv[leftargc] = NULL;

            // Open file/create and trunc
            int fd_outputFile = open(outputFilename, O_WRONLY | O_CREAT | O_TRUNC);
            // Run ls in child process
            if(fork() == 0) {
                dup2(fd_outputFile, STDOUT_FILENO);
                close(fd_outputFile);
                execvp(leftargv[0], leftargv);
                _exit(1);
            } else {
               if((pid = wait(&status)) < 0) {
                    perror("Wait");
                }
            } 
            continue;
        }

        // Redirect Output Append
        if(containsCharacter(myargv, myargc, COMMAND_REDIRECT_OUTPUT_APPEND)) {
            int positionOfCommand = positionOfCharacter(myargv, myargc, COMMAND_REDIRECT_OUTPUT_APPEND);
            int leftargc = positionOfCommand;
            char *leftargv[leftargc];
            char *outputFilename = myargv[myargc - 1];
            
            // Loop through copying the program instructions to a new pointer array
            for(int i = 0; i < leftargc; i++) {
                leftargv[i] = myargv[i];
            }

            // Null terminate
            leftargv[leftargc] = NULL;

            // Open file/create and append
            int fd_outputFile = open(outputFilename, O_WRONLY | O_CREAT | O_APPEND);
            // Run ls in child process
            if(fork() == 0) {
                dup2(fd_outputFile, STDOUT_FILENO);
                close(fd_outputFile);
                execvp(leftargv[0], leftargv);
                _exit(1);
            } else {
               if((pid = wait(&status)) < 0) {
                    perror("Wait");
                }
            } 
            continue;
        }
        
        // Redirect Input
        if(containsCharacter(myargv, myargc, COMMAND_REDIRECT_INPUT)) {
            int positionOfCommand = positionOfCharacter(myargv, myargc, COMMAND_REDIRECT_INPUT);
            int leftargc = positionOfCommand;
            char *leftargv[leftargc];
            char *inputFilename = myargv[myargc - 1];
            
            // Loop through copying the program instructions to a new pointer array
            for(int i = 0; i < leftargc; i++) {
                leftargv[i] = myargv[i];
            }

            // Null terminate
            leftargv[leftargc] = NULL;

            // Open file/create and append
            int fd_inputFile = open(inputFilename, O_RDONLY);
            if(fd_inputFile < 0) {
                perror("inputFile: ");
            }
            // Run ls in child process
            if(fork() == 0) {
                dup2(fd_inputFile, STDIN_FILENO);
                close(fd_inputFile);
                execvp(leftargv[0], leftargv);
                _exit(1);
            } else {
               if((pid = wait(&status)) < 0) {
                    perror("Wait");
                }
            } 
            continue;
        }
        
        // ls/cat/wc
        c_pid = fork();
        if (c_pid == 0) {
            execvp(myargv[0], myargv);
            perror("execvp");
            _exit(1);
        } else if (c_pid > 0) {
            if((pid = wait(&status)) < 0) {
                perror("Wait");
            }
        }
    }
    return 0;
}

bool containsCharacter(char *myargv[], int myargc, char *stringToCheck) {
    for(int i = 0; i < myargc; i++) {
        if(!strcmp(myargv[i], stringToCheck)) {
            return true;
        }
    }
    return false;
}

int positionOfCharacter(char *myargv[], int myargc, char *stringToCheck) {
    for(int i = 0; i < myargc; i++) {
        if(!strcmp(myargv[i], stringToCheck)) {
            return i;
        }
    }
    return -1;

}

