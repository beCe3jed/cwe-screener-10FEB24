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
Node *linked_list_head = NULL;      
Node *alphabetical_list_head = NULL;     // This will be the alphabetical list linked_list_head


void InsertNode(char *input_n, float input_w, float input_v);   
void printList();
void findLimit(int numItems);
void alphabetizeList(Node *incomingNode);
void printFlag();


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
        printf("Line %d: %s\n", lineNum, linePtr);


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










    // First we will organize each item into a list, the nodes will be sorted as they are inserted.
    for (int i = 0; i < numItems; i++){
        InsertNode(items[i], weights[i], values[i]);
    }
    printList();
    // Then we will find the list of items less than 35lbs
    findLimit(numItems);
    printFlag();







    // Properly free and close data as necessary
    if(fp) fclose(fp);
    if(linePtr) free(linePtr);
    if(items){
        printf("Freeing Items...");
        for (int i=0; i<numItems; i++){
            // printf("Freeing pointer to %s\n", items[i]);
            free(items[i]);
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
    Node *temp;
    printf("Freeing sorted linked list...");
    while(linked_list_head){
        temp = linked_list_head;
        linked_list_head = linked_list_head->next;
        free(temp);
    }
    printf("Freeing alphabetized linked list...");
    while(alphabetical_list_head){
        temp = alphabetical_list_head;
        alphabetical_list_head = alphabetical_list_head->next;
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
void InsertNode(char *input_n, float input_w, float input_v){
    Node *temp = linked_list_head;
    Node *before = NULL;
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

    newNode->weight = input_w;
    newNode->value = input_v;
    newNode->next = NULL;

    if (temp == NULL){
        linked_list_head = newNode;
        return;
    }
    while (temp != NULL){
        if( (newNode->value/newNode->weight) > (temp->value/temp->weight) ){
            newNode->next = temp;
            if(before != NULL){
                before->next = newNode;
            }
            else{
                newNode->next = linked_list_head;
                linked_list_head = newNode;
            }  
            return;
        }
        before = temp;
        temp = temp->next;
    }
    before->next = newNode;
    return;
} 



/* FINDS THE LIMIT OF THE CURRENT LIST. THIS FUNCTION USES THE NUMBER OF ELEMENTS (COLUMNNS IN A CSV) AND TRIES TO 
FIT AS MANY OBJECTS INTO THE PACK AS POSSIBLE. CURRENTLY THE PACK IS SET BY 'WEIGHT_LIMIT'. ITEMS ARE PRIORITIZED BY
THEIR VALUE PER WEIGHT, MAXIMIZING THEIR EFFICIENCY IN THE BACKPACK. IF AN OBJECTT IS TOO HEAVY FOR THE REMAINING 
WEIGHT, IT IS SIMPLY SKIPPED OVER. AS EACH NODE IS 'ENTERED' INTO THE BACKPACK, A FUNCTION ALPHABETIZES IT INTO ANOTHER LIST */
void findLimit(int numItems){
    Node *temp = linked_list_head;
    float load = 0;
    float current_value = 0;
    int list_length = 0;
    printf("\n\nStarting the load...\n");
    for(int i = 0; i < numItems; i++){
        load += temp->weight;
        current_value += temp->value;
        list_length++;
        if (load == WEIGHT_LIMIT){
            alphabetizeList(temp);
            printf(" += Load: %f\n", load);
            printf("Current Weight: %f\tCurrent Value: %f\tReturning Length: %d\n", load, current_value, list_length);
            return;
        }
        else if (load < WEIGHT_LIMIT) {
            alphabetizeList(temp);
            printf("Load: %f\n", load);
        }
        else {
            load -= temp->weight;
            current_value -= temp->value;
            list_length--;            
        }
        
        temp = temp->next;
    }
    printf("Current Weight: %f\tCurrent Value: %f\tReturning Length: %d\n", load, current_value, list_length);
    return;
}




/* SORTS THE PRIMED LIST. CURRENTLY GETS CALLED WITHIN findLimit(). THIS FUNCTION USES LESS COMPARISONS AT THE COST OF 
MEMORY. AN ADDITIONAL LINKED LIST IS CREATED TO INSERT EACH VALUE INHERENTLY INTO APLHABETICAL ORDER. IT MAY BE BETTER 
TO SORT THE LIST WITHOUT CREATING A COMPLETELY NEW ONE WITH DUPLICATE INFORMATION*/
void alphabetizeList(Node *incomingNode){
    printf("%15s: - Weight: %f", incomingNode->name, incomingNode->weight);
    Node *temp = alphabetical_list_head;
    Node *before = NULL;
    Node *newNode = (Node*)malloc(sizeof(Node));
    if(newNode == NULL){
        perror("Error allocating memory...");
        exit(1);
    }
    newNode->name = incomingNode->name;
    newNode->value = incomingNode->value;
    newNode->weight = incomingNode->weight;
    newNode->next = NULL;

    if (temp == NULL){
        alphabetical_list_head = newNode;
        return;
    }
    while (temp != NULL){
        if( strcmp(newNode->name, temp->name) < 0 ){
            newNode->next = temp;
            if(before != NULL){
                before->next = newNode;
            }
            else{
                newNode->next = alphabetical_list_head;
                alphabetical_list_head = newNode;
            }  
            return;
        }
        before = temp;
        temp = temp->next;
    }
    before->next = newNode;
    return;
}

/* PRINTS THE CURRENT LINKED LIST */
void printList(){
    Node *temp = linked_list_head;
    int i = 1;
    while (temp!=NULL){
        printf("Node #%d: %-18s--VpW: %f\n", i, temp->name, (temp->value/temp->weight));
        temp = temp->next;
        i++;
    }
    printf("NULL\n");
    return;
}

/* PRINTS THE ALPHABETIZED LINKED LIST INTO FLAG FORMAT */
void printFlag(){
    Node *temp = alphabetical_list_head;
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