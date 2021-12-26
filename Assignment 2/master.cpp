#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;

int main() {

    printf("Master, PID %d, begins execution", getppid());

    // Used for creating a message.
	key_t key;

    // Creates a key. First parameter points to this file (master.cpp), second parameter can be anything.
    if ((key = ftok( ".", ' ')) == -1) {
		perror("key");
		exit(1);
    }

    // Creates a message id.
    // IPC_CREAT creates a message queue if key specified not already have associated ID
    // 0600 is equal to cmod0600, which gives the owner the permission to read and write
    int qid = msgget(key, IPC_CREAT|0600);

    printf("\nMaster acquired a message queue, id %d", qid);
    
	// Flush internal buffer. Prevents previous printf from printing again.
    fflush(stdout);

    // Holds message id in a char. Will be used as parameter for sender & receiver programs.
    char qidNumber[75];

    // Converts qid (int) to the qidNumber (char).
    sprintf(qidNumber, "%d", qid);

    // Creates first child (sender)
    pid_t cpid = fork();

    // 0 = fork successful
    if (cpid == 0) { 
        printf("\nMaster created a child process with PID %d to execute sender", getpid());
        
		// Arguments to be passed to child program
        char *args[] = {qidNumber, NULL};
        
		// Executes child program; passed above arguments
        execv("./sender", args);

        // Exits child process.
        exit(0);
    }

    // Creates second child (receiver)
    cpid = fork();
	if (cpid == 0) {
		printf("\nMaster created a child process with PID %d to execute receiver", getpid());
        
		// Arguments to be passed to child program. 
        char *args[] = {qidNumber, NULL};

        // Executes child program; passes to above arguments.
        execv("./receiver", args);

        // Exits child process.
        exit(0);
    }

    printf("\nMaster wait for both child processes to terminate");
    
	// waiting for both children to terminate (wait(NULL))
    while(wait(NULL) != -1);

    // Destroys message queue, gives it qid and destroy command (IPC_RMID), third parameter is buffer command.
    msgctl(qid, IPC_RMID, NULL); 
    
    printf("\nMaster received termination signals from both child processes, removed message queue, and terminates\n");
    exit(0);
}
