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


    

    // Program loop
    while(inputCommand[0] != 'z') {
        
        // Print the prompt
        printf(PROMPT);

        
        // Get command from user
        fgets(inputCommand, BUFFERSIZE, stdin);
        
        // Split the user command into tokens and have argv point to them
        token = strtok(inputCommand, " ");
        myargc = 0;
        while (token) {
            myargv[myargc] = token;
            printf("%s\n", myargv[myargc]);
            token = strtok(NULL, " ");
            myargc++;
        }
        myargv[myargc] = NULL;

        execlp(myargv[0],myargv[0], NULL);;
        //perror("execvp");
        
        

        
        


        
        
        
       



    
        
    }
    return 0;
}
