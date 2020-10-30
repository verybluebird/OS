#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node {
    char *value;
    struct Node *next;
} Node;

int push_Node(Node **head, char *value) {
    Node *tmp = (Node *) malloc(sizeof(Node));

    if (!tmp) {
        perror("No memory allocated, can't push a node");
        return 1;
    }

    tmp->value = value;
    tmp->next = (*head);
    (*head) = tmp;
    return 0;
}


char* pop(Node **head) {
    Node* prev = NULL;
    char* val;
    if (head == NULL) {
        exit(-1);
    }
    prev = (*head);
    val = prev->value;
    (*head) = (*head)->next;
    free(prev);
    return val;
}

void deleteList(Node **head) {
    while ((*head)->next) {
        pop(head);
        *head = (*head)->next;
    }
    free(*head);
}

void print_list(Node *head) {
    if (head) {
        print_list(head->next);
        printf("%s", head->value);
    }
}

Node *read() {
    Node *head = (Node *) NULL;
    char temp_str[200];
    int a = 1;
    while (a != 0) {
        char *prove = fgets(temp_str, 200, stdin);

        if (!prove) {
            perror("Reading error!");
            return NULL;
        }

        if (temp_str[0] == '.') a = 0;


        char *value = strdup(temp_str);

        if (!value) {
            perror("Memrore allocated");
            return NULL;
        }

        if (push_Node(&head, value) == 1) return NULL;
    }
    return head;
}

int main() {
    Node *lines = read();
    if (lines) {
        print_list(lines);
    }
    deleteList(&lines);
    return 0;
}
