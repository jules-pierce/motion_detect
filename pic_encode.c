#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

typedef struct node_t{
    node_t* child0;
    node_t* child1;
    char node_value;
    int terminal_node;
    //terminal_node == 1 if true, 0 if false
    int symbol;
} node_t;

void huffman_encoder(int symbol_to_encode, int &length, uint32_t &codeword, char* code_file);
int pushbit(uint8_t bit_to_write, char* file2write);
uint8_t popbit(char* file2read);
int huffman_decoder(uint32_t nextbit, char* code_file);
node_t* make_node ();
int find_length (int symbol, unsigned char cw[15][80]);

int main(int argc, char** argv){
    uint8_t symbol;
    int i, j;
    int length;
    uint32_t codeword;
    int retval;
    
    if (argc != 2){
        fprintf (stderr, "usage is: %s code_file.txt\n", argv [0]);
        exit (0);
    }

    
    
    for (i = 0; i < 5; i++){
        symbol = (i % 5) + 1;
        huffman_encoder (symbol, length, codeword, argv[1]);
        fprintf (stderr, "symbol_to_encode = %d    length = %d   codeword = %u\n", symbol, length, codeword);
        for (j = 0; j < length; j++){
            //pushbit();
        }
    }
    
    //next steps: test pushbit and popbit to and from a file with decoding
    //test with a larger code- at least 10 elements
    //make sure it would be possible to use a code larger than 255
    
    
    
    
}

int find_length (int symbol, unsigned char cw[15][80]){
    int j;
    
    j = 0;
    
    while (cw[symbol-1][j] != 2){
        j++;
    }
    return j;
}

//encodes symbol_to_encode into codeword, puts the length of codeword into length
//code_file has each symbol and it's corresponding code
void huffman_encoder(int symbol_to_encode, int &length, uint32_t &codeword, char* code_file){
	static int firstCall = 1;
    static unsigned char symbols[15];
    static unsigned char cw [15][80];
    char c, d, e;
    FILE *in_d;
    FILE *out_d;
    int i, j, k;
    int symbol_val;

	length = 0;
    codeword = 0;
    
    //fprintf (stderr, "%s:symbol_to_encode = %d\n", __FUNCTION__, symbol_to_encode);
    
    if( firstCall ){
		firstCall = 0;
		// Build tree
        in_d = fopen (code_file, "r");
        //fprintf (stderr, "%s\n", code_file);
        if (in_d == NULL) {
            printf ("unable to open file\n");
            exit (0);
        }
        
        i = 0;
        
       while ((c = fgetc (in_d)) != EOF){
            //fputc (c, stderr);
            if (c == 10){
                c = fgetc (in_d);
                if (c == EOF) break;
            }
            
            symbols[i] = c-48;
            j = 0;
            
            while ((d = fgetc (in_d)) != 10){
                //fputc (d, stderr);
                if (d != 32){
                    cw [i][j] = d-48;
                    j++;
                }
                //fprintf (stderr, "made it to here 0\n");
            }
            i++;
        }
        
        fclose (in_d);
    }

    
    length = find_length (symbol_to_encode, cw);
    //fprintf (stderr, "symbol_to_encode = %d   codeword length = %d\n", symbol_to_encode, length);
    
    codeword = 0;
    
    for (k = 0; k < length; k++){
        if (cw[symbol_to_encode-1][k] == 1){
            //fprintf (stderr, "k = %d\n", k);
            codeword = (1 << (length-1-k)) | (codeword);
        }
     }
    return;

}

int huffman_decoder(uint32_t nextbit, char* code_file){
	static int firstCall = 1;
	//static int arrayx[100]; // the tree variables
    int symbol, bit;
    FILE *in_d;
    static node_t* current_node;
    static node_t* start_node;
    int r;
    char symbols [100];
    int i, j;
    char bits[80];
    int code_symbol;
    int num_bits;
    int retval;

	if( firstCall ){
		firstCall = 0;
		// Build tree
        start_node = make_node();
        in_d = fopen (code_file, "r");
        if (in_d == NULL) printf ("unable to open file\n");
        
        //fprintf (stderr, "made it to here\n");
        
        while (1){
            retval = fscanf (in_d, "%d", &code_symbol);
            if (retval != 1) break;
            current_node = start_node;
            while (1){
                fscanf (in_d, "%d", &bit);
                if (bit == 2) {
                    current_node->terminal_node = 1;
                    current_node->symbol = code_symbol;
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
            
            //fprintf (stderr, "made it to here 1\n");
            
        }
        current_node = start_node;
        fclose (in_d);
	}
    
    
    if (nextbit == 0){
        current_node = current_node->child0;
        //fprintf (stderr, "branched 0\n");
    }
    else {
        current_node = current_node->child1;
        //fprintf (stderr, "branched 1\n");
    }
    
    if (current_node->terminal_node == 1){
        retval = current_node->symbol;
        current_node = start_node;
        return retval;
    }
    
    //fprintf (stderr, "made it to here 2\n");
    
    return -1;
	// use the bit to descend the tree.  If at a terminal node, then
	// return the symbol value, otherwise return -1
}

node_t* make_node (){
    node_t* temp;
    temp = (node_t*)malloc (sizeof (node_t));
    temp -> child0 = NULL;
    temp -> child1 = NULL;
    temp -> terminal_node = 0;
    return temp;
}

// This will return a 1 or a 0.  It will return -1 when it reaches the end of 
// the file
uint8_t popbit(char* file2read){
    static int firstCall = 1;
	static FILE *fd;
	static uint8_t curr_byte;
	static int bit_pos;
    int r;
    uint8_t z;

	if( firstCall ){
		fd = fopen(file2read, "rb");
		firstCall = 0;
		bit_pos = 0;
		// read first byte into curr_byte
		r = fread(&curr_byte, 1, 1, fd);
        if (r != 1) return -1;
	}
    
    if (((1<<(bit_pos)) & curr_byte) == 0) z = 0;
    //does this line return the right value?
    else z = 1;
    bit_pos++;
    
    if (bit_pos == 8){
        bit_pos = 0;
        r = fread (&curr_byte, 1, 1, fd);
        if (r != 1) return -1;
    }
    
    
    return z;

	// does bit_pos == 8?  if it does, read another byte and reset
	// bit_pos to 0, otherwise return the next bit in curr_byte, increment
	// bit_pos, then return the bit
}

// This will return a 1 or a 0.  It will return -1 when it reaches the end of 
// the file
int pushbit(uint8_t bit_to_write, char* file2write, int force_write){
    static int firstCall = 1;
	static FILE *fd;
	static uint8_t curr_byte = 0;
	static int bit_pos;
    
    //add parameter to signal end of symbols to encode, shift curr_byte to the top, write it out, and close the file

    if (force_write == 1){
        fwrite (&curr_byte, 1, 1, fd);
        fclose (fd);
    }
    
    if( firstCall ){
		fd = fopen(file2write, "wb");
		firstCall = 0;
		bit_pos = 0;
	}
    
    curr_byte = (bit_to_write << bit_pos) | (curr_byte);
    bit_pos ++;
    
    if (bit_pos == 8){
        fwrite (&curr_byte, 1, 1, fd);
        bit_pos = 0;
        curr_byte = 0;
    }
    
    //*****what do we want the return value to do?******
    return 1;

  //put bit into position bit_pos, increment bit_pos, if bit_pos == 8, output
	//the byte to the file with fwrite() and reset bit_pos to 0 and reset
	//curr_byte to 0
}
