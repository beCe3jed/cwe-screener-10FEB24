/*********************************************************************************************************************************************************
 * THIS PROGRAM READS IN DATA FROM A FILE AND STORES THE DATA IN BLOCKS OF DATA_SIZE (15 BYTES). EACH OF THESE ARRAYS THEN GETS SORTED ACCORDING 
 * TO THE FIRST TWO BYTES. THE REST OF THE 13 BYTES LATER GET XOR'ED AND RESULT IN A FLAG.
 * 
 * 4 MAIN SECTIONS:
 * 1. READ INTO MEMORY
 *  - DATA GETS READ INTO A BUFFER AS CHUNKS OF ONE_KILOBYTE AND THEN COPIED INTO A MALLOCED LARGE DUMP ACCORDINGLY 
 * 2. LOAD INTO 2D ARRAY FORMAT
 *  - NOW WE WILL COPY DATA INTO AN 2D ARRAY FORMAT SO THAT WE CAN EASILY SORT IT 
 * 3. MANIPULATE DATA AND PRINT FLAG
 *  - WE WILL EXECUTE A MERGESORT IN WHICH ONLY THE FIRST TWO BYTES OF EACH SUBARRAY ARE BEING COMPARED, YET THE WHOLE SUBARRAY WILL BE MOVED AROUND AS A SINGLE INDEX
 * 4. FREE MEMORY
 *  - WE WILL FREE ALL ALLOCATED MEMORY IN ACCORDANCE WITH STANDARD PROCEDURE
 * 
 * REFERENCE:
 *  -  https://www.youtube.com/watch?v=dDjfaA9Q3n8 
 *  -  https://www.geeksforgeeks.org/c-program-to-read-contents-of-whole-file/
**********************************************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ONE_KILOBYTE 1024
#define DATA_SIZE 15

void merge_sort(unsigned char ** array, int length); 
void merge_sort_recursion(unsigned char **array, int left, int right);
void merge_sorted_arrays(unsigned char **array, int left, int mid, int right);
void print_array(unsigned char **array, int length);
void xor_pieces(unsigned char** array, int length);


int main(void) {

  /*  SECTION 1: READING TO MEMORY */
  FILE *fp = NULL;
  int num_bytes = 0;
  int total_bytes = 0;
  int end_of_dump = 0;
  int buffer_multiplier = 1;
  unsigned char* old_dump_address = NULL;

  char *buffer = (char*)malloc(sizeof(char)*ONE_KILOBYTE);
  unsigned char *dump = (unsigned char*)malloc(sizeof(unsigned char) * ONE_KILOBYTE + 1);
  if ((buffer == NULL) || (dump == NULL)){
    perror("Error allocating space.");
    exit(1);
  }
  if ((fp = fopen("input_stream.bin", "rb")) == NULL){
    perror("Error opening the file.");
    exit(1);
  }

  while( ((feof(fp) == 0) && ((num_bytes = fread(buffer, sizeof(char), (ONE_KILOBYTE), fp)) != 0)) ){
    total_bytes += num_bytes;
    if ( buffer_multiplier == 1) {
      dump = memcpy(dump, buffer, num_bytes);
      dump[total_bytes] = '\0';
      end_of_dump = total_bytes; // to keep track of the *LAST* null byte
    } else {
      memcpy(&dump[end_of_dump], buffer, num_bytes); // strncat() will look for any Null byte, we need it to look for the LAST one
      dump[total_bytes] = '\0';
      end_of_dump = total_bytes;
    }
    if (total_bytes == ((ONE_KILOBYTE)* buffer_multiplier)) { 
      old_dump_address = dump;
      dump = (char*)realloc(dump, ( (sizeof(char) * (ONE_KILOBYTE) * (++buffer_multiplier) ) + 1 ) ); // double in size each time
      if (old_dump_address != dump) free(old_dump_address);
    }
  }
  if (ferror(fp) != 0) {
    perror("Error reading the file.");
    exit(1);
  }
  if (buffer) free(buffer);
  if (fp) fclose(fp);


  /*  SECTION 2: LOADING INTO ARRAY FORMAT */
  // THIS IS TO TRANSFER THE BYTES FROM THE DUMP INTO THE ARRAY FORMAT
  int num_indices = total_bytes/DATA_SIZE; 
  unsigned char **array = (unsigned char**)malloc(sizeof(unsigned char*) * num_indices);
  if (array == NULL) {
    printf("Error allocating space for the double_pointer array");
    exit(2);
  }
  int row, byte;
  byte = 0;
  row = -1;
  for (byte = 0; byte < total_bytes; byte++) { //  byte by byte in dump

    if ((byte % DATA_SIZE) == 0) { // initializes each subarray and sets the null terminator
      array[++row] = (unsigned char*)malloc((sizeof(unsigned char) * (DATA_SIZE + 1)));
      if (array[row] == NULL){
        printf("Error allocating space for the individual strings of bytes.");
        exit(2);
      }
      array[row][DATA_SIZE] = '\0';
    }
    array[row][(byte % DATA_SIZE)] = dump[byte]; // copies the bytes from the dump approprately
  }


  /*  SECTION 3: DATA MANIPULATION */
  // print_array(array, num_indices);
  merge_sort(array, num_indices);
  // print_array(array, num_indices);
  xor_pieces(array, num_indices);



  /*  SECTION 4: FREEING MEMORY */
  for (int free_row = 0; free_row < row; free_row++) { //  byte by byte in dump
      if(array[free_row]) free(array[free_row]);
  }
  if (array) free(array);
  if (dump) free(dump);
  printf("Exit succesful!");
  return 0;
}

