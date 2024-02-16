/*
AUTHOR: beCe3jed

THE PURPOSE OF THIS CODE IS TO BUILD AN INTERPRETER THAT HANDLES BASIC FUNCTIONALITY BY READING IN A .bin FILE. INCLUDED FILES ARE:
    - "interpreter.h": 
        - HEADER FILE CONTAINING MULTIPLE MACROS, OPCODES, ERRORCODES ETC.
    - "input_stream.bin"
        - OUR INPUT FILE THAT PROVIDES THE DATA TO BE INTERPRETED

Reference:
//https://en.wikipedia.org/wiki/Interpreter_(computing)#:~:text=An%20interpreter%20usually%20consists%20of,how%20to%20mutate%20the%20data
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "interpreter.h" //include this header file to simplify readability



/******************************************************************************************************************************
* THESE THREE FUNCTIONS ARE QUICK TOOLS USED TO SIMPLIFY AND ORGANIZE A LOT OF THE MATH THAT OCCURS IN THE PROGRAM TO INCREASE READABILITY.
*
* THE INTERPRETER WILL NEED TO PARSE BYTES INTO DIFFERENT UNIT TYPES SUCH AS SHORT, INT, AND LONG. THESE BYTES ARE GOING TO NEED TO BE
* CONVERTED FROM LITTLE ENDIAN INTO THERE RESPECTIVE TYPES. SINCE THIS WILL HAPPEN OFTEN, AND THE SHIFTING PROCESS IS INTUITIVE, WE CAN 
* EASILY SEPARATE THESE OPERATIONS INTO DIFFERENT FUNCTIONS AND CALL THEM APPROPRIATELY. 
* 
* EACH FUNCTIONS ACCEPTS AN APPROPRIATE AMOUNT OF BYTES FOR THEIR RESPECTIVE RETURN VALUE
******************************************************************************************************************************/
short bytesToShort(unsigned char byte1, unsigned char byte2);
unsigned int bytesToInt(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4);


/******************************************************************************************************************************
 * main(void)
 * 
 * THIS FUNCTION IS THE BULK OF PROGRAM. IT OPERATES IN 3 STEPS...
 * 1. READING IN FILE INTO MEMORY
 *      - OPENS FILE AND ALLOCATES A BUFFER THAT READS IN DATA 1024 BYTES AT A TIME
 *      - DATA GETS SAVED ONTO DUMP WHICH GROWS BY 1024 EACH ITERATION OF THE LOOP
 *      - FILE AND THE BUFFER ARE CLOSED AFTER THE LOOP
 * 2. INTERPRETER
 *      - USES A LOOP THAT WILL END ONCE AN OPCODE OF END IS GIVEN, OR IF AN INVALID OPCODE IS GIVEN, IN WHICH IT WILL EXIT
 *      - A SWITCH IS USED TO DETECT WHICH OPCODE IS GOING TO BE EXECUTED
 *      - REFERENCES OUR TOOL FUNCTIONS LISTED ABOVE FOR WHEN PARSING OF BYTES NEEDS TO HAPPEN
 *      * IMPORTANT TO NOTE THAT SINCE OUR FILE IS READING FROM A .BIN, BYTES WILL BE INTERPRETTED BETWEEN 0X0 & 0XFF INHERENTELY
 * 3. FREEING MEMORY
 *      - BEFORE RETURNING WE WILL FREE OUR DUMP AS IS STANDARD PRACTICE
 * 
 * ***************************************************************************************************************************/

