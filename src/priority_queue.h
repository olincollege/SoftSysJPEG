#include <stdlib.h>
#include <stdio.h>


// Data type is just an int but we can make it to whatever the runline thing actually is
typedef struct n {
    int freq;
    int data;
    struct n* next;
    struct n* left;
    struct n* right;
} Node;

Node* make_node(int freq, int data, Node* next, Node* left, Node* right); 
int insert(Node** q, int freq, int data, Node* left, Node* right);
int pop(Node** q);
Node* node_pop(Node** q);