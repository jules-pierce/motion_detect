#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

// my_thread_function() is the function we will spin up in a separate thread.
// After doing so, we will have two threads in our process: one for 
// main() and one for my_thread_function().  The OS will run both threads
// simultaneously.
void* my_thread_function(void* arg);

// We will use a mutex to protect access to variables shared
// by the main() and my_thread_function() threads.  This guarantees 
// that only one thread can access shared variables at a time
pthread_mutex_t my_mutex;

// This structure will be the argument to our thread function.
// We will use it to pass variables to the my_thread_function()
typedef struct my_arg_structure{
	int my_favorite_integer;
	double pi_to_4_decimals;
} my_arg_structure_t;

// These are the shared variables.  Both main() and my_thread_function()
// will be able to access them.  To avoid conflicts, we protect such
// accesses with the mutex.
int loop_counter;
int time_to_quit; 

/***
Main function: will run in one thread
***/
int main( int argc, char *argv[] ){
	pthread_t thread_id;
	my_arg_structure_t a;
	int rv;

	// Initialize the arguments
	a.my_favorite_integer = 24;
	a.pi_to_4_decimals = 3.1416;

	// Initialize the mutex and spin up the second thread
	pthread_mutex_init(&my_mutex, NULL);
	rv = pthread_create(&thread_id, NULL, &my_thread_function, 
		               (void*) &a);

	// Sleep for 10 seconds and then tell the other thread to quit
	printf("main: Other thread is started, going to sleep\n");
	usleep(10 * 1000000);
	printf("main: waking from sleep, telling other thread to quit\n");

	// access a shared variable to tell other thread to quit.  Protect
	// the access with the mutex.  Note that two calls are made, one before
	// access and one after
	pthread_mutex_lock(&my_mutex);
	time_to_quit = 1;
	pthread_mutex_unlock(&my_mutex);

	//sleep 2 more seconds and then exit.  This gives the other thread time
	//to quit
	usleep(2 * 1000000);

	// Now print out the number of times the other thread looped.  Note that
	// this uses a shared variable
	pthread_mutex_lock(&my_mutex);				
	printf("main: loops completed by other thread = %d\n", loop_counter);
	pthread_mutex_unlock(&my_mutex);

	// clean up and exit
	pthread_mutex_destroy(&my_mutex);
	exit(1);
}

/***
thread funcntion
***/
void* my_thread_function(void *arg){
	my_arg_structure_t *b;
	int i;

	// This is how we get access to the argument structure
	b = (my_arg_structure_t*) arg;

	// Here we enter an infinite loop; we'll late the main program
	// tell us when to quit using a shared variable
	loop_counter = 0;
	while(1){
		usleep(0.5 * 1000000);
		printf("  child: my_favorite_integer = %d, pi = %6.4lf\n",
		           b->my_favorite_integer, b->pi_to_4_decimals);

		// Increment the loop counter and see if it is time to quit
		// Protect the access to these shared variables using the mutex
		pthread_mutex_lock(&my_mutex);				
		loop_counter++;
		if(time_to_quit == 1){
			pthread_mutex_unlock(&my_mutex);
			break;
		}
		pthread_mutex_unlock(&my_mutex);
	}
	printf("  child: quitting by command of main...\n");
	return NULL;
}
