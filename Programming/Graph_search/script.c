/*************************************************************************************************************************************************************
 * THIS PROGRAM TAKES A SERIALIZED FILE AND READS IN BYTES TO BE TRAVERSED AS A GRAPH. IT USES A BREADTH-FIRST SEARCH TO FIND THE ENDING NODE. 
 * 
 * SECTIONS:
 * 1. READ INTO MEMORY 
 *  - READ TO A BUFFER OF ONE_KILOBYTE AT A TIME, THEN COPY INTO A DUMP
 * 2. FORMAT DATA INTO A SET
 *  - TRANSFER DATA FROM A BLOCK OF MEMORY INTO A 2D ARRAY THAT CAN EASILY WORKED WITH
 * 3. EXECUTE BREADTH FIRST SEARCH
 *  - USE ARRAYS TO KEEP TRACK OF VISTIED, PATH, PARENTS ETC. SEE DEFINITIONS
 * 4. FREE MEMORY
 *  - FREE MEMORY BEFORE EXITING AS PER STANDARD PRACTICE
**************************************************************************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ONE_KILOBYTE 1024
#define EST_NUM_DATA_POINTS 100 // jsut an estimated value that gets increased as the number of nodes increases
#define FILE_NAME "input_stream.bin"

/*************************************************************************************************************************************************************
 * STRUCT: NODE
 * 
 * ATTRIBUTES:
 *  -  int vertex: the number representing the data point
 *  -  Node *next: a pointer to the next node 
 * 
 * USE:
 *  -   THE PURPOSE OF THIS STRUCT IS TO BUILD A QUEUE STRUCTURE THAT WILL BE USED FOR OUR BREADTH FIRST SEARCH. DATA POINTS WILL BE ENQUEUED BASED OFF OF OUT-LINKS
 * 
**************************************************************************************************************************************************************/
typedef struct Node{
    int vertex;
    struct Node* next;
} Node;
Node *first = NULL, *last = NULL; // global nodes for the queue
int BEG = 0, END = 0; // global values for the beginning and ending indices

/* TOOLS USED TO COMPLETE THE SEARCH OF THE GRAPH, SEPARATING INTO FUNCTIONS GREATLY INCREASES READABILITY */
void BFS(unsigned char **adjacency_list, int num_verticies);
int *search(unsigned char **adjacencylist, int num_verticies);
int *reverse_path(int *prev, int num_verticies, int *size_of_path);

/* TOOLS USED FOR THE QUEUE DATA STRUCTURE */
void enqueue(int index);
int dequeue();
void printQueue();

int main (void) {
    /******************************************************************************* SECTION 1. READ INTO MEMORY *************************************************************/
    FILE *fp = NULL;
    int dump_size = ONE_KILOBYTE;
    int num_bytes = 0, total_bytes = 0, multiplier = 1;
    unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * ONE_KILOBYTE);
    if (buffer == NULL){
        printf("Error allocation the buffer");
        exit(2);
    }
    unsigned char *dump = (unsigned char *)malloc(sizeof(unsigned char) * ONE_KILOBYTE);
    if (dump == NULL){
        printf("Error allocating the dump");
        exit(2);
    }  
    if ((fp = fopen(FILE_NAME, "rb")) == NULL){
        printf("Error opening the file!");
        exit(1);
    }

    // Grab all the data from the file and dump 
    while ((feof(fp) == 0) && ((num_bytes = fread(buffer, 1, (ONE_KILOBYTE), fp)) > 0)) {
        total_bytes += num_bytes;
        if (total_bytes <= ONE_KILOBYTE){ // first iteration
            memcpy(dump, buffer, num_bytes);
        } 
        else {
            memcpy(&dump[total_bytes], buffer, num_bytes); // APPENDS DATA ONTO THE END OF THE ARRAY
        }
        if(num_bytes == (ONE_KILOBYTE)){ // the buffer needs to be increased for the NEXT iteration
            dump = (unsigned char*)realloc(dump, (dump_size * (++multiplier)));
        }
    }
    if (num_bytes < 0) {
        printf("Error reading from the file");
        exit(1);
    }
    if(buffer) free(buffer);

