/* shmp3.cpp */
#include "booking3.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>	// not needed for this assignment
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>

using namespace std;

#define NCHILD 3


int shm_init( void * );								// global var for shdmem
void wait_and_wrap_up( int [], void *, int );		// func prototype for wait
void rpterror( char *, char * );					// func prototype for report error
void sem_init();


int main(int argc, char *argv[]) {
	int child[NCHILD], i, shmid;					// creates pids of child processes, process number, shared mem segment id
	void *shm_ptr;									// shdmem ptr
	char ascshmid[10], pname[14];					// shared mem seg id as array and process name as array

	sem_init(&(mybus.sem1), 1, 1);					// initializes semaphore to bus structure sem address in 1st param, 2nd param allows for sharing in threads of processes, 3rd param initializes semaphore

	shmid = shm_init(shm_ptr);						// initializes shared mem seg and assigns identifier to shmid
	sprintf (ascshmid, "%d", shmid);				// stores shmid into char array ascshmid
	
	// outputs msg for bus structure
	cout << "Bus " << mybus.tour1.bus_number << " for "
		<< mybus.tour1.title << " on " << mybus.tour1.date << ", "
		<< mybus.tour1.seats_left << " seats available. " << endl;
	cout << "Booking begins: " << endl << endl;

	// create NCHILD number of child processes
	for (i = 0; i < NCHILD; i++) {
		// attempts to create a child process
		child[i] = fork();
		switch (child[i]) {
			// indicates creation of child process failed b/c of fork returning -1		
			case -1:
				// prints out fork failure pname followed by error msg
				sprintf (pname, "child%d", i+1);
				rpterror ((char *)"fork failed", pname);
				// exits with generic error indication
				exit(1);
			// indicates creation of successful child process b/c of fork returning 0
			// stores process in "shmc%d" into pname
			// %d represents process number + 1
			case 0:
				sprintf (pname, "shmc%d", i+1);
				// replaces program with shmc3
				// passes process name, id
				// runs until termination
				execl("shmc3", pname, ascshmid, (char *)0);
				// prints error message
				rpterror ((char *)"execl failed", pname);
				exit (2);
		}
	}
	// waits for all child processes in child to terminate before detaching shared mem seg from shared mem ptr
	// also destroys shdmem
	wait_and_wrap_up (child, shm_ptr, shmid);
}


// initializes shdmem and assigns it to passed ptr
// returns shdmemid
int shm_init(void *shm_ptr) {
	int shmid;

	// shmget creates shdmem identifier.
    // First param is key, which is created with ftok
    // Second param is size in bytes of shdmem
    // Last param is flag. 0600 = owner can read/write to shdmem. IPC_CREAT creates data structure.
	shmid = shmget(ftok(".",'u'), sizeof(BUS), 0600 | IPC_CREAT);
	
	// tries to acquire shdmem but failed
	// prints error msg
	if (shmid == -1) {
		perror ("shmget failed");
		exit(3);
	}

	// shmat attaches shdmem to address space of this process.
    // First param is id of shdmem
    // Second param is address which calling thread wants shdmem attached to
    // Last param is flag. 0 = no flags.
	shm_ptr = shmat(shmid, (void * ) 0, 0);
	
	// tries to attach shdmem to ptr but failed
	// prints error msg
	if (shm_ptr == (void *) -1) {
		perror ("shmat failed");
		exit(4);
	}
	
	// memcpy copies num bytes of source mem block to destination mem block
    // First param is destination
    // Second param is source
    // Last param is num of bytes to copy.
	memcpy (shm_ptr, (void *) &mybus, sizeof(BUS) );

	return (shmid);
}


// waits for all child process to terminate before detaching shdmem from ptr
// removes shdmem associated with id
void wait_and_wrap_up(int child[], void *shm_ptr, int shmid) {
	int wait_rtn, w, ch_active = NCHILD;	// indicates when child is returned, index of child process, and num of child processes active

	// runs until there are no child processes active
	while (ch_active > 0) {
		wait_rtn = wait( (int *)0 );
		for (w = 0; w < NCHILD; w++) {
			// decrements num of of child processes when child process terminates
			if (child[w] == wait_rtn) {
				ch_active--;
				// exits and waits for another child process
				break;
			}
		}
	}
	cout << "Parent removing shm" << endl;

	// shmdt detaches shdmem seg from process.
    // param: address of shdmem seg
	shmdt (shm_ptr);

	// shmctl performs a variety of shdmem control operations
    // First param is shdmemid
    // Second param is cmds. IPC_RMID removes shdmem seg, data struct from sys.
    // Last param is ptr to shmid_ds struct.
	shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0);

	// destroys the semaphore
	sem_destroy(&(mybus.sem1));
	exit (0);
}


// prints custom msg w/ params concatenated followed by error msg
void rpterror(char *string, char *pname) {
	char errline[50];
	sprintf (errline, "%s %s", string, pname);
	perror (errline);
}