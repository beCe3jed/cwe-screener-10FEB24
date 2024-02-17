#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

// We will need to read the file in, and tokenize (&parse) each line into an array as they come
// We will save into 3 arrays, 
//          -Option 1: sort each array accordinglingly via inserting into a linked-list  ***SELECTED***
//          -Option 2: sort each array in-house (requires 3 switches each time)
//          -Option 3: dump directly into a linked-list, requires 2 extra full-iterations, and a full-sort

// knapsack problem
// https://www.youtube.com/watch?v=cJ21moQpofY

typedef struct Node {
    struct Node *next;         // pointer to the next node
    char *name;        
    float value;       
    float weight;      
} Node;

#define WEIGHT_LIMIT 35 // the max weight that a marine can carry
#define PRECISION 8 // to what precision we will calculate the optimal packing to. 8 means that we will have increments of 1/8 when comparing weights
Node *head = NULL;      



void InsertNode(char *input_n);   
void printList();
void findLimit(int numItems);
void alphabetizeList(Node *incomingNode);
void printFlag();

void merge_sort(char **array, int length); 
void merge_sort_recursion(char **array, int left, int right);
void merge_sorted_arrays(char **array, int beg, int mid, int end);


int main (void) {
    FILE *fp = NULL;
    char *linePtr = NULL;
    char **items;                 
    float *values = NULL;
    float *weights = NULL;
    int bufferSize = 0;
    int lineSize;
    int lineNum = 0;
    char *token = NULL;
    int numItems = 0;
    const char delimiter = ',';

    if ((fp = fopen("data.csv", "r")) == NULL){
        perror("Unable to open the file");
        exit(1);
    }
    while(lineSize = getline(&linePtr, &bufferSize, fp) != -1){
        // get rid of the newline --- This actually isn't properly working with removing the newline off of Zinc-Oxide
        while ((lineSize != 0) && (linePtr[lineSize-1] == '\n' || linePtr[lineSize-1] == '\r')){
            linePtr[lineSize--] = '\0';
        }
        // printf("Line %d: %s\n", lineNum, linePtr);


        // tokenize
        if (lineNum == 0){
            items = (char**)malloc(100 * sizeof(char*));
            if(items == NULL) {
                perror("Error allocating memory for items");
                exit(1);
            }

            token = strtok(linePtr, &delimiter);            // lineptr will move on to new lines, in which 'token' still has a pointer to, and items[i] still has a pointer to
            while(token){
                items[numItems] = (char*)malloc((strlen(token)+1)*sizeof(char));
                if (items == NULL){
                    perror("Error allocating memory for the linked_list_header");
                    exit(1);
                }
                strcpy(items[numItems++], token); // VULN!
                token = strtok(NULL, &delimiter);
            }
            printf("\nNumber of items: %d\n", numItems);
            values = (float*)malloc(sizeof(float) * numItems);
            weights = (float*)malloc(sizeof(float) * numItems);
            if (values == NULL || weights == NULL){
                perror("Error allocating memory for values/weights");
                exit(1);
            }
        }

        else if (lineNum == 1){
            int i = 0; 
            token = strtok(linePtr, &delimiter);
            while (token){
                weights[i++] = (float)atof(token); // This might be the error with the Null byte!
                token = strtok(NULL, &delimiter);
            }
        }

        else if (lineNum == 2){
            int i = 0; 
            token = strtok(linePtr, &delimiter);
            while (token){
                values[i++] = (float)atof(token); 
                token = strtok(NULL, &delimiter);
            }
        }
        lineNum++;
    }

    for (int i = 0; i < numItems; i++){
        printf("At %d Item: %s, Weight: %f, Value: %f\n", i, items[i], weights[i], values[i]);
    }



    // first we will make a matrix so that we can calculate the best knapsack 
    float **matrix = (float **)calloc(numItems, sizeof(float*));
    if (matrix == NULL) {
        printf("error allocating matrix");
        return 1;
    }
    for (int i = 0; i < numItems; i++){
        matrix[i] = (float *)calloc((WEIGHT_LIMIT * PRECISION) + 1, sizeof(float)); // each index represents 1/8 lb
        if (matrix[i] == NULL) {
            printf("error allocating matrix");
            return 1;
        }
    }


    // step through each element and calculate the appropriate values
    for (int row = 0; row < numItems; row++) { // iterate through each item
        for(int col = 0; col < ((WEIGHT_LIMIT * PRECISION) + 1); col++){ // iterate through 0 - 281 of knapsack by 1/8 lb 
            if (weights[row] <= ((float)col/(float)PRECISION)){ // item's weight is less or equal to the sub-knapsack
                if (row == 0){ // first row makes no comparisons
                    // printf("%.3f is less than %d/%d\n", weights[row], col, PRECISION);
                    matrix[row][col] = values[row];
                }
                else {
                    if ( matrix[row - 1][col] >= (matrix[row - 1][col - (int)(weights[row] * PRECISION)] + values[row]) ){ //if the sub-knapsack has a better value without adding in the current item
                        matrix[row][col] = matrix[row - 1][col];
                    } 
                    else { // if the sub-knapsack has better value if it includes the current item... accounting for weight
                        matrix[row][col] = (matrix[row - 1][col - (int)(weights[row] * PRECISION)] + values[row]);
                    }
                }
            }
            else { // the item's weight is too big for current knapsack, so just copy the above row
                if (row != 0) { // the first item will just default to zero
                    matrix[row][col] = matrix[row - 1][col];
                }
            }
        }
    }


    
    // char **optimal_list = (char**)calloc(numItems, sizeof(char*));
    // if (optimal_list == NULL){
    //     printf("Error allocating the optimal list");
    //     return -1;
    // }
    
    //NOW LETS TRAVERSE THE MATRIX BACKWARDS, FIGURING OUT OUR BEST KNAPSACK
    int row = numItems - 1; // starting at the bottom item
    int col = WEIGHT_LIMIT * PRECISION; // starting at the final column
    int counter = 0;
    float total_weight = 0;
    while (row != 0){
        if (matrix[row][col] != matrix[row - 1][col]){
            // optimal_list[counter] = items[row];
            InsertNode(items[row]);
            total_weight += weights[row];
            printf("\n COUNT: %d - Item %s added! V = %.3f, W = %.3f... Total Value: %.3f... TOTAL WEIGHT: %.3f", counter, items[row], values[row], weights[row], matrix[row][col], total_weight);
            col -= (weights[row] * PRECISION); // Each index represents 1/8 lb, so we multiply by 8 to get a proper index
            counter++;
        }
        row--;
    }

    // for (int i = 0; i < counter; i++){
    //     printf("\nItem %d: %s", i, optimal_list[i]);
    // }

    printf("\n\n");
    printList();
    printFlag();







    // merge_sort(optimal_list, counter);

    // // First we will organize each item into a list, the nodes will be sorted as they are inserted.
    // for (int i = 0; i < numItems; i++){
    //     InsertNode(items[i], weights[i], values[i]);
    // }
    // printList();
    // // Then we will find the list of items less than 35lbs
    // findLimit(numItems);
    // printFlag();


    // Properly free and close data as necessary
    if(fp) fclose(fp);
    if(linePtr) free(linePtr);
    if(items){
        printf("\nFreeing Items x2...");
        for (int i=0; i<numItems; i++){
            // printf("Freeing pointer to %s\n", items[i]);
            free(items[i]);
            free(matrix[i]);
        }
        printf("Freeing the array of pointers...");
        free(items);
    }
    if(values){
        printf("Freeing the array of values...");
        free(values);
    }
    if(weights){
        printf("Freeing the array of weights...");
        free(values);
    }
    if(matrix){
        printf("Freeing MAtrix...");
        free(matrix);
    }
    Node *temp;
    printf("Freeing sorted linked list...");
    while(head){
        temp = head;
        head = head->next;
        free(temp);
    }
    printf("It was successful!");
    return 0;
}


