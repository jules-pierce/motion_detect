/* A simple server. The port number is passed in as an argument. */
//sometime in motion detection, i believe after running imagesnap, the program exists all the way out so it can't receive another command
//man 2 unlink
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <sys/stat.h>

#include "bmp_io.h"
#include "imgstuff.h"

#define PSIZE 256

void* my_thread_function(void* arg);
pthread_mutex_t my_mutex;

int time_to_quit = 0;
char name [80];

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	unsigned char buffer[PSIZE], ack [PSIZE];
	struct sockaddr_in serv_addr, cli_addr;
	int n, i, j, k, l, m;
    int rows, cols;
    unsigned char *picdat;
    time_t now;
    struct tm *time_info;
    char filename [80], filename1[80];
    char command [200], command1[200];
    struct timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = 1000000;
    FILE *indes;
    int num_read;
    int child_pid;
    unsigned char *pic1, *pic2;
    double total, divide;
    double thresh = 200;
    double mean1, mean2, diff;
    pthread_t thread_id;
    int rv;

	if (argc < 2) {
		 fprintf(stderr,"ERROR, no port provided\n");
		 exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) fprintf(stderr, "ERROR opening socket");

	// zero out the string
	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		fprintf(stderr, "ERROR on binding");
	}
    //listening to socket
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0){
		fprintf(stderr, "ERROR on accept");
	}	
	
	for(i = 0; i < 5; i++){
        //receiving command
		bzero(buffer, 256); //zeros buffer
		n = read(newsockfd, buffer, PSIZE); //reading message from network
		if(n < 0) fprintf(stderr, "ERROR reading from socket\n");
        
        //sending acknowledgement
        n = write(newsockfd, buffer, PSIZE);
        
        //code for grabpic
        switch (buffer[1]){
            case 13: //add if else for jpg and bmp
                printf ("got grab pic command\n");
                now = time (NULL);
                time_info = localtime (&now);
                sprintf (filename, "image%02d-%02d-%04d_%02d-%02d-%02d.jpg", 1 + time_info->tm_mon, time_info->tm_mday, 1900 + time_info->tm_year, time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
                
                sprintf (command, "./imagesnap %s", filename);
                system (command);
                
                indes = fopen (filename, "rb");
                buffer[0] = strlen(filename);
                fprintf (stderr, "buffer[0] = %d\n", buffer [0]);
                buffer[1] = 10;
                buffer [2] = 0;
                for (j = 0; j <= strlen(filename); j++){
                    buffer [10+j] = filename [j];
                }
                n = write (newsockfd, buffer, PSIZE);
                //while ((n = read (newsockfd, ack, PSIZE))== -1);
                n = read (newsockfd, ack, PSIZE);
                fprintf (stderr, "n = %d\n", n);
                if (memcmp ((void*)buffer, (void*)ack, PSIZE) != 0){
                    fprintf (stderr, "error in acknowledgement: 01\n");
                    goto exitnow;
                }
                while (1){
                    //nanosleep(&sleeptime, NULL);
                    bzero (buffer, PSIZE);
                    num_read = fread (buffer+10, 1, PSIZE - 10, indes);
                    if (num_read > 0) {
                        buffer [0] = num_read;
                        buffer [1] = 100;
                        buffer [2] = 0;
                        n = write (newsockfd, buffer, PSIZE); //sending data
                        n = read (newsockfd, ack, PSIZE); //receiving acknowledgement
                        if (memcmp ((void*)buffer, (void*)ack, PSIZE) != 0){
                            fprintf (stderr, "Error in acknowledgement: 02\n");
                            goto exitnow;
                            /*buffer[1] = 101;
                            n = write (newsockfd, buffer, PSIZE);*/
                        }
                    }
                    else{
                        buffer [0] = 0;
                        buffer [1] = 11;
                        n = write (newsockfd, buffer, PSIZE);
                        n = read (newsockfd, ack, PSIZE);
                        if (memcmp ((void*)buffer, (void*) ack, PSIZE) != 0){
                            fprintf (stderr, "error in acknowledgement: 03\n");
                            goto exitnow;
                        }
                        break;
                    }
                }
                break;
            case 14:
                printf ("got grab audio command\n");
                now = time(NULL);
                time_info = localtime (&now);
                sprintf (filename, "audio_%02d-%02d-%04d_%02d-%02d-%02d.wav", 1+time_info->tm_mon, time_info->tm_mday, 1900+time_info->tm_year, time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
                child_pid = fork();
                if (child_pid == 0){
                    sprintf (command, "sox -d -r8000 -c1 -b16 -e signed-integer %s 2> /dev/null", filename);
                    //sox program doesn't appear to be doing anything
                    system (command);
                    exit (1);
                }
                
                sleep (10);
                system ("killall sox");
                
                indes = fopen (filename, "rb");
                buffer[0] = strlen(filename);
                //buffer[1] and [2] need to be updated on wifi
                buffer[1] = 10;
                buffer [2] = 0;
                
                for (j = 0; j <= strlen(filename); j++){
                    buffer [10+j] = filename [j];
                }
                n = write (newsockfd, buffer, PSIZE);
                n = read (newsockfd, ack, PSIZE);
                if (memcmp ((void*)buffer, (void*)ack, PSIZE) != 0){
                    fprintf (stderr, "error in acknowledgement: 04\n");
                    goto exitnow;
                }
                
                while (1){
                    //nanosleep(&sleeptime, NULL);
                    num_read = fread (buffer+10, 1, 246, indes);
                    if (num_read > 0) {
                        buffer [0] = num_read;
                        //buffer[1] and [2] need to updated on wifi
                        buffer [1] = 100;
                        buffer [2] = 0;
                        n = write (newsockfd, buffer, PSIZE);
                        n = read (newsockfd, ack, PSIZE);
                        if (memcmp ((void*)buffer, (void*)ack, PSIZE) != 0){
                            fprintf (stderr, "Error in acknowledgement: 05\n");
                            goto exitnow;
                        }
                    }
                    else{
                        //check these buffer values
                        buffer [0] = 0;
                        buffer [1] = 11;
                        n = write (newsockfd, buffer, PSIZE);
                        n = read (newsockfd, ack, PSIZE);
                        if (memcmp ((void*)buffer, (void*)ack, PSIZE) != 0){
                            fprintf (stderr, "Error in acknowledgement: 06\n");
                            goto exitnow;
                        }
                        break;
                    }
                }
                break;
            case 15:
                fprintf (stderr, "got motion detect command\n");
                //delete all existing pics
                system ("rm -f image*.bmp");
                
                //start second thread
                pthread_mutex_init (&my_mutex, NULL);
                rv = pthread_create (&thread_id, NULL, &my_thread_function, NULL);
                
                
                usleep(10 * 1000000);
                
                //check if motion was detected, if it was write to buffer
                //only writes to buffer if motion was detected
                break;
            case 16:
                //checks motion status and returns it
                pthread_mutex_lock(&my_mutex);
                buffer [0] = strlen (name);
                buffer [1] = 16;
                buffer[2] = 30;
                for (i = 0; i <= strlen(name); i++){
                    buffer [3 + i] = name [i];
                }
                time_to_quit = 0;
                pthread_mutex_unlock(&my_mutex);
                n = write (newsockfd, buffer, PSIZE);
                break;
            case 250:
                printf ("exiting program\n");
                goto exitnow;
            case 251:
                pthread_mutex_lock(&my_mutex);
                time_to_quit = 1;
                pthread_mutex_unlock(&my_mutex);
                usleep(1 * 1000000);
                goto exitnow;
            default:
                printf ("no command\n");
                break;
            /*bzero (buffer, 256);
            m = 0;
            system ("./imagesnap");
            input_bmp (snapshot.jpg, &cols, &rows, &picdat, MONO);
            l = 0;
            for (j = 0; j < rows, j++){
                for (k = 0; k < cols, k++){
                    if (l == 255){
                        n = write(newsockfd, buffer, strlen(buffer));
                        if (n<0) fprintf (stderr, "ERROR writing to socket");
                        l = 0;
                        m = 0;
                        bzero (buffer, 256);
                    }
                    else {
                        buffer [m] = picdat [mi (j, k, cols)];
                        m++;
                        l++;
                    }
                }
            }*/
        }

	}
    
exitnow:
	close(newsockfd);
	close(sockfd);
	return 0; 
}

void* my_thread_function (void *arg){
    int i, j;
    char filename[80];
    char command[200];
    char filename1[80];
    char command1[200];
    char file[80], file1 [80];
    int cols, rows;
    unsigned char *pic1, *pic2;
    double mean1, mean2;
    int divide;
    double diff;
    double thresh = 200;
    time_t now;
    struct tm *time_info;
    struct timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = 1000000;
    double total;
    
    while (1){
        usleep (0.5 * 1000000);
        //take two pics
        now = time (NULL);
        time_info = localtime (&now);
        sprintf (filename, "image%02d-%02d-%04d_%02d-%02d-%02d.bmp", 1 + time_info->tm_mon, time_info->tm_mday, 1900 + time_info->tm_year, time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
        
        sprintf (command, "./imagesnap -w .25 %s", filename);
        system (command);
        
        nanosleep(&sleeptime, NULL);
        
        now = time (NULL);
        time_info = localtime (&now);
        sprintf (filename1, "image%02d-%02d-%04d_%02d-%02d-%02d.bmp", 1 + time_info->tm_mon, time_info->tm_mday, 1900 + time_info->tm_year, time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
        
        sprintf (command1, "./imagesnap -w .25 %s", filename1);
        system (command1);
        //input pics
        input_bmp(filename, &cols, &rows, &pic1, MONO);
        input_bmp(filename1, &cols, &rows, &pic2, MONO);
        
        //find mean value of each pic
        mean1 = 0;
        mean2 = 0;
        divide = rows * cols;
        
        for (i = 0; i < rows; i++){
            for (j = 0; j < cols; j++){
                mean1 += pic1 [mi (i, j, cols)];
                mean2 += pic2 [mi (i, j, cols)];
            }
        }
        
        mean1 = mean1/divide;
        mean2 = mean2/divide;
        if (mean1 > mean2){
            diff = floor (mean1 - mean2 + 0.5);
            for (i = 0; i < rows; i++){
                for (j = 0; j < cols; j++){
                    pic2 [mi (i, j, cols)] += diff;
                }
            }
        }
        else if (mean2 > mean1){
            diff = floor (mean2-mean1 + 0.5);
            for (i = 0; i < rows; i++){
                for (j = 0; j < cols; j++){
                    pic1 [mi (i, j, cols)] += diff;
                }
            }
        }
        
        //check for motion between two most recent pics
        total = 0;
        for (i = 0; i < rows; i++){
            for (j = 0; j < cols; j++){
                total += (pic1 [mi (i, j, cols)] - pic2 [mi (i, j, cols)]) * (pic1 [mi (i, j, cols)] - pic2 [mi (i, j, cols)]);
            }
        }
        
        if (total/divide > thresh){
            system ("rm -f zmage*.bmp");
            for (i = 1; i <= strlen(filename); i++){
                file [i] = filename [i];
            }
            for (i = 1; i <= strlen(filename1); i++){
                file1 [i] = filename1 [i];
            }
            file [0] = 'z';
            file1 [0] = 'z';
            system ("mv filename file");
            system ("mv filename1 file1");
            
            pthread_mutex_lock(&my_mutex);
            for (i = 0; i <= strlen(file); i++){
                name [i] = file [i];
            }
            pthread_mutex_unlock(&my_mutex);
        }
        
        pthread_mutex_lock(&my_mutex);
        if (time_to_quit == 1){
            break;
        }
        pthread_mutex_unlock(&my_mutex);
    }
    free (pic1);
    free (pic2);
    pthread_mutex_unlock(&my_mutex);
    return NULL;
}
