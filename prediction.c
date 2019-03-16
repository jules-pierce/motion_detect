#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "bmp_io.h"
#include "imgstuff.h"

int main(int argc, char *argv[] ){
	int i, j;
    double x;
    double count = 0;
    double prob;
    int rows, cols;
    unsigned char *picdat, *picdat1, *picdat2, *picdat3, *picdat4, *picdat5;
    double expected_xx, sum_xx, total_xx;
    double expected_xy, sum_xy, total_xy;
    double a;
    int b;
    double xn, x1, x2, e1;
    int en;
    int step_size = 10;
    int num_bits = 3;
    int low_lim, upp_lim;
    
    if ( argc != 4){
        fprintf( stderr, "usage is: %s in.bmp out.bmp out1.bmp\n", argv[0]);
        exit (1);
    }

    input_bmp (argv [1], &cols, &rows, &picdat, MONO);
    picdat1 = (unsigned char*) malloc (rows * cols * sizeof (unsigned char));
    picdat2 = (unsigned char*) malloc (rows * cols * sizeof (unsigned char));
    picdat3 = (unsigned char*) malloc (rows * cols * sizeof (unsigned char));
    picdat4 = (unsigned char*) malloc (rows * cols * sizeof (unsigned char));
    picdat5 = (unsigned char*) malloc (rows * cols * sizeof (unsigned char));
    
    sum_xx = 0;
    total_xx = 0;
    sum_xy = 0;
    total_xy = 0;
    
    /*calculate picture prediction and error based on expected values 
     y = ax-- x is last pixel, y is current pixel
     a = E[xy]/E[xx]*/
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            sum_xx += picdat [mi (i, j, cols)] * picdat [mi (i, j, cols)];
            total_xx += 1;
            if (j != cols-1){
                sum_xy += picdat [mi (i, j, cols)] * picdat [mi (i, j+1, cols)];
                total_xy += 1;
            }
        }
    }
    expected_xx = sum_xx/total_xx;
    expected_xy = sum_xy/total_xy;
    
    a = expected_xy / expected_xx;
    fprintf (stderr, "a = %lf\n", a);
    
    //don't know what to do for things in the left column-- rn they are just being copied exactly
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            if (j ==0) {
                picdat1 [mi (i, j, cols)] = picdat [mi (i, j, cols)];
            }
            else {
                picdat1 [mi (i, j, cols)] = picdat [mi (i, j-1, cols)] * a;
            }
        }
    }
    
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            picdat2 [mi (i, j, cols)] = fabs ((double)(picdat1 [mi (i, j, cols)] - picdat [mi (i, j, cols)]));
        }
    }
    
    /*calculate prediction and error based on difference
     x = C + (B-A)
     */
    
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            if (j == 0 || i == 0) {
                picdat3 [mi (i, j, cols)] = picdat [mi (i, j, cols)];
            }
            else {
                b = picdat [mi (i, j-1, cols)] + (picdat [mi (i-1, j, cols)] - picdat [mi (i-1, j-1, cols)]);
            }
            if (b > 255) b = 255;
            else if (b < 0) b = 0;
            picdat3 [mi (i, j, cols)] = b;
        }
    }
    
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            picdat4 [mi (i, j, cols)] = fabs ((double)(picdat3 [mi (i, j, cols)] - picdat [mi (i, j, cols)]));
        }
    }
    
    low_lim = -(1 << num_bits - 1) * step_size;
    upp_lim = -low_lim - step_size;
    printf ("low_lim = %d   upp_lim = %d\n", low_lim, upp_lim);
    //calculate prediction and error based on lossless compression
    for (i = 0; i < rows; i++){
        for (j = 0; j < cols; j++){
            if (j == 0) x1 = 128;
            
            //calculate e(n) and quantize it
            en = picdat [mi (i, j, cols)] - x1;
            en = (en / step_size) * step_size;
            if (en < low_lim) en = low_lim;
            else if (en > upp_lim) en = upp_lim;
            
            //add e(n) to x(n-1) to calculate x(n)
            x = x1 + en;
            
            //check if x(n) is in range
            if (x < 0) x = 0;
            else if (x > 255) x = 255;
            else x = floor (x+0.5);
            
            //output x(n) to file
            picdat5 [mi (i, j, cols)] = x;
            
            //reset
            x1 = x;
        }
    }
    
    output_bmp (argv [2], cols, rows, picdat2, MONO);
    output_bmp (argv [3], cols, rows, picdat4, MONO);
    output_bmp ("zzz.bmp", cols, rows, picdat1, MONO);
    output_bmp ("yyy.bmp", cols, rows, picdat3, MONO);
    output_bmp ("aaa.bmp", cols, rows, picdat5, MONO);
    free (picdat);
    free (picdat1);
    free (picdat2);
    free (picdat3);
    free (picdat4);
}

