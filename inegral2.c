#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

double func (double mu, double t, double x);
double integrate (int start, int end, int n, double mu, double t);

int main(int argc, char *argv[]){
    double mu, t, a, ans;
    
    if (argc != 5){
        fprintf (stderr, "usage is: %s mu t low_a high_a\n", argv[0]);
        exit (1);
    }

    mu = atof (argv[1]);
    t = atof (argv[2]);

    for (a = atoi (argv[3]); a <= atoi (argv[4]); a++){
        ans = integrate (mu - (a*t),mu + (a*t), 10, mu, t);
        fprintf (stderr, "%f    %lf\n", a, ans);
    }
    
}

double integrate (int start, int end, int n, double mu, double t){
    double delta, i;
    double sum = 0;

    delta = (double)(end-start)/n;
    
    for (i = 0; i < n; i++){
        sum += func (mu, t, (double)start + i*delta);
        sum += func (mu, t, (double)start + (i+1)*delta);
    }
    
    return sum * delta/2;
}

double func (double mu, double t, double x){
    double pi = 3.1415926;
    double e = 2.71828;
    return (1/(t * sqrt(2*pi)))*pow (e, -.5*((x-mu)/t)*((x-mu)/t));
}

