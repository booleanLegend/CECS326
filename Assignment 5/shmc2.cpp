/* shmc2.cpp */
#include "booking.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>

using namespace std;


// Declares new BUS structure
BUS *bus_ptr;

void *memptr;
char *pname;
int shmid, ret;

// Declares function prototypes
void rpterror(char *), srand(), perror(), sleep();
void sell_seats();

sem_t *sem;
char *semName;


int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf (stderr, "Usage:, %s shmid\n", argv[0]);
		exit(1);
	}
	pname = argv[0];
	sscanf (argv[1], "%d", &shmid);
	semName = argv[2];

	// shmat attaches shared memory to address space of process
	// First param is id of shared memory
	// Second param is address which calling thread wants shmd attached to
	// Last param is flag, 0 = no flags
	memptr = shmat (shmid, (void *)0, 0);

	// If memptr = -1, shared mem attachment failed
	// Prints msg w/ error exit msg
	if (memptr == (char *)-1 ) {
		rpterror ((char *)"shmat failed");
		exit(2);
	}

	// Assigns shared mem ptr to bus ptr
	bus_ptr = (struct BUS *)memptr;
	

	// sem_open opens connection between named sem and process
	// First param is name of sem. Process gets from parent
	// Second param is initial value to set to sem to, such as 1
	if ((sem = sem_open(semName, 1)) == SEM_FAILED) {
		perror("sem_open failed");
		exit(1);
	}

	// sells all seats for bus
	sell_seats();

	// shmdt detaches shared mem segment from process
	// param is address of shared mem segment
	ret = shmdt(memptr);
	exit(0);
	return 0;
}


// Function that sells seats randomly
void sell_seats() {

	// Indicates num of remaining seats
	int all_out = 0;

	// gets pid
	srand ( (unsigned) getpid() );


	while ( !all_out) { /* loop to sell all seats */
	sem_wait(sem);
		// checks that seats are still available
		if (bus_ptr->seats_left > 0) {
			// sleeps for random time between 1 and 3 seconds
			sleep ( (unsigned)rand()%2 + 1);
			
			// removes seat
			bus_ptr->seats_left--;
			
			// sleeps again between 1 and 5 seconds
			sleep ( (unsigned)rand()%5 + 1);

			// outputs process name w/ seats left
			cout << pname << " SOLD SEAT -- "
				<< bus_ptr->seats_left << " left" << endl;
		} else {
			// indicates there are no more seats
			all_out++;

			// outputs msg
			cout << pname << " sees no seats left" << endl;
		}
		
		// sem_post unlocks sem referenced by sem ptr, increases val from 0 to 1
		// Param is ptr to sem
		sem_post(sem);
		// sleeps for random time between 1 and 5 seconds
		sleep ( (unsigned)rand()%5 + 1);
	}
}


// Function given a ptr to a string and prints error name to screen
void rpterror(char* string) {
	char errline[50];
	sprintf (errline, "%s %s", string, pname);
	perror (errline);
}