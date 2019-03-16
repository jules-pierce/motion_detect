#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


int main(int argc, char *argv[] ){
    unsigned char symbols[15];
    char cw [15][80];
    char c, d, e;
    FILE *in_d;
    FILE *out_d;
    int i, j, k;
    
    if (argc != 4){
        fprintf (stderr, "usage is: %s code_file.txt message.txt output.txt\n", argv [0]);
        exit (0);
    }
    
    in_d = fopen (argv [1], "r");
    if (in_d == NULL) printf ("unable to open file\n");
    
    i = 0;

    while ((c = fgetc (in_d)) != EOF){
        if (c == 10){
            c = fgetc (in_d);
            if (c == EOF) break;
        }
        
        symbols[i] = c;
        j = 0;

        while ((d = fgetc (in_d)) != 50){
            cw [i][j] = d;
            j++;
        }
        i++;
    }
    
    fclose (in_d);
    
    in_d = fopen (argv [2], "r");
    if (in_d == NULL) printf ("unable to open file\n");
    
    out_d = fopen (argv[3], "w");
    
    while ((c = fgetc (in_d)) != EOF){
        for (i = 0; i < 15; i++){
            if (symbols [i] == c){
                for (k = 0; k < strlen(cw[i]); k++){
                    //printf ("putting %d\n", cw [i][k]);
                    fputc (cw [i][k], out_d);
                }
            }
        }
    }
    
    fclose (in_d);
    fclose (out_d);
    
}



