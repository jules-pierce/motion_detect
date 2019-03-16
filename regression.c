
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[] ){
	int i, j;
	unsigned int start_seed;
    double m, b, m1, b1;
    int max;
    double x[100], y[100];
    double y1, r;
    double sum_yx, sum_x, sum_xx, sum_y, avg_x, avg_y;
    
    if ( argc != 5){
        fprintf( stderr, "usage is: %s start_seed m b max\n", argv[0]);
        exit (1);
    }
    
    m = atof (argv [2]);
    b = atof (argv [3]);
    max = atoi (argv[4]);
    
    start_seed = atoi( argv[1] );
    srand (start_seed);

    for (i = 0; i < 100; i++){
        x[i] = max * (double)(rand())/RAND_MAX;
        y1 = m * x[i] + b;
        r = (sqrt (-2 * log ((double)(rand())/RAND_MAX)) * cos (6.283 * (double)(rand())/RAND_MAX));
        y[i] = y1 + r;
        printf ("%lf     %lf\n", x[i], y[i]);
    }
    
    sum_x = 0;
    sum_xx = 0;
    sum_y = 0;
    sum_yx = 0;
    
    for (i = 0; i< 100; i++){
        sum_x += x[i];
        sum_xx += x[i] * x[i];
        sum_y += y[i];
        sum_yx += y[i] * x[i];
    }
    
    avg_x = sum_x / 100;
    avg_y = sum_y / 100;
    
    m1 = (sum_yx - (avg_y * sum_x))/(sum_xx - (avg_x * sum_x));
    b1 = avg_y - (m1 * avg_x);
    
    printf ("y = %lfx + %lf\n", m1, b1);
}


