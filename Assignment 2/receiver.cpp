#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/msg.h>

using namespace std;

// Struct to get message buffer
struct my_msgbuf {
	long mtype;
	char message[75];
};

int main (int argc, char* argv[]) {

    if (argc > 0) {
        printf("Receiver, PID %d, begins execution", getpid());

        // Holds message queue id this program receives as argument; later used to receive message
        int qid;
        
		// Converts char argument (message queue id) to int.
        sscanf(argv[0], "%d", &qid);
        my_msgbuf msg;

        // Calculates message size.
        int size = sizeof(msg)-sizeof(long);

        printf("\nReceiver received message queue id %d through commandline parameter", qid);

        // Receives message.
        // First param is message queue id
        // Second param is pointer to msgbuf
        // Third param is size of message
        // Fourth param is messagetype number
        // The last param is MessageFlag. Set to 0. Specifies action to take if message cannot be sent.
        msgrcv(qid, (struct my_msgbuf *)&msg, size, 128, 0);
        printf("\nReceiver: retrieved the following message from message queue: \n");
        cout << msg.message << endl;
        printf("\nReceiver terminates\n");
    }
	
    return 0;
}