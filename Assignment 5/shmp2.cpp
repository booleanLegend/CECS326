/* shmp2.cpp */
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


// Creates global variables instance of BUS structure
BUS mybus = { "4321", "11262021", "Grand Canyon Tour", 20 };


#define NCHILD 3	// creates 3 child processes if not given
#define SEMNAME "shmp2Sem"

int shm_init( void * , sem_t *);								// global var for shdmem
void wait_and_wrap_up( int [], void *, int , sem_t *);		// func prototype for wait
void rpterror( char *, char * );					// func prototype for report error


int main(int argc, char *argv[]) {
	int child[NCHILD], i, shmid;		// creates pids of child processes, process number, shared mem segment id
	void *shm_ptr;						// shdmem ptr
	char ascshmid[10], pname[14];		// shared mem seg id as array and process name as array
	sem_t *sem;
	shmid = shm_init(shm_ptr, sem);			// initializes shared mem seg and assings indentifier to shmid
	sprintf (ascshmid, "%d", shmid);	// stores shmid into char array ascshmid

	// outputs msg for bus structure
	cout << "Bus " << mybus.bus_number << " for " 
		<< mybus.title << " on " << mybus.date << ", " 
		<< mybus.seats_left << " seats available. " 
		<< endl;
	cout << "Booking begins: \n" << endl;

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
				// replaces program with shmc1
				// passes process name, id
				// runs until termination
				execl("shmc2", pname, ascshmid, SEMNAME, (char *)0);
				// prints error message
				rpterror ((char *)"execl failed", pname);
				// exits with indication of misuse of built-in functs
				exit (2);
		}
	}
	// waits for all child processes in child to terminate before detaching shared mem seg from shared mem ptr
	// also destroys shdmem
	wait_and_wrap_up (child, shm_ptr, shmid, sem);
	return 0;
}


// initializes shdmem and assigns it to passed ptr
// returns shdmemid
int shm_init(void *shm_ptr, sem_t *sem) {
	int shmid; // shdmemid

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


	// sem_open opens connection between named sem and process
	// First param is name of sem. 
	// Second param is flag. O_CREAT creates sem
	// Third param is file perms. 0600 = can read/write sem
	// Last param is init val set to sem
	sem = sem_open(SEMNAME, O_CREAT, 0666, 1);

	// if sem fails to open, output msg and exit code
	if (sem == SEM_FAILED) {
		perror("sem_open failed");
		exit(1);
	}

	return (shmid);
}


// waits for all child process to terminate before detaching shdmem from ptr
// removes shdmem associated with id
void wait_and_wrap_up(int child[], void *shm_ptr, int shmid, sem_t *sem) {
	int wait_rtn, w, ch_active = NCHILD;	// indicates when child is returned, index of child process, and num of child processes active

	// runs until there are no child processes active
	while (ch_active > 0) {
		// waits for child process to terminate
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

	// sem_unlink removes sem from sys by name. Param = name of sem
	sem_unlink(SEMNAME);

	// sem_cloe closes named sem opened by sem_open. Param = name of sem
	sem_close(sem);
	
	// exits
	exit (0);
}


// prints custom msg w/ params concatenated followed by error msg
void rpterror(char *string, char *pname) {
	char errline[50];
	sprintf (errline, "%s %s", string, pname);
	perror (errline);
}