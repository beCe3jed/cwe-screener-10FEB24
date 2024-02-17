// https://inst.eecs.berkeley.edu/~cs152/sp23/assets/homeworks/hw02.pdf
// https://www.youtube.com/watch?v=B-EMkzv2AHE&t=4s
// https://slideplayer.com/slide/13412556/
// https://parisc.wiki.kernel.org/images-parisc/2/20/Pa7200_design.pdf

#include <stdlib.h>
#include <stdio.h>
#include<string.h>

#define ONE_KILOBYTE 1024
#define CACHE_LINE_SIZE 64
#define CACHE_LINE_MAP_BITS 6
#define ADDRESS_MAP_BITS 20
#define DELMITITER '\n'

#define L1_CACHE_LINES 32
#define L1_TAG_BIT_SHIFT 11
#define L1_LINE_BIT_SHIFT 5

#define L2_CACHE_LINES 256
#define L2_TAG_BIT_SHIFT 14
#define L2_LINE_BIT_SHIFT 8

#define V_CACHE_LINES 8
#define V_TAG_BIT_SHIFT 9
#define V_LINE_BIT_SHIFT 3


struct Primary_Cache{ // primary cache. Each miss will cause an eviction. Evicts to Victim. All cache Hits return to L1. 
    unsigned char cache_line_32[L1_CACHE_LINES][CACHE_LINE_SIZE]; //
    unsigned short TAG[L1_CACHE_LINES];
};
struct Secondary_Cache{ // Secondary cache. Larger. Recieves from Victim cache, dispels all evictions to the abyss
    unsigned char cache_line_256[L2_CACHE_LINES][CACHE_LINE_SIZE];
    unsigned short TAG[L2_CACHE_LINES];
};
struct Assist_Cache{ // Assist cache. Fully-Associative? FIFO. All evictions to L2.
    unsigned char cache_line_8[V_CACHE_LINES][CACHE_LINE_SIZE];
    unsigned short TAG[V_CACHE_LINES];
};

// declare the cache objects
struct Primary_Cache L1_Cache;
struct Secondary_Cache L2_Cache;
struct Assist_Cache Victim_Cache;
unsigned char cache_line_data = {64};
FILE *memory = NULL;

// functions for lookups
void lookInL1(unsigned long address);
void lookInL2(unsigned long address, unsigned long victim_cache_line);
void lookInVictim(unsigned long address, unsigned long victim_cache_line);
void readFromMemory(unsigned long address);
void writeIntoL1(unsigned long address);

//tools for hashing/lookups
unsigned char getWordNumber(unsigned long address, unsigned num_bytes);
unsigned long getCacheLineStart(unsigned long address, unsigned char num_bytes);
unsigned char getLineNumber(unsigned long address, unsigned short num_lines);
unsigned short getTag(unsigned long address, unsigned char bit_shift);
unsigned long unhashIntoCacheLine(unsigned short tag, unsigned char line_num, unsigned char tag_shift);


