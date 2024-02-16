#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ONE_KILOBYTE 1024

typedef struct Node{
    int vertex;
    struct Node* next;
} Node;

Node *first = NULL, *last = NULL; // global nodes for the queue
int BEG = 0, END = 0; // global values for the beginning and ending indices

void BFS(unsigned char **adjacency_list, int num_verticies);
int *search(unsigned char **adjacencylist, int num_verticies);
int *get_neighbors(unsigned char *vertex);
int *reverse_path(int *prev, int num_verticies);

void enqueue(int index);
int dequeue();
void printQueue();

int main (void) {
    FILE *fp = NULL;
    int dump_size = ONE_KILOBYTE;
    int num_bytes = 0, total_bytes = 0, multiplier = 1;
    unsigned char *old_address = NULL;
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
    
    if ((fp = fopen("input_stream.bin", "rb")) == NULL){
        printf("Error opening the file!");
        exit(1);
    }

    // Grab all the data from the file and dump 
    while ((feof(fp) == 0) && ((num_bytes = fread(buffer, 1, (ONE_KILOBYTE), fp)) != 0)) { // what if num_bytes == -1?
        total_bytes += num_bytes;
        if (total_bytes <= ONE_KILOBYTE){ // first iteration
            memcpy(dump, buffer, num_bytes);
        } 
        else {
            memcpy(&dump[total_bytes], buffer, num_bytes);
        }

        if(num_bytes == (ONE_KILOBYTE)){ // the buffer needs to be increased for the NEXT iteration
            old_address = dump;
            dump = (unsigned char*)realloc(dump, (dump_size * (++multiplier)));
            if(old_address != dump) free(old_address); // if the pointer moved, free the old pointer
        }
    }
    if(buffer) free(buffer);

    // VERIFY THE DUMP IS ACCURATE
    // printf("Total Bytes: %d\nDump:", total_bytes);
    // for (int i = 0; i < total_bytes; i++){
    //     if ((i%16) == 0) printf("\n");
    //     printf("%02x ", dump[i]);
    // }
    printf("\n\n");



    // Transition the data into a usable format
    int data_points = 100;
    unsigned char ** old_dbl_ptr;
    unsigned char **graph_set = (unsigned char **)malloc(sizeof(unsigned char*) * data_points);
    if (graph_set == NULL) {
            printf("Error allocating the graph set");
            exit(2);
        }
    int b = 0, num_vertices = 0, vertex = 0;
    while(b < total_bytes) {
        int outs = (int)dump[b + 1] + ((int)dump[b + 2] << (1*8)); // parses the length of the out-links
        int full_length = 3 + (outs * 2); //computes the total length in bytes
        num_vertices++;
        if (num_vertices == data_points) {
            // reallocate the graphset
            old_dbl_ptr = graph_set;
            data_points *= 2;
            graph_set = (unsigned char **)realloc(graph_set, (sizeof(unsigned char*) * (data_points)));
            if (graph_set == NULL) {
                printf("Error reallocating the graph set");
                exit(2);
            }
            if (old_dbl_ptr != graph_set) free(old_dbl_ptr);
        }
        graph_set[vertex] = (unsigned char*)malloc(sizeof(unsigned char) * full_length);
        if (graph_set[vertex] == NULL) {
            printf("Error allocating a vertex with the graphset");
            exit(2);
        }
        for (int j = 0; j < full_length; j++){
            graph_set[vertex][j] = dump[b++];
            // printf("%02x ", graph_set[vertex][j]);
        }
        vertex++;
        printf("\n");
    }
    printf("\n");

    // reallocate the graph set in order to use efficient space
    old_dbl_ptr = graph_set;
    graph_set = (unsigned char **)realloc(graph_set, (sizeof(unsigned char*) * num_vertices));
    if (old_dbl_ptr != graph_set) free(old_dbl_ptr);

    // VERIFY CODE WAS SAVED CORRECTLY
    b = 0;
    for (int i = 0; i < num_vertices; i++){
        int outs = (int)graph_set[i][b + 1] + ((int)graph_set[i][b + 2] << (1*8)); // parses the length of the out-links
        int full_length = 3 + (outs * 2); //computes the total length in bytes
        printf("Index %d: \n", i);
        for (int j = 0; j < full_length; j++){
            printf("%02x ", graph_set[i][j]);
        }
        printf("\n");
    }
    printf("Number of Nodes: %d\n", num_vertices);
    


    // TESTING ENQUEUE/DEQUEUE
    // for (int i = 0; i < 10; i++){
    //     enqueue(i);
    // }
    // printQueue();
    // for (size_t i = 0; i < 10; i++)
    // {
    //     int trash = dequeue();
    //     printf("Dequeued %d!\n", trash);
    //     printQueue();
    // }


    // NOW EXECUTE A BREADTH FIRST SEARCH
    BFS(graph_set, num_vertices);



    
    // for (int i = 0; i < num_vertices; i++){
    //     free(graph_set[i]);
    // }
    // free(graph_set);
    if (dump) free(dump);
    fclose(fp);
    return 0;
}

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
    int *path = reverse_path(prev, num_verticies);
    for (int i = 0; i < num_verticies; i++){
        printf("%c", adjacency_list[path[i]][0]);
    }

    if(prev) free(prev);
    if(path) free(path);
}

int *reverse_path(int *prev, int length){
    int *reversed = (int *)malloc(sizeof(int) * length);
    if (reversed == NULL){
        printf("Error allocating path");
        exit(2);
    }
    int index = 0;
    // build the path from end to the beginning
    for (int current = END; current != -1; current = prev[current])
    {
        reversed[index++] = current;
    }
    int *path = (int*)malloc(sizeof(int) * (index + 1));
    if (path == NULL){
        printf("Error allocating path");
        exit(2);
    }
    int i = 0;
    // reverse the path
    for (int current = index; current >= 0; current--)
    {
        path[i++] = reversed[current];
    }
    if (END == 0) printf("End was never found");
    if (reversed) free(reversed);
    return path;
}

int *search(unsigned char **adjacencylist, int num_verticies){
    enqueue(BEG); // adds the first node
    int *seen = (int*)calloc(num_verticies, sizeof(int)); // array of 1 or 0 if the node has been visited
    int *prev = (int*)calloc(num_verticies, sizeof(int)); // array that stores the parent node
    if((seen == NULL) || (prev == NULL)) {
        printf("Error allocating the visited/prev array");
        exit(2);
    }
    seen[BEG] = 1; // first node has been visited

    for(int i = 0; i < num_verticies; i++) { // using -1 to represent null
        prev[i] = -1;
    }

    while (first != NULL){ // checks if the queue is not empty
        int current = dequeue(); // grab the popped value
        int links = (int)adjacencylist[current][1] + ((int)adjacencylist[current][2] << (1*8));
        int b = 0;
        for(size_t i = 0; i < links; i++){
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
    return prev;
}

int *get_neighbors(unsigned char *vertex){
    int length = (int)vertex[1] + ((int)vertex[2] << (1*8));
    int *neighbors = (int*)malloc(sizeof(int) * length);
    if (neighbors == NULL)
    {
        printf("Error allocating neighbors");
        exit(2);
    }
    int b = 0;
    for(size_t i = 0; i < length; i++){
        neighbors[i] = (int)vertex[3 + b++] + ((int)vertex[3 + b++] << (1*8)); // starting the 3rd index, b increments twice as much as i
    }
    return neighbors;
}

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

void printQueue(){
    Node* temp = first;
    printf("Head\n");
    while (temp != NULL){
        printf("  %d  ", temp->vertex);
        temp = temp->next;
    }
    printf("NULL\n");
}