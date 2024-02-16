#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"


//https://en.wikipedia.org/wiki/Interpreter_(computing)#:~:text=An%20interpreter%20usually%20consists%20of,how%20to%20mutate%20the%20data.

#define BUFFER_SIZE 1024


short bytesToShort(unsigned char byte1, unsigned char byte2);
unsigned int bytesToInt(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4);
long long bytesToLLong(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4, unsigned char byte5, unsigned char byte6, unsigned char byte7, unsigned char byte8);


int main (void) {
    //initiate variables for reading in the file
    FILE *fp = NULL;
    unsigned char *old_address = NULL;
    unsigned char *buffer = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
    unsigned char *dump = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
    if (buffer == NULL || dump == NULL){
        printf("Error allocating the buffer/dump");
        exit(1);
    }
    int buffer_multiplier = 1;
    int numBytes = 0;
    int total_bytes = 0;

    // read in the file via the buffer
    if((fp = fopen(FILE_NAME, "rb")) == NULL){
        printf("Error opening the file");
        exit(2);
    }
    while ( (feof(fp) == 0) && ((numBytes = fread(buffer, 1, BUFFER_SIZE, fp)) != 0) ){
        memcpy(&dump[total_bytes], buffer, numBytes);
        total_bytes += numBytes;
        if (numBytes == BUFFER_SIZE){
            old_address = dump;
            dump = (unsigned char*)realloc(dump, sizeof(unsigned char) * (++buffer_multiplier));
            if (old_address != dump) free(old_address);
        }
    }
    // clean up the buffer and dump
    old_address = dump;
    dump = (unsigned char*)realloc(dump, sizeof(unsigned char) * (total_bytes));
    if (old_address != dump) free(old_address);
    if(fp) fclose(fp);
    if(buffer) free(buffer);
    // print what we have from the file
    for (int i = 0; i < total_bytes; i++){
        printf(" %02x", dump[i]);
    }
    printf("\nTotal Bytes: %d \n\n", total_bytes);



    // initiate variables for our interpreter
    size_t index = 0, counter = 0;
    short offset = 0;
    unsigned int target = 0, temp = 0, target2 = 0, sum = 0;
    unsigned int adding = 0, num = 0;
    unsigned char OPCODE = 0;
    unsigned char *destination_index_ptr = NULL;
    unsigned char *parameter_ptr = NULL;

    printf("--- STARTING LOOP ---\n");
    // create a loop 
    while ((OPCODE = dump[index]) != END){
        // printf("STEP %d:\nIndex at %02x with OPCODE: %02x\n", counter++, index, OPCODE);
        // CREATE A SWITCH CASE
        switch(OPCODE) {
            case JMP: // grab two bytes, and move instruction ptr
                // printf("Execute JMP\n");
                offset = bytesToShort(dump[index + 1], dump[index + 2]);
                // printf("Offset is: %d\n", offset);
                index += offset;
                // printf("Index is now = %02x\n\n", index);
                break;

            case SWP:
                // printf("Execute SWP\n");
                target = dump[++index];
                target2 = dump[++index];
                // printf("Target is: %02x\n", target);
                // printf("Target2 is: %02x\n", target2);
                // printf("%02x and %02x !SWP!", dump[target], dump[target2]);
                temp = dump[target2];
                dump[target2] = dump[target];
                dump[target] = temp;
                // printf(" %02x and %02x\n", dump[target], dump[target2]);
                ++index;
                // printf("Index is now = %02x\n\n", index);
                break;

            case ADD:
                // printf("-----ADD------\n");
                target = dump[++index];
                // printf("Target is: %02x\n", target);
                index++;
                adding = bytesToInt(dump[index], dump[index + 1], dump[index + 2], dump[index + 3]);
                // printf("Add %08x\n", adding);
                if (1) { // This orders the target bytes into an integer from little endian
                    num = bytesToInt(dump[target], dump[target + 1], dump[target + 2], dump[target + 3]);
                    // printf("Unto 0x%08x\n", num);
                }
                if (0) { // This adds directly to the bytes without converting target from little endian
                    num = bytesToInt(dump[target + 3], dump[target + 2], dump[target + 1], dump[target]);
                    // printf("Unto 0x%08x   -- This is adding directly to the bytes\n", num);
                }

                sum = adding + num;
                // printf("Sum = %08x\n", sum);

                dump[target] = sum & 0xFF;
                dump[target + 1] = (sum >> (1*8)) & 0xFF;
                dump[target + 2] = (sum >> (2*8)) & 0xFF;
                dump[target + 3] = (sum >> (3*8)) & 0xFF;
                // printf("Dump[%02x] = %02x %02x %02x %02x\n", target, dump[target], dump[target+1], dump[target+2], dump[target+3]);

                index += 4;
                // printf("Index is now = %02x\n\n", index);
                break;

            case XOR:
                // printf("Execute XOR\n");
                target = dump[++index];
                // printf("Target is: %02x\n", target);
                ++index;
                for(int i = 0; i < (XOR_SIZE - 2); i++){
                    // printf("%02x XOR %02x... ", dump[target], dump[index]);
                    dump[target] = dump[target] ^ dump[index++];
                    // printf("Result: %02x\n", dump[target]);
                    target++;
                }
                // printf("Index is now = %02x\n\n", index);
                break;

            case INVERT:
                // printf("Execute INVERT\n");
                target = dump[++index];
                // printf("Target is: %02x\n", target);
                if (0) {
                    // printf("%02x XOR 0xff... ", dump[target]);
                    dump[target] = dump[target] ^ 0xFF;
                }
                if (1) {
                    // printf("Reversing the order of the bits from %02x...\n", dump[target]);
                    temp = ((dump[target] & 0x1) << 7) + 
                    ((dump[target] & 0x2) << 5) + 
                    ((dump[target] & 0x4) << 3) + 
                    ((dump[target] & 0x8) << 1) + 
                    ((dump[target] & 0x10) >> 1) + 
                    ((dump[target] & 0x20) >> 3) + 
                    ((dump[target] & 0x40) >> 5) + 
                    ((dump[target] & 0x80) >> 7);
                    dump[target] = temp;
                }
                
                // printf("Result: %02x\n", dump[target]);
                ++index;
                // printf("Index is now = %02x\n\n", index);
                break;

            case PRINT:
                // printf("Execute PRINT\n");
                printf("%c", dump[index + 1]);
                index += 2;
                // printf("Index is now = %02x\n\n", index);
                break;

            default: //ERROR!
                printf("We are at default....?");
                exit(3);
        }
    }
    printf("\nExecute END w/OPCODE: %d at index %d\n", OPCODE, index);
        
            


    
    if (dump) free(dump);
    return 0;
}



short bytesToShort(unsigned char byte1, unsigned char byte2) {
    return (int)byte1 + ((int)byte2 << (1 * 8));
}
unsigned int bytesToInt(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4){
    // printf("Converting into...... %02x %02x %02x %02x\n", byte4, byte3, byte2, byte1);
    return (int)byte1 + ((int)byte2 << (1 * 8)) + ((int)byte3 << (2 * 8)) + ((int)byte4 << (3 * 8));
}