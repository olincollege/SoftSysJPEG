#include <stdlib.h>
#include <stdio.h>

// Data type is just an int but we can make it to whatever the runline thing actually is
typedef struct n {
    int priority;
    int data;
    struct n* next;
} Node;

Node* make_node(int priority, int data, Node* next) {
    Node* ret = malloc(sizeof(Node));
    ret->priority = priority;
    ret->data = data;
    ret->next = next;
    return ret;
}

int insert(Node** q, int priority, int data) {
    if ((*q)->priority > priority) {
        Node* tmp = make_node((*q)->priority, (*q)->data, (*q)->next);
        *q = make_node(priority, data, tmp);
        return 0;
    } else if ((*q)->next == NULL) {
        (*q)->next = make_node(priority, data, NULL);
        return 0;
    } 
    return insert(&((*q)->next), priority, data);
}

int pop(Node** q) {
    Node* tmp = (*q);
    int ret = tmp->data;
    (*q) = tmp->next;
    free(tmp);
    return ret;
}

int main(int argc, char const* argv[]) {
    Node * q = make_node(0,50, NULL);
    insert(&q, 1, 1010);
    insert(&q, 3, 312);
    insert(&q, 5, 123);
    insert(&q, 2, 345345);

    while(q->next != NULL) {
        printf("%i \n", pop(&q));
    }
    printf("%i \n", pop(&q));
    return 0;
}