int main (void) {

    /* SECTION 1: READING FILE INTO MEMORY */

    //initiate variables for reading in the file
    FILE *fp = NULL;
    unsigned char *buffer = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
    unsigned char *dump = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
    if (buffer == NULL || dump == NULL){
        printf("Error allocating the buffer/dump");
        exit(EXIT_ALLLOCATION_ERROR);
    }
    int buffer_multiplier = 1;
    int num_bytes = 0;
    int total_bytes = 0;

    // read in the file via the buffer
    printf("Reading in '%s'...", FILE_NAME);
    if((fp = fopen(FILE_NAME, "rb")) == NULL){
        printf("Error opening the file");
        exit(EXIT_FILE_ERROR);
    }
    while ( (feof(fp) == 0) && ((num_bytes = fread(buffer, 1, BUFFER_SIZE, fp)) != 0) ){
        memcpy(&dump[total_bytes], buffer, num_bytes);
        total_bytes += num_bytes;
        if (num_bytes == BUFFER_SIZE){
            dump = (unsigned char*)realloc(dump, sizeof(unsigned char) * (++buffer_multiplier));
        }
    }
    printf("Success!\n");
    // clean up the buffer and dump
    dump = (unsigned char*)realloc(dump, sizeof(unsigned char) * (total_bytes));
    if(fp) fclose(fp);
    if(buffer) free(buffer);

    


    /* SECTION 2: INTERPRETTING */
    // ONLY ERRORS TO CATCH ARE:
    // - INVALID OPCODE
    // - INDEX OUT-OF-BOUNDS

    // initiate variables for our interpreter
    size_t index = 0, counter = 0; // will always be positive!
    short offset = 0;
    unsigned int target = 0, temp = 0, target2 = 0, sum = 0;
    unsigned char OPCODE = 0;
    unsigned char *destination_index_ptr = NULL;
    unsigned char *parameter_ptr = NULL;

    printf("--- STARTING LOOP ---\n\n");
    while ((OPCODE = dump[index]) != END){
        // CREATE A SWITCH CASE
        switch(OPCODE) {
            // Jump execution by the given byte offset.
            // | OPCODE | signed short offset | (3 bytes)
            case JMP: 
                offset = bytesToShort(dump[index + 1], dump[index + 2]);
                index += offset;
                if (index >= total_bytes){ // IF WE JMP OUTSIDE BOUNDS OF OUR UNSIGNED CHAR ARRAY
                    printf("JMP - INDEX outside of bounds. %d", index);
                    exit(EXIT_DATA_ERROR);
                }
                break;

            //Swap the bytes at given byte indexes.
            // | OPCODE | unsigned byte index | unsigned byte index | (3 bytes)
            case SWP: 
                target = dump[++index];
                target2 = dump[++index];
                temp = dump[target2];
                dump[target2] = dump[target];
                dump[target] = temp;
                ++index;
                if (index >= total_bytes){ // IF WE JMP OUTSIDE BOUNDS OF OUR UNSIGNED CHAR ARRAY
                    printf("SWP - INDEX outside of bounds. %d", index);
                    exit(EXIT_DATA_ERROR);
                }
                break;

            //Add a constant unsigned int to the 4 bytes at the given byte index. 
            // | OPCODE | unsigned byte index | unsigned int adding | (6 bytes)
            case ADD: 
                target = dump[++index];
                index++;
                // This orders the target bytes into an integer from little endian
                sum = bytesToInt(dump[index], dump[index + 1], dump[index + 2], dump[index + 3]) + bytesToInt(dump[target], dump[target + 1], dump[target + 2], dump[target + 3]);
                dump[target] = sum & 0xFF;
                dump[target + 1] = (sum >> (1*8)) & 0xFF;
                dump[target + 2] = (sum >> (2*8)) & 0xFF;
                dump[target + 3] = (sum >> (3*8)) & 0xFF;
                index += 4;
                if (index >= total_bytes){ // IF WE JMP OUTSIDE BOUNDS OF OUR UNSIGNED CHAR ARRAY
                    printf("ADD - INDEX outside of bounds. %d", index);
                    exit(EXIT_DATA_ERROR);
                }
                break;

            //Xor a constant long long to the 8 bytes at the given byte index.
            // | OPCODE | unsigned byte index | long long xoring | (10 bytes)
            case XOR:
                target = dump[++index];
                ++index;
                for(int i = 0; i < (XOR_SIZE - 2); i++){
                    dump[target] = dump[target] ^ dump[index++];
                    target++;
                }
                if (index >= total_bytes){ // IF WE JMP OUTSIDE BOUNDS OF OUR UNSIGNED CHAR ARRAY
                    printf("ADD - INDEX outside of bounds. %d", index);
                    exit(EXIT_DATA_ERROR);
                }
                break;

            // Swap order of bits in the byte at the given byte index.
            // | OPCODE | unsigned byte index | (2 bytes)
            case INVERT:
                target = dump[++index];
                // Reversing the order of the bits 
                temp = ((dump[target] & 0x1) << 7) + 
                ((dump[target] & 0x2) << 5) + 
                ((dump[target] & 0x4) << 3) + 
                ((dump[target] & 0x8) << 1) + 
                ((dump[target] & 0x10) >> 1) + 
                ((dump[target] & 0x20) >> 3) + 
                ((dump[target] & 0x40) >> 5) + 
                ((dump[target] & 0x80) >> 7);
                dump[target] = temp;
                ++index;
                if (index >= total_bytes){ // IF WE JMP OUTSIDE BOUNDS OF OUR UNSIGNED CHAR ARRAY
                    printf("INVERT - INDEX outside of bounds. %d", index);
                    exit(EXIT_DATA_ERROR);
                }
                break;

            // Print out the following ascii char. Will print out the flag, one character at a time
            // | OPCODE | ascii char | (2 bytes)
            case PRINT: 
                printf("%c", dump[index + 1]);
                index += 2;
                if (index >= total_bytes){ // IF WE JMP OUTSIDE BOUNDS OF OUR UNSIGNED CHAR ARRAY
                    printf("PRINT - INDEX outside of bounds. %d", index);
                    exit(EXIT_DATA_ERROR);
                }
                break;

            // If we get an unknown OPCODE, something went wrong, and we need to exit! 
            default: //ERROR!
                printf("We have an invalid OPCODE: 0x%02x\n", OPCODE);
                exit(EXIT_OPCODE_ERROR);
        }
    }
    printf("\n\nExecute END w/OPCODE: %d at index %d\n", OPCODE, index);


    /* SECTION 3: FREE MEMORY */

    if (dump) free(dump);
    return 0;
}


/**************************************************************
 * bytesToShort()
 * 
 * TAKES 2 BYTES AND SHIFTS THEM ACCORDING TO LITTLE ENDIAN
**************************************************************/
short bytesToShort(unsigned char byte1, unsigned char byte2) {
    return (int)byte1 + ((int)byte2 << (1 * 8));
}
/**************************************************************
 * bytesToInt()
 * 
 * TAKES 2 BYTES AND SHIFTS THEM ACCORDING TO LITTLE ENDIAN
**************************************************************/
unsigned int bytesToInt(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4){
    return (int)byte1 + ((int)byte2 << (1 * 8)) + ((int)byte3 << (2 * 8)) + ((int)byte4 << (3 * 8));
}
unsigned int bytesToInt(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4){
    // printf("Converting into...... %02x %02x %02x %02x\n", byte4, byte3, byte2, byte1);
    return (int)byte1 + ((int)byte2 << (1 * 8)) + ((int)byte3 << (2 * 8)) + ((int)byte4 << (3 * 8));
}
