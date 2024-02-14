#include <stdio.h>
#include <stdlib.h>
#include <string.h>

short hash(unsigned char * s){

    unsigned short h = 0;
    unsigned int i = 0;
    unsigned char *letter_address = s;
    
    while(*letter_address != 0){ // check if == '\0'
         void *copy_address = (void*)letter_address;
        if((i % 2) == 0) {
            // if i is even 
            unsigned short multiply = *(unsigned short*)copy_address;
            multiply *= 6406;
            h = h ^ multiply;
        }
        else {
            // if odd
            unsigned char copy_letter = *(unsigned char*)copy_address;
            copy_letter = (unsigned char)(((copy_letter * 8) + copy_letter) * 2);
            h = h ^ (unsigned short)copy_letter;
        }
        // printf("iteration #%d, letter: %c, val: %d\n", i, *letter_address, h);
        i++;
        letter_address++;
    }
    return h;

}

int main(int argc, char * argv[]){

    for(int i = 1;i<argc;i++)
        printf("hash(\"%s\")=0x%04hx\n",argv[1], hash(argv[i])); 
}