/******************************************************************************* SECTION 2. FORMAT DATA INTO A SET *************************************************************/
    // Transition the data into a graph format
    int data_points = EST_NUM_DATA_POINTS;
    unsigned char **graph_set = (unsigned char **)malloc(sizeof(unsigned char*) * data_points);
    if (graph_set == NULL) {
            printf("Error allocating the graph set");
            exit(2);
        }
    int b = 0, num_vertices = 0, vertex = 0;
    while(b < total_bytes) {
        if ( ((b + 1) >= total_bytes) || ((b + 2) >= total_bytes) ){
            printf("Our index has gone out-of-bounds, there is invalid data");
            exit(1);
        }
        int outs = (int)dump[b + 1] + ((int)dump[b + 2] << (1*8)); // parses the length (bytes 1 & 2) of the out-links
        int full_length = 3 + (outs * 2); //computes the total length in bytes
        num_vertices++; //increment the counter for our vertices
        if (num_vertices == data_points) { // if our estimate needs to be increased
            // reallocate the graphset
            data_points *= 2;
            graph_set = (unsigned char **)realloc(graph_set, (sizeof(unsigned char*) * (data_points)));
            if (graph_set == NULL) {
                printf("Error reallocating the graph set");
                exit(2);
            }
        }
        graph_set[vertex] = (unsigned char*)malloc(sizeof(unsigned char) * full_length);
        if (graph_set[vertex] == NULL) {
            printf("Error allocating a vertex with the graphset");
            exit(2);
        }
        for (int j = 0; j < full_length; j++){ // grab the full dataline 
            graph_set[vertex][j] = dump[b++];
        }
        vertex++;
    }

    // reallocate the graph set in order to use efficient space
    graph_set = (unsigned char **)realloc(graph_set, (sizeof(unsigned char*) * num_vertices));
    if (graph_set == NULL){
        printf("Error during reallocation of the graph_set");
        exit(1);
    }


/******************************************************************************* SECTION 3. EXECUTE BREADTH FIRST SEARCH *************************************************************/
    BFS(graph_set, num_vertices);

/******************************************************************************* SECTION 4. FREE MEMORY *************************************************************/

    for (int i = 0; i < num_vertices; i++){
        if(graph_set[i]) free(graph_set[i]);
    }
    if(graph_set) free(graph_set);
    if (dump) free(dump);
    fclose(fp);
    return 0;
}




/*************************************************************************************************************************************************************
 * BFS()
 * 
 * THIS FUNCTION NEATLY ORGANIZES THE SEARCH AND IS SEPARATED INTO A DIFFERENT FUNCTION IN ORDER TO INCREASE READABILITY AND MODULARITY. IT CALLS OTHER FUNCTIONS
 * TO DO THE WORK, BUT PRINTS DATA AFTER THE CALLS. 
 * 
 * 
 * PARAMS:
 *  - unsigned char **adjacency_list = our saved set of nodes/links
 *  - int num_verticies = the total number of verticies in the adjacency_list
 * 
 * VARIABLES:
 *  - prev is a pointer to an array that contains... 
 *  - path is a pointer to ...
 * 
 * CALLS:
 *  - search()
 *  - reverse_path()
**************************************************************************************************************************************************************/
void BFS(unsigned char **adjacency_list, int num_verticies){
    // start the search
    int *prev = search(adjacency_list, num_verticies);

    printf("Search complete!\n [");
    for (size_t i = 0; i < num_verticies; i++)
    {
        printf(" %d,", prev[i]);
    }
    printf("]\n");

    // return the reconstructed path
    int num_in_path = 0;
    int *path = reverse_path(prev, num_verticies, &num_in_path);

    // print flag
    for (int i = 0; i < num_in_path; i++){
        printf("%c", adjacency_list[path[i]][0]);
    }

    // free pointers
    if(prev) free(prev);
    if(path) free(path);
}

/*************************************************************************************************************************************************************
 * search()
 * 
 * THIS FUNCTION CREATES TWO ARRAYS THAT KEEP TRACK OF NODES VISITED AND PARENT NODES.  WE START WITH THE BEGINNING NODE, AND ENQUEUE IT. WE THEN ENTER A LOOP 
 * THAT DEQUEUES WHATEVER IS ON OUR STRUCTURE, AND ENQUEUES ALL OF THE LINKS (AS LONG AS THEY HAVE NOT BEEN VISITED BEFORE). IF THE END IS FOUND, THE LOCATION 
 * IS PRINTED AND SAVED, BUT THE LOOP WILL CONTINUE UNTIL ALL NODES ARE EXPLORED.
 * 
 * PARAMS:
 *  - unsigned char **adjacency_list = our saved set of nodes/links
 *  - int num_verticies = the total number of verticies in the adjacency_list
 * 
 * VARIABLES:
 *  - seen is an array that keeps track of each node that was visited
 *  - prev is an array that stores the parent to the node, it will be used later when we reverse the list
 * 
 * CALLS:
 *  - enqueue()
 *  - dequeue()
**************************************************************************************************************************************************************/
int *search(unsigned char **adjacencylist, int num_verticies){

    enqueue(BEG); // adds the first node

    int *seen = (int*)calloc(num_verticies, sizeof(int)); // array of 1 or 0 if the node has been visited
    int *prev = (int*)calloc(num_verticies, sizeof(int)); // array that stores the parent node

    if((seen == NULL) || (prev == NULL)) {
        printf("Error allocating the visited/prev array");
        exit(2);
    }
    seen[BEG] = 1; // first node has been visited

    // initiate using -1 to represent null
    for(int i = 0; i < num_verticies; i++) { 
        prev[i] = -1;
    }

    // go thru dequeueing nodes and enqueueing its respective links
    while (first != NULL){ // checks if the queue is not empty
        int current = dequeue(); // grab the popped value
        int links = (int)adjacencylist[current][1] + ((int)adjacencylist[current][2] << (1*8)); // parse from little endian
        int b = 0;
        
        // go thru each link and add to the queue if it hasn't been seen. If the end is found, display it, but keep checking through 
        for(int i = 0; i < links; i++){
            int neighbor = (int)adjacencylist[current][3 + b++] + ((int)adjacencylist[current][3 + b++] << (1*8)); // starting the 3rd index, b increments twice as much as i
            if (seen[neighbor] == 0){
                enqueue(neighbor);
                seen[neighbor] = 1;
                prev[neighbor] = current;
                
                if (adjacencylist[neighbor][0] == '}')
                    printf("XXXXXXX FOUND } at %d XXXXXXXXX\n", neighbor);
                    END = neighbor;
            }
        }
    }
    if (seen)free(seen);
    return prev; // prev is allocated on the heap, its okay to return
}