int main (void) {

    // intiate the caches
    for (int i = 0; i < L1_CACHE_LINES; i++){
        for (int j = 0; j < CACHE_LINE_SIZE; j++){
            L1_Cache.cache_line_32[i][j] = 0;
        }
        L1_Cache.TAG[i] = 0;
    }
    for (int i = 0; i < L2_CACHE_LINES; i++){
        for (int j = 0; j < CACHE_LINE_SIZE; j++){
            L2_Cache.cache_line_256[i][j] = 0;
        }
        L2_Cache.TAG[i] = 0;
    }
    for (int i = 0; i < V_CACHE_LINES; i++){
        for (int j = 0; j <CACHE_LINE_SIZE; j++){
            Victim_Cache.cache_line_8[i][j] = 0;
        }
        Victim_Cache.TAG[i] = 0;
    }


    // initialize data for reading the files
    FILE *reads_file = NULL;
    int num_bytes = 0, total_bytes = 0, multiplier = 1;
    unsigned char *old_address = NULL;
    unsigned char *buffer = (unsigned char*)malloc(sizeof(unsigned char) * (ONE_KILOBYTE + 1));
    unsigned char *reads_data = (unsigned char*)malloc(sizeof(unsigned char) * (ONE_KILOBYTE + 1));
    if (buffer == NULL)
    {
        printf("Error allocating space for buffer");
        return 1;
    }
    if (reads_data == NULL)
    {
        printf("Error allocating space for buffer");
        return 1;
    }
    if ((reads_file = fopen("reads.txt", "r")) == NULL){
        printf("Unable to open the reads.txt file");
        return 1;
    }
    if ((memory = fopen("memory.bin", "rb")) == NULL){
        printf("Unable to open the memory region");
        return 1;
    }


    // USES BUFFER to READ_DATA
    printf("Reading the file...\n");
    while ( (!feof(reads_file)) && ((num_bytes = fread(buffer, 1, ONE_KILOBYTE, reads_file)) > 0) ){
        // printf("num_bytes: %d, multiplier: %d\n", num_bytes, multiplier);
        memcpy(&reads_data[total_bytes], buffer, num_bytes); 
        total_bytes += num_bytes;
        // printf("total_bytes read: %d\n", total_bytes);
        // reads_data[total_bytes] = '\0';
        if (num_bytes == ONE_KILOBYTE)
        {
            multiplier++;
            old_address = reads_data;
            reads_data = (unsigned char*)realloc(reads_data, (sizeof(unsigned char) * ((ONE_KILOBYTE * multiplier) + 1)));
            if (reads_data == NULL){
                printf("Error reallocating reads_data");
                return 1;
            }
        }
    }
    if (buffer) free(buffer);
    if (reads_file) fclose(reads_file);

    // PREP TO TOKENIZE DATA
    const char delimiter = DELMITITER;
    unsigned long address = 0;
    unsigned char *token = (unsigned char*)malloc(sizeof(unsigned char) * 8);
    if (token == NULL){
        printf("error allocating token");
        exit(1);
    }

    // TOKENIZE THE DATA
    token = strtok(reads_data, &delimiter);
    address = strtoul(token, NULL, 16);
    printf("------------- Value number %d: 0x%X ----------\n", 1, strtoul(token, NULL, 16));
    lookInL1(address);
    // printf("Value number 1: 0x%X, word# 0x%X, cache start 64: 0x%X, line number (32): %d, TAG (bs11): 0x%X\n", address, getWordNumber(address, 64), getCacheLineStart(address, 64), getLineNumber(address, 32), getTag(address, 11)); 
    

    // TEST THE TOKENS
    for (int i = 0; i < 10; i++){
        token = strtok(NULL, &delimiter);
        printf("------------- Value number %d: 0x%X ----------\n", i + 2, strtoul(token, NULL, 16));
        lookInL1(strtoul(token, NULL, 16));
    }

    printf("Freeing memory...\n");
    if (reads_data) free(reads_data);
    if (memory) fclose(memory);
    return 0;
}


/*****************************************************************************************************
THESE ARE QUICK FUNCTIONS USED AS TOOLS TO BETTER READABILITY AND CONSISTENCY AMONG LOOK-UP FUNCTIONS
******************************************************************************************************/

// RETURNS THE NUMBER WORD IN THE CACHE LINE THAT WE INTEND TO READ -- MAY BE USELESS
//  - HOLDS UP TO 2^6 TO LOCATE WHICH OF THE 64 POSSIBLE LOCATIONS IN THE CACHE_LINE
unsigned char getWordNumber(unsigned long address, unsigned num_bytes){
    return (address % num_bytes);
}
// RETURNS AN ADDRESS OF WHICH THE CACHE_LINE STARTS
//  - CUTS OUT THE LOWER 6 BITS AS THEY ARE GOING TO BE REPRESENTED ELSEWHERE
unsigned long getCacheLineStart(unsigned long address, unsigned char num_bytes){
    return address - (address % num_bytes);
}
// RETURNS THE VALUE OF WHICH LINE THE ADDRESS WILL BE STORED AT
//  - VALUE UP TO 2^8 BITS DEPENDENT ON THE NUMBER OF CACHE LINES IN THE CACHE
unsigned char getLineNumber(unsigned long address, unsigned short num_lines){
    return ((address / CACHE_LINE_SIZE) % num_lines);
}
// RETURNS THE VALUE OF THE TAG THAT THE ADDRESS SHOULD HAVE
//  - SIZE IS DEPENDENT ON CACHE_LINE_SIZE AND NUM_CACHE_LINES HOLDS UP TO 2^11 BITS
unsigned short getTag(unsigned long address, unsigned char tag_shift){
    return (address >> tag_shift);
}
// TAKES AN ADDRESS FROM A CACHE, AND RETURNS THE CACHE_LINE START -- THIS MAY BE USELESS
//  - SIZE OF AN ADDRESS UP TO AN UNSIGNED LONG
unsigned long unhashIntoCacheLine(unsigned short tag, unsigned char line_num, unsigned char tag_shift){ 
    return ( (tag << (tag_shift)) + (line_num << (ADDRESS_MAP_BITS - tag_shift - CACHE_LINE_MAP_BITS)) );
}




