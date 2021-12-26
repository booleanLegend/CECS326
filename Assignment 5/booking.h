/* booking.h */
/* Header file to be used with
 * shmp2.cpp and shmc2.cpp
*/


// BUS structure, creates arrays that will store some bus data
struct BUS {
	char bus_number[6];
	char date[9];
	char title[50];
	int seats_left;
};