/*************************************************************************************************************************************************************
 * reverse_path()
 * 
 * THIS FUNCTION FOLLOWS A PATH OF FOUND NODES AND SAVES IT INTO AN ARRAY reversed, BUT THE PATH IS BACKWARDS, SO A CORRECT LIST IS SAVED TO path. IF END HASNT 
 * CHANGED, THEN WE ALERT THE USER AND EXIT
 * 
 * PARAMS:
 *  - int *prev = our set of nodes/parents
 *  - int length = the total number of verticies in the adjacency_list
 *  - int *size_of_path = a ptr that saves the length of the path for printing the flag
 * 
 * VARIABLES:
 *  - reversed is an array to hold our path, but it will be backwards
 *  - path will be the correctly formed version of reversed
 * 
**************************************************************************************************************************************************************/
int *reverse_path(int *prev, int length, int *size_of_path){
    // this is an array to hold our path, but it will be backwards
    int *reversed = (int *)malloc(sizeof(int) * length);
    if (reversed == NULL){
        printf("Error allocating path");
        exit(2);
    }
    
    // build the path from end to the beginning
    int index = 0;
    for (int current = END; current != -1; current = prev[current])
    {
        reversed[index++] = current;
    }
    int *path = (int*)malloc(sizeof(int) * (index + 1)); // path will match the size of reversed
    if (path == NULL){
        printf("Error allocating path");
        exit(2);
    }
    
    // reverse the path
    int i = 0;
    for (int current = index; current >= 0; current--)
    {
        path[i++] = reversed[current];
    }
    *size_of_path = i; // this is our size of the path array
    if (END == 0) {
        printf("End was never found");
        exit(1);
    }
    if (reversed) free(reversed);
    return path;
}

/*************************************************************************************************************************************************************
 * enqueue()
 * 
 * FOLLOWS TYPICAL QUEUE STRUCTURE OF FIFO
 * 
 * EACH NODE GETS ALLOCATED IN THIS FUNCTION
 * 
 * GLOBALS:
 *  - CHANGES FIRST AND LAST TO KEEP TRACK OF THE STATE OF THE QUEUE
 * 
 * CREATES:
 *  - NEW NODE WITH EVERY CALL WITH INDEX AS DATA
*************************************************************************************************************************************************************/
void enqueue(int index){
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Error allocating new node");
        exit(2);
    }
    newNode->vertex = index;
    newNode->next = NULL;

    if (first == NULL) {
        first = newNode;
        last = newNode;
        return;
    }
    last->next = newNode;
    last = newNode;
}
/*************************************************************************************************************************************************************
 * dequeue()
 * 
 * FOLLOWS TYPICAL QUEUE STRUCTURE OF FIFO
 * 
 * EACH NODE GETS ALLOCATED IN THIS FUNCTION
 * 
 * GLOBALS:
 *  - CHANGES FIRST AND LAST TO KEEP TRACK OF THE STATE OF THE QUEUE
 * 
 * CREATES:
 *  - NODE GETS FREED WITH EACH CALL
*************************************************************************************************************************************************************/
int dequeue(){
    int temp_data;
    if (first == NULL){
        printf("Error! There's nothing in the Queue!");
        exit(3);
    }
    else if(first == last){
        temp_data = first->vertex;
        free(first);
        first = last = NULL;
        return temp_data;
    }
    else {
        Node *temp = first;
        temp_data = first->vertex;
        first = first->next;
        free(temp);
        return temp_data;
    }

}
/*************************************************************************************************************************************************************
 * printQueue()
 * 
 * DISPLAYS THE VALUES IN THE QUEUE FOR DEBUGGING PURPOSES
*************************************************************************************************************************************************************/
void printQueue(){
    Node* temp = first;
    printf("Head\n");
    while (temp != NULL){
        printf("  %d  ", temp->vertex);
        temp = temp->next;
    }
    printf("NULL\n");
}