#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "priority_queue.h"

Node* make_tree(Node* q){
    int count;
    int i;
    while(q->next->next != NULL){
        count = q->next->freq + q->next->next->freq;
        insert(&q, count, 1000+i, node_pop(&q), node_pop(&q));
        i++;
    }
    Node* root = malloc(sizeof(Node));
    //count = q->next->freq + q->next->next->freq;
    root = make_node(count, 1000+i, NULL, node_pop(&q), node_pop(&q));
    return root;
}

int is_leaf(Node* n){
    if (n->left == NULL && n->right==NULL) {return 1;} return 0;
}

void make_queue(int* priority, Node ** root){
    for (int i = 0; i < 256; i++){
        if (priority[i] != 0) {insert(root, priority[i], i, NULL, NULL);}
    }
}

void count_freq(int* data, int* frequencies){
    // change bound of i based on how large actual data is
    for (int i = 0; i < 20; i++){
        if (data[i] >= 0 && data[i] < 256){
            frequencies[data[i]] += 1;
        } else{
            printf("Data %i out of range 0-255\n", data[i]);
            exit(0);
        }
    }    
}

void free_tree(Node* root){
    Node* tmp = root;
    if(root->left!=NULL) { free_tree(root->left); }
    if(root->right!=NULL) { free_tree(root->right); }
    free(root);
}

void make_table(int* code_table, Node* root, int code){
    if (root==NULL) { return; }
    if (root->data <= 255){
        code_table[root->data] = code;
        return;
    }

    make_table(code_table, root->left, code*10);
    make_table(code_table, root->right, code*10+1);
}

int main(int argc, char const *argv[])
{
    // dummy data to test everything
    int data[20] = {55,55,55,0,0,0,0,0,0,0,23,140,255,255,255,255,0,0,35,204};
    int frequencies[256];
    memset(frequencies, 0, sizeof(frequencies));
    count_freq(data, frequencies);
    Node *q = make_node(0, 999, NULL, NULL, NULL);
    make_queue(frequencies, &q);
    Node* root = malloc(sizeof(Node));
    root = make_tree(q);
    
    int table[256]; int path;
    memset(table, 0, sizeof(table));
    make_table(table, root, path);
    for (int i = 0; i<256; i++){
        if (table[i]!=0) { printf("%i, %i\n", i, table[i]); }
    }
    //sanity check
    printf("%d\n", root->left->left->data);
}
