/* shmp1.cpp */

// includes all necessary header files
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

// creates global variable instance of type CLASS with arguments
CLASS myclass = { "4321", "082620", "Operating Systems", 20 };

#define NCHILD	3 /* if file has not been given arguments, creates this */


int	shm_init( void * );								// global variable for shared memory
void	wait_and_wrap_up( int [], void *, int );	// function prototype for wait call
void	rpterror( char *, char * );					// function prototype for report error

// main function 
int main(int argc, char *argv[]) {
	int 	child[NCHILD], i, shmid;	// creates process ids of child processes, process number, shared memory segment id
	void	*shm_ptr;					// shared memory pointer
	char	ascshmid[10], pname[14];	// shared memory segment id as an array, and procss name as array

	shmid = shm_init(shm_ptr);			// initializes the shared memory segment and assigns its identifier to shmid
	sprintf (ascshmid, "%d", shmid);	// stores shmid into the char array ascshmid 

	// Create NCHILD number of child processes
	for (i = 0; i < NCHILD; i++) {
		// attempts to create a child process	
		child[i] = fork();
		switch (child[i]) {
			// indicates the creation of child process failed as result of fork returning -1
		case -1:
			// prints out fork failure pname followed by error message 
			sprintf (pname, "child%d", i+1);
			rpterror ((char *)"fork failed", pname);
			// exits with generic error indication
			exit(1);
			// indicates the creation of successful child process as a result of fork returning 0
			// stores process in "shmc%d" into pname
			// %d represents process number + 1
		case 0:
			sprintf (pname, "shmc%d", i+1);
			// replaces program with shmc1
			// passes process name, id
			// runs until termination
			execl("shmc1", pname, ascshmid, (char *)0);
			// prints error message 
			rpterror ((char *)"execl failed", pname);
			// exits with indication of misuse of built-in functions
			exit (2);
		}
	}
	// waits for all child processes in child to terminate before detaching shared memory segment from shared memory pointer
	// also destroys shared memory
	wait_and_wrap_up (child, shm_ptr, shmid);
    return 0;
}

// initializes shared memory and assigns it to passed pointer
// Returns shared memory ID
int shm_init(void *shm_ptr) {
	int	shmid; 		// Shared memory id

    // shmget creates shared memory identifier.
    // First parameter is  key, which is created with ftok
    // Second parameter is size in bytes of shared memory
    // Last parameter are flags. In this case, 0600 = owner can read/write to shared memory. IPC_CREAT creates data structure.
	shmid = shmget(ftok(".",'u'), sizeof(CLASS), 0600 | IPC_CREAT);

	// Tries to acquire shared memory but failed
	// Prints error message
	if (shmid == -1) {
		perror ("shmget failed");
		exit(3);
	}

    // shmat attaches shared memory to address space of this process.
    // First parameter is id of shared memory
    // Second parameter is address which calling thread would like shared memory attached
    // Last parameter are flags. 0 = no flags.
	shm_ptr = shmat(shmid, (void * ) 0, 0);

	// Tries to attach shared memory to pointer but failed
	// Prints error message
	if (shm_ptr == (void *) -1) {
		perror ("shmat failed");
		exit(4);
	}

    // memcpy copies num bytes of source mem block to destination mem block
    // First parameter is destination
    // Second parameter is source
    // Last parameter is num of bytes to copy.
	memcpy (shm_ptr, (void *) &myclass, sizeof(CLASS) );
	return (shmid);
}

// Waits for all child processes to terminate before detaching shared memory from pointer
// Removes shared memory that is associated with id
void wait_and_wrap_up(int child[], void *shm_ptr, int shmid) {
	int wait_rtn, w, ch_active = NCHILD;		// Indicates when a child is returned, index of child process, and num of child processes active

	// Runs until there are no child processes active
	while (ch_active > 0) {
		// Waits for child process to terminate
		wait_rtn = wait( (int *)0 );
		for (w = 0; w < NCHILD; w++) {
			// decrements number of child processes when child process terminates
			if (child[w] == wait_rtn) {
				ch_active--;
				// Exits and waits for another child child process
				break;
			}
		}
	}

	cout << "Parent removing shm" << endl;
    
	// shmdt detaches the shared memory segment from the process.
    // parameter: the address of the shared memory segment
	shmdt (shm_ptr);
    
	// shmctl performs  a variety of shared memory control operations
    // First parameter is shared memory id
    // Second parameter are commands. In this case IPC_RMID removes shared memory segment and data structure from system.
    // Last parameter is pointer to shmid_ds structure.
	shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0);
	// Exits 
	exit (0);
}

// Prints custom message with parameters concatenated followed by error message
void rpterror(char *string, char *pname)
{
	char errline[50];

	sprintf (errline, "%s %s", string, pname);
	perror (errline);
}
