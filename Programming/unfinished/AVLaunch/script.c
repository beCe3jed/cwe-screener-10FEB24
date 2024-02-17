#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define DELIMITER1 ','
#define DELIMITER2 '\n'

typedef struct Node {
    int key;
    char val;
    struct Node* left; 
    struct Node* right;
    struct Node* parent;
} Node;
Node *root = NULL;

void createNode(int key, char val);



int main(void) {
    // USING A HYBRID APPROACH THAT BALANCES USING A BUFFER AND OPERATING WITH DATA
    FILE *fp = NULL;
    const char delim1 = DELIMITER1;
    const char delim2 = DELIMITER2;
    int num_bytes = 0, total_bytes = 0, buffer_multiplier = 1;
    
    char *old_address = NULL;
    char *dump = (char*)malloc(sizeof(char) * BUFFER_SIZE + 1);
    if (dump == NULL) {
        printf("Error allocating dump");
        exit(1);
    }
    char *buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    if (buffer == NULL) {
        printf("Error allocating buffer");
        exit(1);
    }
    if ((fp = fopen("nodes.txt", "r")) == NULL){
        printf("Error opening file");
        exit(1);
    }
    
    
    while ((feof(fp) == 0) && (num_bytes = fread(buffer, 1, BUFFER_SIZE, fp)) != 0){ // Read line at a time: getline() IS THIS VULNERABLE TO MEMORY LEAKS????
        printf("num_bytes: %d\n", num_bytes);
        dump = strncpy(&dump[total_bytes], buffer, num_bytes);
        total_bytes += num_bytes;
        dump[total_bytes] = '\0';
        printf("---dump---\n%s\n", dump);

        if (num_bytes == BUFFER_SIZE){ // increase size of the dump
            buffer_multiplier++;
            old_address = dump;
            dump = (char*)realloc(dump, (BUFFER_SIZE * buffer_multiplier) + 1);
            if (dump == NULL) {
                printf("Error reallocating dump");
                exit(1);
            }
            if (old_address != dump) free(old_address);
        }
    }
    printf("dump: \n %s", dump);


    if(buffer) free(buffer);


    


    if (buffer) free(buffer);
    // close file
    if (fp) fclose(fp);
    return 0;
}

void createNode(int key, char val){
    // GENERATE NODE
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL){
        printf("Error allocating newNode");
        exit(1);
    }
    newNode->key = key;
    newNode->val = val;
    newNode->left = newNode->right = newNode->parent = NULL;

    Node* temp = root;

    // INSERT NODE INTO THE BINARY TREE
    if (root == NULL){
        root = newNode;
        return;
    }
    while (temp != NULL) {
        if (newNode->key < temp->key) {
            if(temp->left == NULL) {
                newNode->parent = temp;
                temp->left = newNode;
                return;
            }
            temp = temp->left;
        }
        else {
            if(temp->right == NULL) {
                newNode->parent = temp;
                temp->right = newNode;
                return;
            }
            temp = temp->right;
        }
    }
}