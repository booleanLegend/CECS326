#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>


/*
 * Given struct
 */
struct myShm {
    int index;
    int response[10];
};


int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("Please provide the correct amount of arguments. [# of child processes] [shared memory segment name].");
    } else {
        printf("Master begins execution\n");
        char* numberOfChildren = argv[1];
        char* sharedMemorySegmentName = argv[2];

        // Give file a size in bytes
        const int SIZE = 4096;

        // Struct base address, from mmap()
        struct myShm *shm_base;

        // File descriptor, from shm_open()
        int shm_fd;

        // shm_open creates or opens a POSIX shared memory object
        // First param is name of shared memory
        // Second param are flags. Multiple flags available: creating, read only, read-write etc.
        // Last parameter sets permissions of shared memory object
        shm_fd = shm_open(sharedMemorySegmentName, O_CREAT | O_RDWR, 0666);

        if (shm_fd == -1) {
            printf("prod: Shared memory failed: %s\n", strerror(errno));
            // Exit 1 here b/c we indicate something failed, not a normal exit. Exit 0 exits program normally.
            exit(1);
        }

        if (shm_base == MAP_FAILED) {
            // close and shm_unlink
            printf("prod: Map failed: %s\n", strerror(errno));
            // Exit 1 here b/c we indicate something failed, not a normal exit. Exit 0 exits program normally.
            exit(1);
        }

        // Configure the size of shared memory segment
        // First param is file descriptor
        // Second param is desired size of file in bytes
        ftruncate(shm_fd, SIZE);

        // mmap establishes a mapping between address space of process and file associated with file descriptor (shm_fd)
        // First param specifies starting address of memory region to be mapped.
        // Second param specifies length
        // Third param specifies access permissions, in this case: read/write
        // Fourth param specifies attrib of mapped region. MAP_SHARED makes mods to region's visibility.
        // Fifth param specifies file descriptor
        // Last param specifies file byte offset at which mapping starts.
        shm_base = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        printf("Master created a shared memory segment named %s.\n", sharedMemorySegmentName);

        // Convert numberOfChildren to int. To be used in for loop.
        int nChildren = atoi(numberOfChildren);

        for (int i = 0; i < nChildren; i++) {

            // Fork creates a new process.
            pid_t cpid = fork();

            // 0 = fork successful
            if (cpid == 0) { 
                char number_str[10];

                // Convert an integer to char to be passed as argument.
                sprintf(number_str, "%d", i);

                // Arguments to be passed to child program.
                char *args[] = {number_str, sharedMemorySegmentName, NULL};

                // Execute the slave program pass above arguments.
                // First param is path to compiled slave program.
                // Second param is args to be passed to slave program.
                execv("./slave", args);

                // Exit child process normally.
                exit(0);
            }
        }

        printf("Master created %s child processes to execute slave.\n", numberOfChildren);
        printf("Master waits for all child processes to terminate.\n");

        // Waiting for both children to terminate (wait(NULL))
        while(wait(NULL) != -1);
        
        printf("Master received termination signals from all %s child processes.\n", numberOfChildren);
        printf("Content of shared memory segment filled by child processes:\n");
        printf("--- content of shared memory ---\n");
        for(int i = 0; i < nChildren; i++) {
            printf("%d\n", shm_base -> response[i]);
        }

        // Removes mapped shared memory segment from address space of process
        // First param is starting address of region.
        // Second param is length of address range.
        if (munmap(shm_base, SIZE) == -1) {
            printf("cons: Unmap failed: %s\n", strerror(errno));
            // Exit 1 here b/c we indicate something failed, not a normal exit. Exit 0 exits program normally.
            exit(1);
        }

        // Closes shared memory segment like a file
        if (close(shm_fd) == -1) {
            printf("cons: Close failed: %s\n", strerror(errno));
            // Exit 1 here b/c we indicate something failed, not a normal exit. Exit 0 exits program normally.
            exit(1);
        }

        // Removes shared memory segment from file system
        // Param: shared memory segment name
        if (shm_unlink(sharedMemorySegmentName) == -1) {
            printf("cons: Error removing %s: %s\n", sharedMemorySegmentName, strerror(errno));
            // Exit 1 here b/c we indicate something failed, not a normal exit. Exit 0 exits program normally.
            exit(1);
        }

        printf("Master removed shared memory segment, and is exiting.\n");
    }

    // Exit 0 exits program normally.
    exit(0);
}