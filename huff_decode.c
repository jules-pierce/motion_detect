#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct node_t{
    node_t* child0;
    node_t* child1;
    char node_value;
    int terminal_node;
    //terminal_node == 1 if true, 0 if false
} node_t;

node_t* make_node ();

int main(int argc, char *argv[] ){
    int symbol, bit;
    FILE *in_d;
    node_t* current_node;
    node_t* start_node;
    int r;
    char symbols [100];
    int i, j;
    char bits[80];
    char code_symbol;
    int num_bits;
    
    if (argc != 4){
        fprintf (stderr, "usage is: %s code_file.txt bits.txt symbols.txt\n", argv [0]);
        exit (0);
    }
    
    start_node = make_node();
    in_d = fopen (argv [1], "r");
    if (in_d == NULL) printf ("unable to open file\n");
    
    printf ("entering while loop\n");
    while (1){
        code_symbol = fgetc (in_d);
        if (code_symbol == EOF) break;
        current_node = start_node;
        while (1){
            fscanf (in_d, "%d", &bit);
            if (bit == 2) {
                current_node->terminal_node = 1;
                current_node->node_value = code_symbol;
                break;
            }
            else {
                if (bit == 1) {
                    if (current_node->child1 == NULL){
                        current_node->child1 = make_node();
                    }
                    current_node = current_node -> child1;
                }
                else {
                    if (current_node->child0 == NULL){
                        current_node->child0 = make_node();
                    }
                    current_node = current_node -> child0;
                }
            }
        }
        
    }
    printf ("end of while loop\n");
    fclose (in_d); //should in_d be in the parentheses or the filename?
    in_d = fopen (argv[2], "r");
    i = 0;
    while (1) {
        r = fscanf (in_d, "%d", &symbol);
        if (r != 1) break;
        bits [i] = symbol;
        i ++;
    }
    num_bits = i;
    current_node = start_node;
    
    j = 0;
    for (i = 0; i < num_bits; i++){
        bit = bits [i];
        if (bit == 0){
            current_node = current_node->child0;
        }
        else{
            current_node = current_node->child1;
        }
        if (current_node->terminal_node == 1){
            symbols [j] = current_node->node_value;
            j++;
            current_node = start_node;
        }
    }
    symbols[j] = '\0';
    fprintf (stderr, "answer is: %s\n", symbols);
    fclose (in_d);
    
}


node_t* make_node (){
    node_t* temp;
    temp = (node_t*)malloc (sizeof (node_t));
    temp -> child0 = NULL;
    temp -> child1 = NULL;
    temp -> terminal_node = 0;
    return temp;
}

