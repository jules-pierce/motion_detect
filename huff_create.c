//Huffman code designer written by Jules Pierce, Spring 2016
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct node_t{
    int symbol;
    double probability;
    int up_position;
    int bit;
}node_t;

void print_prob_info (node_t** listptrs, int n, int pctrl);

int main(int argc, char *argv[] ){
    node_t** listptrs;
    int n; //n = number of symbols
    int i, j, l, a, b, h;
    FILE *in_d;
    FILE *out_d;
    char c;
    double k;
    char temp[30];
    
    //assumes that huff_weights.txt is sorted from biggest to smallest by the probability
    if (argc != 4){
        fprintf (stderr, "usage is: %s num_symbols huff_weights.txt output.txt\n", argv [0]);
        exit (0);
    }
    
    n = atoi (argv[1]);
    
    in_d = fopen (argv[2], "r");
    if (in_d == NULL) printf ("unable to open file\n");
    //in_d is the file with the characters and their weights, separated by a space
    //each character has a new line
    
    listptrs = (node_t**) malloc (n*sizeof (node_t*));
    
    for (i = 0; i < n; i++){
        listptrs[i] = (node_t*) malloc ((n-i)*sizeof(node_t));
    }
    
    for (i = 0; i < n; i++){
        fscanf(in_d, "%d %lf", &listptrs[0][i].symbol, &listptrs[0][i].probability);
        listptrs[0][i].up_position = -1;
        listptrs[0][i].bit = -1;
    }
    
    for (i = 0; i < n-1; i++){
        for (j = 0; j < (n-i-1); j++){
            listptrs[i][j].bit = -1;;
        }
    }
    
    //fprintf (stderr, "lc 1\n");
    //print_prob_info(listptrs, n, 0);
    
    
    //first iteration in the document
    //outer loop goes through the list of lists, inner loops through the individual lists
    for (i = 0; i < n-1; i++){
        k = listptrs[i][n-i-1].probability + listptrs[i][n-i-2].probability;
        //fprintf (stderr, "lc 2, i = %d, k = %f\n", i, k);
        //print_prob_info(listptrs, n, 0);
        a = 0; //tracks whether or not the new structure has been put in
        for (j = 0; j < (n-i-1); j++){
            //fprintf (stderr, "lc 2a, i = %d, j = %d, k = %f\n", i, j, k);
            //print_prob_info(listptrs, n, 0);
            if (listptrs[i][j].probability >= k){
                listptrs[i+1][j] = listptrs[i][j];
                listptrs[i][j].up_position = j;
            }
            else if (a==0){
                listptrs[i+1][j].symbol = -1;
                listptrs[i+1][j].probability = k;
                listptrs[i+1][j].bit = -1;
                //fprintf (stderr, "**listptrs[2][0] = %f\n", listptrs[2][0].probability);
                listptrs[i][n-i-1].up_position = j;
                listptrs[i][n-i-2].up_position = j;
                listptrs[i][n-i-1].bit = 0;
                listptrs[i][n-i-2].bit = 1;
                //print_prob_info(listptrs, n, 1);
                //fprintf (stderr, "***listptrs[2][0] = %f\n", listptrs[2][0].probability);
                listptrs[i+1][j].up_position = -1;
                //listptrs[i+1][j].bit = -1;
                a = 1;
                //fprintf (stderr, "lc 2b, i = %d, j = %d, k = %f\n", i, j, k);
                //fprintf (stderr, "****listptrs[2][0] = %f\n", listptrs[2][0].probability);
                //print_prob_info(listptrs, n, 0);

            }
            else {
                listptrs[i+1][j] = listptrs [i][j-1];
                listptrs[i][j-1].up_position = j;
            }
            //fprintf (stderr, "lc 3, i = %d, j = %d, k = %f\n", i, j, k);
            //print_prob_info(listptrs, n, 0);
        }
        //fprintf (stderr, "lc 4\n");
        //print_prob_info(listptrs, n, 0);
    }
    print_prob_info(listptrs, n, 1);
    

    
    //get the codes into temp array (backwards) then copy them out of temp backwards
    //fprintf (stderr, "here\n");
    for (i = 0; i < n; i++){
        b = 0;
        l = 0;
        for (j = 0; j < n-1; j++){
            if (j==0){
                if (listptrs[j][i].bit != -1){
                    temp [b] = listptrs[j][i].bit;
                    b++;
                }
                l = listptrs[j][i].up_position;
            }
            else {
                if (listptrs[j][l].bit != -1){
                    temp [b] = listptrs [j][l].bit;
                    b ++;
                }
                l = listptrs [j][l].up_position;
            }
            //fprintf (stderr, "here2\n");
        }
        fprintf (stderr, "%d ", listptrs[0][i].symbol);
        //fprintf (stderr, "here3\n");
        for (h = b-1; h >= 0; h--){
            fprintf (stderr, "%d ", temp[h]);
        }
        fprintf (stderr, "2\n");
    }
    fclose (in_d);
    
}

void print_prob_info (node_t** listptrs, int n, int pctrl){
    int i, j;
    char dummystr[2];
    
    if (pctrl == 0) return;
    
    for (i = 0; i < n; i++){
        for (j = 0; j < (n-i); j++){
            fprintf (stderr, "%d, ", listptrs[i][j].bit);
        }
        fprintf (stderr, "\n");
    }
    //fprintf (stderr, "enter a character to continue: \n");
    //scanf ("%s", dummystr);
}