/*
-----------------------------------------------------------------------------------------------------------------------------
                                                Functions
-----------------------------------------------------------------------------------------------------------------------------
*/


/* 
ADD NODES INTO A LIST. THEY ARE ORIGINALLY IN AN ARRAY FORMAT, THEREFORE INSERTION SHOULD INVOLVE SORTING SIMULTANEOUSLY.
BY TAKING THE DATA INTO A LINKED LIST, WE GREATLY SIMPLIFY THE AMOUNT OF ITERATIONS THROUGH THE ARRAYS AT THE COST OF 
TAKING UP EXTRA SPACE. 
*/
void InsertNode(char *input_n){
    Node *temp = head;
    Node *prev = NULL;
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Error allocating node!");
        exit(1);
    }

    newNode->name = malloc(strlen(input_n) + 1);
    if (newNode->name == NULL) {
        perror("Error allocating node!");
        exit(1);
    }
    strcpy(newNode->name, input_n);

    newNode->next = NULL;

    if (temp == NULL){
        head = newNode;
        return;
    }
    while (temp != NULL){
        if( strcmp(newNode->name, temp->name) < 0){
            newNode->next = temp;
            if(prev != NULL){
                prev->next = newNode;
            }
            else{
                newNode->next = head;
                head = newNode;
            }  
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    prev->next = newNode;
    return;
} 

/* PRINTS THE CURRENT LINKED LIST */
void printList(){
    Node *temp = head;
    int i = 1;
    while (temp!=NULL){
        printf("Node #%d: %-18s\n", i, temp->name);
        temp = temp->next;
        i++;
    }
    printf("NULL\n");
    return;
}

/* PRINTS THE ALPHABETIZED LINKED LIST INTO FLAG FORMAT */
void printFlag(){
    Node *temp = head;
    int i = 1;
    printf("\nHere is the answer: flag{");
    while (temp!=NULL){
        printf("%c", temp->name[0]);
        temp = temp->next;
        i++;
    }
    printf("}\n\n");
    return;
}





/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!NOT BEING USED!!!!!!!!!!!!!!!!!!!!!!!!!*/
void merge_sort(char **array, int length){
    merge_sort_recursion(array, 0, length - 1);
    return;
}
void merge_sort_recursion(char **array, int left, int right){
    if ( left < right){
        int mid = left + (right - left) / 2; 
        merge_sort_recursion(array, left, mid);
        merge_sort_recursion(array, mid + 1, right);
        merge_sorted_arrays(array, left, mid, right);
    }
}
void merge_sorted_arrays(char **array, int beg, int mid, int end){
    int left_length = mid - beg  + 1;
    int right_length = end - mid;

    char **left_array = (char**)malloc(sizeof(char*) * left_length);
    char **right_array = (char**)malloc(sizeof(char*) * right_length);
    if (left_array == NULL || right_array == NULL) {
        printf("Error allocating the left and/or right arrays");
        exit(1);
    }

    for (int i = 0; i < left_length; i++){ // allocate the left_array by copying values from the original array
        left_array[i] = (char*)malloc(sizeof(char) * (strlen(array[beg + i]) + 1));
        if (left_array[i] == NULL) {
            printf("Error allocating the left_array index");
            exit(1);
        }
        memcpy(left_array[i], array[beg + i], strlen(array[beg + i]));
        left_array[strlen(array[beg + i])] = '\0';
    }
    for (int i = 0; i < right_length; i++){// allocate the right_array by copying values from the original array
        right_array[i] = (char*)malloc(sizeof(char) * (strlen(array[mid + i + 1]) + 1));
        if (right_array[i] == NULL) {
            printf("Error allocating the left_array index");
            exit(1);
        }
        memcpy(right_array[i], array[mid + i + 1], strlen(array[mid + i + 1]));
        right_array[strlen(array[mid + i + 1])] = '\0';
    }

    int left_val, right_val;
    char *old_address = NULL;
    for (int i = beg, l = 0, r = 0; i <= end; i++){
        if (l < (left_length) && (r >= left_length || (strcmp(left_array[l], right_array[r]) < 0)) ) {
            old_address = array[i];
            array[i] = (char*)realloc(array[i], strlen(left_array[l] + 1));
            if (old_address != array[i]) free(old_address);
            memcpy(array[i], left_array[l], strlen(left_array[l] + 1));
            l++;
        }
        else {
            old_address = array[i];
            array[i] = (char*)realloc(array[i], strlen(right_array[r] + 1));
            if (old_address != array[i]) free(old_address);
            memcpy(array[i], right_array[r], strlen(right_array[r] + 1));
            r++;
        }
    }


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