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

/* CANNOT BE CHANGED */
#define BUFFERSIZE 256
/* --------------------*/
#define PROMPT "myShell >> "
#define PROMPTSIZE sizeof(PROMPT)



int main(int argc, char** argv) {
    char inputCommand[BUFFERSIZE];
    char *token;
    char *myargv[BUFFERSIZE];
    int myargc; 
    int status;
    pid_t c_pid, pid;

    // Program loop
    while(inputCommand[0] != 'z') {
        
        // Print the prompt
        printf(PROMPT);

        // Get command from user
        fgets(inputCommand, BUFFERSIZE, stdin);

        // Null terminate string
        inputCommand[strlen(inputCommand) - 1] = '\0';
        
        // Split the user command into tokens and have argv point to them
        token = strtok(inputCommand, " ");
        myargc = 0;
        while (token) {
            myargv[myargc] = token;
            token = strtok(NULL, " ");
            myargc++;
        }

        // null terminate
        myargv[myargc] = NULL;
        
        
        // 
        c_pid = fork();
        
        if (c_pid == 0) {
            execvp(myargv[0], myargv);
            perror("execvp");
        } else if (c_pid > 0) {
            if((pid = wait(&status)) < 0) {
                perror("Wait");
            }
        } else {
            
        }
    }
    return 0;
}


