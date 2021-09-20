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

#include <stdio.h> 

int main(int argCount, char* argv[]) {

    // Checks if there are 3 arguments (child #, gender, and name)
    if (argCount == 3) {
        printf("\n Child # %s: I am a %s, and my name is %s.", argv[0], argv[1], argv[2]);
    }

    return 0;
}