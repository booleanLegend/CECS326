#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/msg.h>

using namespace std;

// Struct to get message
struct my_msgbuf {
    long mtype;
    char message[75];
};

int main (int argc,char* argv[]) {
	if (argc > 0) {
		int qid;
        
		// Converts cmdline argument (message id) from a char to an int.
        sscanf(argv[0], "%d", &qid);
		my_msgbuf msg;

        // Defines message type. Can be anything, as long as sender and receiver are same.
        msg.mtype = 128;

        // Calculates message size
        int size = sizeof(msg)-sizeof(long);

        // User input is stored here.
        string message;

        // Pauses program, ensures receiver is running. Also, declutters output.
        sleep(2);
		printf("Sender, PID %d, begins execution", getpid());
		printf("\nSender received message queue id %d through commandline parameter", qid);
		printf("\nPlease input your message: \n");
		
		// Gets user input
		getline (cin, message);
		
		// Puts message in msgbuf
        strcpy(msg.message, message.c_str());
		
		// Sends message
        // First param is message queue id
        // Second param is pointer to msgbuf
        // Third parame is size of message
        // The last param is MessageFlag. Set to 0. Specifies action to take if message cannot be sent.
        msgsnd(qid,(my_msgbuf *)&msg, size, 0);

        printf("\nSender sent message to message queue");
        printf("\nSender terminates\n");
    }

    return 0;
}