/**********************************************************************************************************************************************************
 * merge_sort()
 * 
 * SIMPLISTIC MERGESORT CALL FOR EASE OF READABILITY 
 * 
 * THIS FUNCTION STARTS THE SORTING ALGORITHM. BY HAVING ONLY THIS FUNCTION BEING CALLED, WE CAN SIMPLIFY THE 
 * INITIAL PARAMETERS THAT GET PASSED. CALCULATES THE LEFT, AND RIGHT INDICIES OF THE ARRAY
 * 
 * PARAMETERS: 
 *  - unsigned char **array is the srray to be sorted
 *  - int length is the length of array 
 * 
 * CALLS: merge_sort_recursion()
***********************************************************************************************************************************************************/
void merge_sort(unsigned char **array, int length){
  merge_sort_recursion(array, 0, length - 1);
}

/**********************************************************************************************************************************************************
 * merge_sort_recursion()
 * 
 * THE RECURSION PART OF MERGESORT, BREAKS THE ARRAY DOWN INTO ITS SINGULAR INDICES
 * 
 * THIS FUNCTION IS THE RECURSIVE PORTION OF THE ALGORITHM THAT CHECKS IF THE SIZE OF THE ARRAY IS GREATER THAN 1, AND CALCULATES MID FOR THE 
 * CORRESPONDING RECURSIVE CALLS, THEN FINALLY SPLITS THE ARRAY IN HALF UNTIL IT IS SMALLER THAN 1
 * 
 * PARAMETERS: 
 *  - unsigned char **array is the srray to be sorted
 *  - int left & right are both ends of the array
 * 
 * CALLS: merge_sort_recursion(), merge_sorted_arrays()
**********************************************************************************************************************************************************/
void merge_sort_recursion(unsigned char **array, int left, int right) {
  if (left < right) {
    int mid = left + (right - left)/2;
    merge_sort_recursion(array, left, mid);
    merge_sort_recursion(array, mid + 1, right);
    merge_sorted_arrays(array, left, mid, right);
  }
}

