/*
* CECS 326-01
* Operating Systems
* Matthew Zaldana (ID 027008928)
* Assignment 1
* Due Date: 9/16/2021
* Submission Date: 9/16/2021
* Program Description
* 1. The program is supposed to print the amount of childs, their genders, and their names to the screen having been passed as arguments to child.cpp fro parent.cpp
* 2. parent.cpp takes in a gender-name pair and passes them as arguments to child, waits for child to finish executing, then continues; child.cpp prints the gender-name pair to the screen using a number as a delimiter
*/

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h> 
#include <stdlib.h>

using namespace std;

int main(int argCount,char* argv[]) {

    // Holds the number of children.
    int amountOfChildren = 0;

    // Defines a pair.
    const int pair = 2;

    // Checks if the given arguments are an even number.
    if ((argCount - 1) % pair == 0 ) {
        // Sets amountOfChilderen to number of given arguments.
        amountOfChildren = argCount / pair;
        printf("I have %d children.", amountOfChildren);

        // Flush internal buffer. This will prevent the above printf from printing afterwards.
        fflush(stdout);

        pid_t child_pid, wpid;
        char childNumber[3];

        for (int i = 0; i < amountOfChildren; i++) {
            // Create a new child process.
            if ((child_pid = fork()) == 0) {
                // Convert an int to a char. This is needed in order to pass the number to the arguments below.
                sprintf(childNumber, "%d", i);

                // Arguments that will be passed to the child program. 
                char *args[] = {childNumber, argv[i*2+1],argv[i*2+2], NULL};

                // Execute the child program and give it the above arguments.
                execv("./child", args);

                // Exit the child process.
                exit(0);
            }
        }
        
        int status = 0;

        // Wait for all child processes.
        while ((wpid = wait(&status)) > 0);

        printf("\nAll child processes terminated. Parent exits.\n");
    } else {
        // Odd pair error handling
        printf("Please provide pair(s) in arguments!\n");
    }

    return 0;
}