/***************************************************************************************************
THE LOOK IN L1 FUNCTION RECIEVES AN ADDRESS, THEN IT CHECKS FOR A HIT OR MISS BY COMPARING THE TAGS
    - HIT:
        - WE HAVE A MATCH, NO PROCESS NECCESSARY
    - MISS:
        - COPY THE VICTIM CACHELINE = TAG << LINE_NUM
        - GRAB THE VICTIM CACHELINE DATA
        - LOOK IN VC FOR THE ORIGINAL ADDRESS, DISPLACING WHATEVER WAS THERE WITH THE VICTIM
****************************************************************************************************/
void lookInL1(unsigned long address){
    unsigned char last_byte = (address & 0xFF); // captures the last byte
    unsigned short address_tag = getTag(address, L1_TAG_BIT_SHIFT);
    unsigned char address_line_num = getLineNumber(address, L1_CACHE_LINES);
    printf("Address: 0x%x, tag: 0x%x, line_num: %d \n", address, address_tag, address_line_num);
    printf("Comparing 0x%x to L1 tag: 0x%x \n", address_tag, L1_Cache.TAG[address_line_num]);

    if (address_tag == L1_Cache.TAG[address_line_num]) //HIT!
    {
        printf("Hit IN L1!\n");
    }
    else  //MISS!
    {
        printf("Miss in L1\n");
        // 1. GRAB THE VICTIM CACHE LINE
        unsigned long victim_cache_line = unhashIntoCacheLine(address_tag, address_line_num, L1_TAG_BIT_SHIFT); 
        // 2. GRAB THE VICTIM BYTEs
        memcpy(&cache_line_data, L1_Cache.cache_line_32[address_line_num], CACHE_LINE_SIZE); // this may need to be played around with regard to dereferencing
        // 3. Look up in L2
        lookInVictim(address, victim_cache_line);
    }
}


/***************************************************************************************************
THE LOOK IN VC FUNCTION RECIEVES AN ADDRESS AND VICTIM, THEN IT CHECKS FOR A HIT OR MISS BY COMPARING THE TAGS
    - UPDATE VC TAG AND CACHE_LINE
    - HIT:
        - COPY VC DATA INTO GLOBAL 'cache_line_data'
        - WRITE EVICTION BACK INTO L1
    - MISS:
        - LOOK IN L2 FOR THE ORIGINAL ADDRESS, DISPLACING WHATEVER WAS THERE WITH THE VICTIM
****************************************************************************************************/
void lookInVictim(unsigned long address, unsigned long victim_cache_line){
    unsigned short address_tag = getTag(address, V_TAG_BIT_SHIFT);
    unsigned char address_line_num = getLineNumber(address, V_CACHE_LINES);

    printf("Address: 0x%x, tag: 0x%x, line_num: %d \n", address, address_tag, address_line_num);
    printf("Comparing 0x%x to VC tag: 0x%x \n", address_tag, Victim_Cache.TAG[address_line_num]);
    
    unsigned char *temp = Victim_Cache.cache_line_8[address_line_num];
    

    if (address_tag == Victim_Cache.TAG[address_line_num]) //HIT!
    {
        printf("hit IN VC!\n");
        // update VC tag and cache_line
        memcpy(&Victim_Cache.TAG[address_line_num], &victim_cache_line, 2);
        memcpy(Victim_Cache.cache_line_8[address_line_num], &cache_line_data, CACHE_LINE_SIZE); // CARRIED OVER FROM L1
        
        // copy cache_line data
        memcpy(&cache_line_data, temp, CACHE_LINE_SIZE);
        

        // write back into L1
        writeIntoL1(address);
    }
    else  //MISS!
    {   
        // update VC tag and cache_line
        Victim_Cache.TAG[address_line_num] = address_tag;
        memcpy(Victim_Cache.cache_line_8[address_line_num], &cache_line_data, CACHE_LINE_SIZE); // CARRIED OVER FROM L1
        printf("Miss in VC!\n");
        // Look up in L2
        lookInL2(address, victim_cache_line);
    }
}

