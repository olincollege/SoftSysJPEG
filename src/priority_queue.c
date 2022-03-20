#include <stdlib.h>
#include <stdio.h>
#include "priority_queue.h"


Node* make_node(int freq, int data, Node* next, Node* left, Node* right) {
    Node* ret = malloc(sizeof(Node));
    ret->freq = freq;
    ret->data = data;
    ret->next = next;
    ret->left = left;
    ret->right = right;
    return ret;
}

int insert(Node** q, int freq, int data, Node* left, Node* right) {
    if ((*q)->freq > freq) {
        Node* tmp = make_node((*q)->freq, (*q)->data, (*q)->next, left, right);
        *q = make_node(freq, data, tmp, left, right);
        return 0;
    } else if ((*q)->next == NULL) {
        (*q)->next = make_node(freq, data, NULL, left, right);
        return 0;
    } 
    return insert(&((*q)->next), freq, data, left, right);
}

int pop(Node** q) {
    Node* tmp = (*q);
    int ret = tmp->data;
    (*q) = tmp->next;
    free(tmp);
    return ret;
}

Node* node_pop(Node** q){
    Node* ret = malloc(sizeof(Node));
    Node* tmp = (*q);
    ret->freq = tmp->freq;
    ret->data = tmp->data;
    ret->next = tmp->next;
    ret->left = tmp->left;
    ret->right = tmp->right;
    (*q) = ret->next;
    free(tmp);
    return ret;
}

int main(int argc, char const* argv[]) {
    Node * q = make_node(14,50, NULL, NULL, NULL);
    Node* tmp = malloc(sizeof(Node));
    while(q->next != NULL){
        printf("%i\n", pop(&q));
        node_pop(&q);
        printf("%i\n", tmp->data);
    }
    node_pop(&q);
    printf("%i\n", tmp->data);
    free(tmp);
}
