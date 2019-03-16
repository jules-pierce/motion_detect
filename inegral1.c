#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

double f(double num);
double integrate (int start, int end, int n);

int main(int argc, char *argv[]){
    int start, end, n, i;
    double ans;
    
    if (argc != 5){
        fprintf (stderr, "usage is: %s start end low_rect high_rect\n", argv[0]);
        exit (1);
    }

    
    start = atoi (argv[1]);
    end = atoi (argv[2]);

    for (n = atoi(argv[3]); n <= atoi (argv[4]); n++){
        ans = integrate (start, end, n);
        fprintf (stderr, "%d    %lf\n", n, ans);
    }
    
}

double integrate (int start, int end, int n){
    double delta, i;
    double sum = 0;

    delta = (double)(end-start)/n;
    
    for (i = 0; i < n; i++){
        sum += f ((double)start + i*delta);
        sum += f ((double)start + (i+1)*delta);
    }
    
    return sum * delta/2;
}

double f (double num){
    return num*num;
}