/***************************************************************************************************
THE LOOK IN L2 FUNCTION RECIEVES AN ADDRESS AND VICTIM, THEN IT CHECKS FOR A HIT OR MISS BY COMPARING THE TAGS
    - UPDATE L2 TAG AND CACHE_LINE
    - HIT:
        - COPY L2 DATA INTO GLOBAL 'cache_line_data'
        - WRITE EVICTION BACK INTO L1
    - MISS:
        - READ FROM MEMORY
****************************************************************************************************/
void lookInL2(unsigned long address, unsigned long victim_cache_line){
    unsigned short address_tag = getTag(address, L2_TAG_BIT_SHIFT);
    unsigned char address_line_num = getLineNumber(address, L2_CACHE_LINES);

    printf("Address: 0x%x, tag: 0x%x, line_num: %d \n", address, address_tag, address_line_num);
    printf("Comparing 0x%x to L2 tag: 0x%x \n", address_tag, L2_Cache.TAG[address_line_num]);
    
    unsigned char *temp = L2_Cache.cache_line_256[address_line_num];


    if (address_tag == L2_Cache.TAG[address_line_num]) //HIT!
    {
        printf("Match IN L2!\n");
        // update L2 tag and cache_line with the victim
        memcpy(&L2_Cache.TAG[address_line_num], &victim_cache_line, 2);
        memcpy(L2_Cache.cache_line_256[address_line_num], &cache_line_data, CACHE_LINE_SIZE); // CARRIED OVER FROM L1

        // copy cache_line data
        memcpy(&cache_line_data, L2_Cache.cache_line_256[address_line_num], CACHE_LINE_SIZE); // update global
        
        // write back into L1
        writeIntoL1(address);
    }
    else  //MISS!
    {
        printf("Miss in L2\n");

        // update L2 tag and cache_line
        L2_Cache.TAG[address_line_num] = address_tag;
        memcpy(L2_Cache.cache_line_256[address_line_num], &cache_line_data, CACHE_LINE_SIZE); // CARRIED OVER FROM L1

        // Read from memory
        readFromMemory(address);
    }
}



/***************************************************************************************************
READS DATA FROM MEMORY BY USING AN OFFSET TO THE START OF THE CACHE_LINE IN ORDER TO COPY 64 BYTES
    - CHECKS IF THE MEMORY REGIONS ACCESS IS SUCCESSFUL
    - CHECKS IF THE CACHE_LINE HAD A SUCCESSFUL READ
    - COPIES 64 BYTES OF DATA INTO GLOBAL "cache_line_data" 
****************************************************************************************************/
void readFromMemory(unsigned long address){
    unsigned long cache_line_start = getCacheLineStart(address, CACHE_LINE_SIZE); // This gets the data at the specific page

    int num_bytes = 0;
    unsigned char *buffer = (unsigned char*)malloc(sizeof(unsigned char) * (CACHE_LINE_SIZE) ); // could be a static variable
    if (buffer == NULL)
    {
        printf("Error allocating space for buffer");
        exit(1);
    }

    if (fseek(memory, cache_line_start, SEEK_SET) != -1) // successful access to memory region
    { 
        if ((num_bytes = fread(buffer, 1, CACHE_LINE_SIZE, memory)) > 0)
        {
           memcpy(&cache_line_data, buffer, CACHE_LINE_SIZE); // what if there is not 
        }
        else  // if cache_line is not read
        {
            printf("Cache_line read error!");
            exit(1);
        }
    }
    else // if memory access is unsuccesful
    {
        printf("Unsuccessful Memory access");
        exit(1);
    }
    if (buffer) free(buffer);

    writeIntoL1(address);
}

/***************************************************************************************************
WRITES A NEW TAG AND A CACHE_LINE BACK INTO L1 - CALLED FROM 'readFromMemory()', 'lookInVictim()', 'lookInL2()' 
    - USES THE ADDRESS THAT WAS PASSED THROUGH ALL OF THE CACHES
        - GETS TAG AND LINE NUM
    - WRITES 64 BYTES OF DATA USING MEMCPY()
****************************************************************************************************/
void writeIntoL1(unsigned long address){
    printf("Writing to L1...\n");
    // get tag & line_num from the address
    unsigned short new_tag = getTag(address, L1_TAG_BIT_SHIFT);
    unsigned short new_line_num = getLineNumber(address, L1_CACHE_LINES);
    
    // find line and overwrite the tag and the 64 bytes of data 
    L1_Cache.TAG[new_line_num] = new_tag;
    memcpy(L1_Cache.cache_line_32[new_line_num], &cache_line_data, CACHE_LINE_SIZE);
    // print for debugging
    printf("TAG %X ", L1_Cache.TAG[new_line_num]);
    for (int i = 0; i < CACHE_LINE_SIZE; i++){
        printf(" %02x", L1_Cache.cache_line_32[new_line_num][i]);
    }
    printf("\n");
}