/**********************************************************************************************************************************************************
 * merge_sorted_arrays()
 * 
 * THE COMPARISON AND MERGENCE OF THE ARRAY
 * 
 * THIS FUNCTION IS WHERE EACH ARRAY NOW GETS MERGED WITH ITS PARTNER AND IS SORTED IN LINE AS THEY ARE PLACED IN THE NEWLY SORTED ARRAY.
 * COPIES OF ARRAYS ARE USED TO GREATLY SPEED UP SORTING AT THE EXPENSE OF MEMORY USAGE
 * 
 * PARAMETERS: 
 *  - unsigned char **array is the srray to be sorted
 *  - int beg, mid & end correspond to the different indices of the array. Note that mid index is left-preferred in the case of an even-index array
 * 
 * CALLS: no calls
**********************************************************************************************************************************************************/
void merge_sorted_arrays(unsigned char **array, int beg, int mid, int end) {
  // PREP VARIABLES
  int left_length = mid - beg + 1;
  int right_length = end - mid;

  unsigned char **left_array = (unsigned char**)malloc(sizeof(unsigned char*)*left_length);
  unsigned char **right_array = (unsigned char**)malloc(sizeof(unsigned char*)*right_length);
  if ((left_array == NULL) || (right_array == NULL)){
    printf("Error allocating sub-array");
    exit(1);
  }
  for (int i = 0; i < left_length; i++){
    left_array[i] = (unsigned char*)malloc(sizeof(unsigned char)*DATA_SIZE);
    if (left_array[i] == NULL) {
      printf("Error allocating left array");
      exit(1);
    }
    left_array[i] = memcpy(left_array[i], array[beg + i], DATA_SIZE);
  }
  for (int i = 0; i < right_length; i++){
    right_array[i] = (unsigned char*)malloc(sizeof(unsigned char)*DATA_SIZE);
    if (right_array[i] == NULL) {
      printf("Error allocating right array");
      exit(1);
    }
    right_array[i] = memcpy(right_array[i], array[mid + 1 + i], DATA_SIZE);
  }

   // INITIATE MERGE
  int left_val, right_val;
  for(int i = beg, l = 0, r = 0; i <= (end); i++){

    // DECLARE VALUES AND PARSE THEM FROM LITTLE ENDIAN
    if (l < left_length) left_val = (int)left_array[l][0] + ((int)left_array[l][1] << (1*8));
    if(r < right_length) right_val = (int)right_array[r][0] + ((int)right_array[r][1] << (1*8));

    // MAKE COMPARISONS
    if ( (l < left_length) && ((r >= right_length) || (left_val <= right_val)) ) { // copy left array value
      array[i] = memcpy(array[i], left_array[l++], DATA_SIZE);
    }
    
    else {//copy right array value
      array[i] = memcpy(array[i], right_array[r++], DATA_SIZE);
    }
  }

  // FREE THE COPIED ARRAYS
  if (left_array && right_array){ 
    for(int i = 0; i < (left_length); i++){
      free(left_array[i]);
    }
    free(left_array);
    for(int i = 0; i < (right_length); i++){
      free(right_array[i]);
    }
    free(right_array);
  }
}

/**********************************************************************************************************************************************************
 * print_array()
 * 
 * SIMPLY PRINT THE ARRAY BYTE, BY BYTE. THIS FUNCTION IS USED FOR DEBUGGING
 * 
 * PARAMETERS: 
 *  - unsigned char **array is the array to be printed
 *  - the num of indicies in the 2D array 
 * 
 * CALLS: no calls
**********************************************************************************************************************************************************/
void print_array(unsigned char **array, int num_indicies){
  int value;
  for(int i = 0; i< num_indicies; i++){
    printf("Array[%d]: ", i);
    for(int j = 0; j < DATA_SIZE; j++){
      printf("%02x ", array[i][j]);
    }
    printf("\n");
  }
}

/**********************************************************************************************************************************************************
 * xor_pieces()
 * 
 * XOR'S EACH BYTE IN EACH FLAG-PIECE, AND THEN PRINTS THE RESULTING FLAG. A NEW ARRAY IS ALLOCATED TO HOLD THE FLAG DATA.
 * 
 * PARAMETERS: 
 *  - unsigned char **array is the array to be printed
 *  - int length is the length of array 
 * 
 * CALLS: no calls
**********************************************************************************************************************************************************/
void xor_pieces(unsigned char** dataset, int length){
  unsigned char *FLAG = (unsigned char *)malloc(sizeof(unsigned char)*(DATA_SIZE - 2));
  if (FLAG == NULL) {
    perror("Allocation failed");
    exit(1);
  }

  for(int i = 0; i < length; i+=2) {
    if(i == 0){
      memcpy(FLAG, &dataset[i][2], DATA_SIZE - 2);
    }
    else{
      for (int j = 0; j < (DATA_SIZE - 2); j++) {
        FLAG[j] = (FLAG[j] ^ dataset[i][j + 2]);
      }
    }
  }

  for (int i = 0; i < (DATA_SIZE - 2); i++){
    printf("%c", FLAG[i]);
  }
  printf("\n");

  if(FLAG) free(FLAG);
}
