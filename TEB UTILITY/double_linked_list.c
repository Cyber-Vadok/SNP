#include <stdio.h>
#include <stdlib.h>

struct node {
  int data;
  struct node* next;
  struct node* prev;
};

struct node* head = NULL;

void insert_at_head(int data) {
  struct node* new_node = (struct node*)malloc(sizeof(struct node));
  new_node->data = data;
  new_node->next = head;
  new_node->prev = NULL;
  if (head != NULL) {
    head->prev = new_node;
  }
  head = new_node;
}

void insert_at_tail(int data) {
  struct node* new_node = (struct node*)malloc(sizeof(struct node));
  new_node->data = data;
  new_node->next = NULL;
  if (head == NULL) {
    new_node->prev = NULL;
    head = new_node;
    return;
  }     
  struct node* temp = head;
  while (temp->next != NULL) { //esplora la lista fino alla fine
    temp = temp->next;
  }
  temp->next = new_node; //aggiungiamo quindi alla coda il nuovo nodo
  new_node->prev = temp; //lo colleghiamo anche al precedente
}

void print_list() {
  struct node* temp = head;
  while (temp != NULL) {
    printf("%d ", temp->data);
    temp = temp->next;
  }
  printf("\n");
}

int main() {    
  insert_at_head(1);
  insert_at_head(2);
  insert_at_tail(3);
  insert_at_tail(4);
  print_list();
  return 0;
}
