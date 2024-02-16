#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://www.youtube.com/watch?v=dDjfaA9Q3n8
// https://www.geeksforgeeks.org/c-program-to-read-contents-of-whole-file/

#define ONE_KILOBYTE 1024
#define DATA_SIZE 15

int tracker = 0;

void printFlag();
void merge_sort(unsigned char ** array, int length); 
void merge_sort_recursion(unsigned char **array, int left, int right);
void merge_sorted_arrays(unsigned char **array, int left, int mid, int right);
void print_array(unsigned char **array, int length);
void xor_pieces(unsigned char** array, int length);


int main(void) {
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
    // printf("Buff_Mult: %d\n", buffer_multiplier);
    // printf("Bytes read: %d\n", total_bytes);
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

  // TESTING THAT THE DUMP IS CORRECT.
  for (int i = 0; i < total_bytes; i++){
    // printf("%02x ", dump[i]);
  }
  printf("\n");


  // THIS IS TO TRANSFER THE BYTES FROM THE DUMP INTO THE ARRAY FORMAT
  int num_indices = total_bytes/DATA_SIZE; 
  unsigned char **array = (unsigned char**)malloc(sizeof(unsigned char*) * num_indices);
  if (array == NULL) {
    perror("Error allocating space for the double_pointer array");
    exit(2);
  }
  int row, byte;
  byte = 0;
  row = -1;
  for (byte = 0; byte < total_bytes; byte++) { //  byte by byte in dump
    // printf("col = %d, mod = %d\n", byte, byte % DATA_SIZE);

    if ((byte % DATA_SIZE) == 0) { // initializes each subarray and sets the null terminator
      // printf("Iterating to row... %d\n", ++row);
      array[++row] = (unsigned char*)malloc((sizeof(unsigned char) * (DATA_SIZE + 1)));
      // printf("Array[%i]: %s\n", row, array[row]);
      if (array[row] == NULL){
        perror("Error allocating space for the individual strings of bytes.");
        exit(2);
      }
      // printf("Allocated correctly\n");
      array[row][DATA_SIZE] = '\0';
      // printf("\nArray [%d]: ", row);
    }
    array[row][(byte % DATA_SIZE)] = dump[byte]; // copies the bytes from the dump approprately
    // printf("%02x ", array[row][(byte % DATA_SIZE)]);
  }


  print_array(array, num_indices);
  printf("\n\n", array);
  merge_sort(array, num_indices);
  print_array(array, num_indices);
  xor_pieces(array, num_indices);







  if (buffer) free(buffer);
  if (dump) free(dump);
  if (fp) fclose(fp);
  printf("Exit succesful!");
  return 0;
}

// SIMPLISTIC MERGESORT CALL FOR THE EASE OF THE USER
void merge_sort(unsigned char **array, int length){
  merge_sort_recursion(array, 0, length - 1);
}

// THE RECURSION PART OF MERGESORT, BREAKS THE ARRAY DOWN INTO ITS SINGULAR INDICES
void merge_sort_recursion(unsigned char **array, int left, int right) {
  if (left < right) {
    int mid = left + (right - left)/2;
    merge_sort_recursion(array, left, mid);
    merge_sort_recursion(array, mid + 1, right);
    merge_sorted_arrays(array, left, mid, right);
  }
}

// THE COMPARISON AND MERGENCE OF THE ARRAY
void merge_sorted_arrays(unsigned char **array, int beg, int mid, int end) {
  int left_length = mid - beg + 1;
  int right_length = end - mid;

  // printf("Merge #%d-------\n Begin: %d, Mid: %d, End: %d\n", tracker++, beg, mid, end);

  // allocating the arrays
  unsigned char **left_array = (unsigned char**)malloc(sizeof(unsigned char*)*left_length);
  unsigned char **right_array = (unsigned char**)malloc(sizeof(unsigned char*)*right_length);
  if ((left_array == NULL) || (right_array == NULL)){
    perror("Error allocating substitute");
    exit(1);
  }
  for (int i = 0; i < left_length; i++){
    left_array[i] = (unsigned char*)malloc(sizeof(unsigned char)*DATA_SIZE);
    if (left_array[i] == NULL) {
      perror("Error allocating left array");
      exit(1);
    }
    left_array[i] = memcpy(left_array[i], array[beg + i], DATA_SIZE);
  }
  for (int i = 0; i < right_length; i++){
    right_array[i] = (unsigned char*)malloc(sizeof(unsigned char)*DATA_SIZE);
    if (right_array[i] == NULL) {
      perror("Error allocating right array");
      exit(1);
    }
    right_array[i] = memcpy(right_array[i], array[mid + 1 + i], DATA_SIZE);
  }


   // The actual merge
  int left_val, right_val;
  for(int i = beg, l = 0, r = 0; i <= (end); i++){
    if (l < left_length) left_val = (int)left_array[l][0] + ((int)left_array[l][1] << (1*8));
    if(r < right_length) right_val = (int)right_array[r][0] + ((int)right_array[r][1] << (1*8));
    int dbg_val = (int)array[i][0] + ((int)array[i][1] << (1*8));
    // printf("Array[%d] was %d... ", i, dbg_val);

    // copy left array value
    if ( (l < left_length) && ((r >= right_length) || (left_val <= right_val)) ) {
      array[i] = memcpy(array[i], left_array[l++], DATA_SIZE);
      dbg_val = left_val;
      // printf("*Left* ");
    }
    //copy right array value
    else {
      array[i] = memcpy(array[i], right_array[r++], DATA_SIZE);
      dbg_val = right_val;
      // printf("*Right* ");
    }
    // printf("now it is Array[%d] = %d\n", i, dbg_val);
  }

  // ALLOWS YOU TO PRINT THE ARRAY AFTER EACH MERGE
  // printf("(Sorted) Array: ");
  // for(int i = beg; i <= end; i++) {
  //   int value = (int)array[i][0] + ((int)array[i][1] << (1*8));
  //   printf("%d ", value);
  // }
  // printf("\n\n");



  // free the arrays
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


void print_array(unsigned char **array, int length){
  int value;
  for(int i = 0; i< length; i++){
    printf("Array[%d]: ", i);
    for(int j = 0; j < DATA_SIZE; j++){
      printf("%02x ", array[i][j]);
    }
    printf("\n");
  }
}



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






void printFlag(unsigned char *FLAG){
  unsigned long long int bytes1 = 0xfe7f2ccb8a;
  unsigned long long int bytes2 = 0x98134dacf1;
  unsigned long long int result = bytes1 ^ bytes2;

  printf("\nBytes 1: 0x%llx, Bytes 2: 0x%llx, XOR: 0x%llx\n\n", bytes1, bytes2, result);

  printf("Result in chars: %c%c%c%c%c\n", 
    (char)((result >> (4*8)) & 0xFF), 
    (char)((result >> (3*8)) & 0xFF), 
    (char)((result >> (2*8)) & 0xFF),
    (char)((result >> (1*8)) & 0xFF),
    (char)(result & 0xFF));
  return;
}
