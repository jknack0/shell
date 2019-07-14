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
#define COMMAND_PIPE "|"
#define COMMAND_BACKGROUND_PROCESS "&"

bool containsCharacter(char *myargv[], int myargc, char stringToCheck[]);
int positionOfCharacter(char *myargv[], int myargc, char *stringToCheck);



int main(int argc, char** argv) {
    bool isWaiting = true;
    char cwdBuffer[BUFFERSIZE];
    char inputCommand[BUFFERSIZE];
    char *token;
    char *myargv[BUFFERSIZE];
    int myargc; 
    int status = 0;
    pid_t c_pid = 0, pid = 0;


   
    // Program loop
    while(true){ 
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
            if(!strcmp(token,COMMAND_BACKGROUND_PROCESS)) {
                isWaiting = false;
                break;
            }
            myargv[myargc] = token;
            token = strtok(NULL, " ");
            myargc++;
        } 


        // NULL terminate
        myargv[myargc] = NULL;

        // Get current working directory
        if(!strcmp(*myargv, COMMAND_CWD)) {
            printf("%s\n",cwdBuffer);
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
            int fd_outputFile = open(outputFilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            // Run ls in child process
            if(fork() == 0) {
                dup2(fd_outputFile, STDOUT_FILENO);
                close(fd_outputFile);
                execvp(leftargv[0], leftargv);
                _exit(1);
            } else {
                if(isWaiting) {
                    if((pid = wait(&status)) < 0 && isWaiting) {
                        perror("Wait");
                    }
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
            int fd_outputFile = open(outputFilename, O_WRONLY | O_CREAT | O_APPEND, 0666);
            // Run ls in child process
            if(fork() == 0) {
                dup2(fd_outputFile, STDOUT_FILENO);
                close(fd_outputFile);
                execvp(leftargv[0], leftargv);
                _exit(1);
            } else {
               if(isWaiting) {
                    if((pid = wait(&status)) < 0 && isWaiting) {
                        perror("Wait");
                    }
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
               if(isWaiting) {
                    if((pid = wait(&status)) < 0 && isWaiting) {
                        perror("Wait");
                    }
                }
            } 
            continue;
        } 

        // Pipe
        if(containsCharacter(myargv, myargc, COMMAND_PIPE)) {
            int positionOfCommand = positionOfCharacter(myargv, myargc, COMMAND_PIPE);
            int leftargc = positionOfCommand;
            char *leftargv[leftargc];
            int rightargc = myargc - positionOfCommand;
            char *rightargv[rightargc];
            int p[2];
            
            // Loop through copying the program instructions to a new pointer array
            for(int i = 0; i < leftargc; i++) {
                leftargv[i] = myargv[i];
            }

            // Null terminate
            leftargv[leftargc] = NULL;
            
            int j = 0;
            for(int i = leftargc + 1; i < myargc; i++) {
                rightargv[j] = myargv[i];
                j++;
            }
            
            // Null terminate
            rightargv[j] = NULL;

            // Pipe
            pipe(p);

            // Fork to run left side in child
            if(fork() == 0) {
                dup2(p[1], 1);
                close(p[0]);
                close(p[1]); 
                execvp(leftargv[0],leftargv);
                perror("execvp");
                _exit(1);
            } else {
                // Fork to run the right side
                if(fork() == 0) {
                    dup2(p[0], 0);
                    close(p[0]);
                    close(p[1]);
                    execvp(rightargv[0],rightargv);
                    perror("execvp");
                    _exit(1);
                } else {
                    close(p[0]);
                    close(p[1]);
                    if(isWaiting) {
                        // Wait for child process
                        if((pid = wait(&status)) < 0 && isWaiting) {
                            perror("Wait");
                        }
                }
                }
                close(p[0]);
                close(p[1]);
                // Wait for child process
                if(isWaiting) {
                    if((pid = wait(&status)) < 0 && isWaiting) {
                        perror("Wait");
                    }
                }
            }
            continue;
        }       
        
        // ls/cat/wc
        if (fork() == 0) {
            execvp(myargv[0], myargv);
            perror("execvp");
            _exit(1);
        } else {
            if(isWaiting) {
                    if((pid = wait(&status)) < 0 && isWaiting) {
                        perror("Wait");
                    }
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

