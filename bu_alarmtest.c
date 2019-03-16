#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]){
	int first_time = 1;
	int child_pid;
	useconds_t sleep_time_us = 1000000; // 1 Second

	while(1){
		if( first_time == 1 ){
			first_time = 0;
			child_pid = fork();
			if( child_pid == 0 ){
				system("sox air_raid.wav -d");
				exit(1);
			}
		}	
		printf("sleeping 1 second\n");
		usleep(sleep_time_us);
	}	
}
