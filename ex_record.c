#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]){
	time_t now;
	struct tm *time_info;
	char filename[80];
	char command[100];
	int child_pid;

	// Get the time of day
	now = time(NULL); // get the time from the clock in seconds since the epoch
	time_info = localtime(&now); // populate the info structure with data
	sprintf(filename, "audio_%02d-%02d-%04d_%02d-%02d-%02d.wav",
	             1 + time_info->tm_mon,
							 time_info->tm_mday,
							 1900 + time_info->tm_year,
							 time_info->tm_hour,
							 time_info->tm_min,
							 time_info->tm_sec);
	printf("%s\n", filename);

	// Kick off a recording process
	child_pid = fork();
	if( child_pid == 0 ){
		sprintf(command, "sox -d %s 2> /dev/null", filename);
		system(command);
		exit(1);
	}	

	// Sleep for 10 seconds, then kill the recording process
	sleep( 10 ); // sleep
	system("killall sox"); // kill the recording process

	// Ready to move on!
	printf("audio file is ready to process\n");
}
