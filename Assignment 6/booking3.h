/* booking3.h
 * Header file to be used with
 * shmp3.cpp and shmc3.cpp
 */

#include <semaphore.h> // header file needed for POSIX semaphore


struct TOUR {
	char bus_number[6];
	char date[9];
	char title[50];
	int seats_left;
};


struct BUS {
	sem_t sem1; // semaphore to control access to tour data below
	TOUR tour1 = { "4321", "11262021", "Grand Canyon Tour", 20 };
} mybus;