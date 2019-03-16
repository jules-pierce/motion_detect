#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]){
	time_t now;
	struct tm *time_info;
	char filename[80];

	now = time(NULL); // get the time from the clock in seconds since the epoch
	time_info = localtime(&now); // populate the info structure with data
	sprintf(filename, "image%02d-%02d-%04d_%02d-%02d-%02d",
	             1 + time_info->tm_mon,
							 time_info->tm_mday,
							 1900 + time_info->tm_year,
							 time_info->tm_hour,
							 time_info->tm_min,
							 time_info->tm_sec);
	printf("%s\n", filename);
}
