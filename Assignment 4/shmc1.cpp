/* shmc1.cpp */

// loads all necessary header files
#include "registration.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>

using namespace std;

/* creates variables related to memory and system calls
*/
CLASS *class_ptr; 										// CLASS pointer
void *memptr; 											// memory pointer
char *pname;											// process name pointer
int	shmid, ret;											// holds shared memory ID and return value
void rpterror(char *), srand(), perror(), sleep();		// system report error function 
void sell_seats();										// function to sell seats


/*
main function 
*/
int main(int argc, char* argv[]) {
	
	// Checks to see if there are 2 arguments passed
	// Prints error message if condition fails
	if (argc < 2) {
		fprintf (stderr, "Usage:, %s shmid\n", argv[0]);
		exit(1);
	}

	pname = argv[0];	// Initializes first argument to process name
	sscanf (argv[1], "%d", &shmid);		// Stores second argument as decimal into shared memory segment id

    // shmat attaches shared memory to address space of this process.
    // First parameter is id of shared memory
    // Second parameter is address at which calling thread would like shared memory attached
    // Last parameter are flags. 0 = no flags.
	memptr = shmat (shmid, (void *)0, 0);
	// If memptr = -1, shared mem attachment failed
	// Prints message with error exit message
	if (memptr == (char *)-1 ) {
		rpterror ((char *)"shmat failed");
		exit(2);
	}

	// Assigns shared memory pointer to class pointer
	class_ptr = (struct CLASS *)memptr;
	// sells all seats for the class
	sell_seats();

    // shmdt detaches shared memory segment from process.
    // parameter: the address of the shared memory segment
	// return value is 0 if successful, -1 if detachment failed
	ret = shmdt(memptr);
	exit(0);
    return 0;
}

// Sell seats randomly
void sell_seats() {
	int all_out = 0;	// Indicates num of remaining seats

	srand ((unsigned) getpid()); // gets process id
	while ( !all_out) {   /* loop to sell all seats */
		// Checks to make sure there are seats left
		if (class_ptr->seats_left > 0) {
			// Sleeps for random time between 1 and 5 seconds
			sleep ( (unsigned)rand()%5 + 1);
			// Removes a seat
			class_ptr->seats_left--;
			// Sleeps again
			sleep ( (unsigned)rand()%5 + 1);
			// Outputs process name with seats left
			cout << pname << " SOLD SEAT -- " 
			     << class_ptr->seats_left << " left" << endl;
		}
		else {
			all_out++;	// Indicates there are no seats left
			// Outputs message
			cout << pname << " sees no seats left" << endl;
		}
		// sleeps for random time between 1 and 10 seconds
		sleep ( (unsigned)rand()%10 + 1);
	}
}

// given a pointer to a string and prints error name to the screen
void rpterror(char* string) {
	char errline[50];

	sprintf (errline, "%s %s", string, pname);
	perror (errline);
}
