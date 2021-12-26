#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>


/*
 * Given struct
 */
struct myShm {
    int index;
    int response[10];
};


int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Please provide the correct amount of arguments. [child number] [shared memory segment name].");
    } else {
        printf("Slave begins execution\n");
        char* childNumber = argv[0];

        // Convert a char to int.
        int childNumberInt = atoi(childNumber);

        char* sharedMemorySegmentName = argv[1];

        // Give  file a size in bytes
        const int SIZE = 4096;

        printf("I am child number %s, received shared memory name %s.\n", childNumber, sharedMemorySegmentName);

        // File descriptor, from shm_open()
        int shm_fd;

        // Struct base address, from mmap()
        struct myShm *shm_base;

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

        // Configure size of shared memory segment
        // First param is file descriptor
        // Second param is desired size of file in bytes
        ftruncate(shm_fd,SIZE);

        // mmap establishes a mapping between address space of process and file associated with file descriptor (shm_fd)
        // First param specifies starting address of memory region to be mapped.
        // Second param specifies length
        // Third param specifies access permissions, in this case: read/write
        // Fourth param specifies attrib of mapped region. MAP_SHARED makes mods to region's visibility.
        // Fifth param specifies file descriptor
        // Last param specifies file byte offset at which mapping starts.
        shm_base = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        
        if (shm_base == MAP_FAILED) {
            // close and shm_unlink
            printf("prod: Map failed: %s\n", strerror(errno));
            // Exit 1 here b/c we indicate something failed, not a normal exit. Exit 0 exits program normally.
            exit(1);
        }
        
        // Write to shared memory region.
        shm_base -> index = childNumberInt;
        shm_base -> response[childNumberInt] = childNumberInt;
        printf("I have written my child number to shared memory.\n");
        
        // Removes mapped shared memory segment from address space of process
        // First param is starting address of region.
        // Second param is length of address range.
        if (munmap(shm_base, SIZE) == -1) {
            printf("prod: Unmap failed: %s\n", strerror(errno));
            // Exit 1 here b/c we indicate something failed, not a normal exit. Exit 0 exits program normally.
            exit(1);
        }  

        // Closes shared memory segment like a file
        // Param: file descriptor
        if (close(shm_fd) == -1) {
            printf("prod: Close failed: %s\n", strerror(errno));
            // Exit 1 here b/c we indicate something failed, not a normal exit. Exit 0 exits program normally.
            exit(1);
        }

        printf("Slave closed access to shared memory and terminates\n");
    }

    // Exit 0 exits program normally.
    exit(0);
}