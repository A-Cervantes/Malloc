#include "malloc.h" //for heap_block struct, function declarations
#include <stddef.h> //for size_t
#include <unistd.h> //for write(), STDOUT_FILENO

extern struct heap_block *head;

void stkprintf(void *mem_addr) { 
  char buffer[20];
  char *hex = "0123456789abcdef";

  //Loop backwards, read each byte at a time, store in buffer
  int len = 0;
  for (int i = sizeof(void*) * 2 - 1; i >= 0; --i) {
      buffer[i] = hex[(uintptr_t)address % 16];
      address = (void*)((uintptr_t)address / 16);
      len++;
  }

  buffer[0] = '0';
  buffer[1] = 'x';

  //Write out the memory address
  write(STDOUT_FILENO, buffer, len + 2);
}

void stkwrite(char *message) {
  write(STDOUT_FILENO, message, strlen(message));
}

 /*
  * Function Notes  
  * ==> I will be sorting free list based on memory location
  * ==> This will ensure that coalescing is easier
  * ==> Consequently, free block look up time will effected
  */

void insert(struct heap_block *node) { 
  //Case: The free list is empty
  //Note: Size and memory field will be set prior to calling this
  if (head == NULL) {
    head = node;
    head->prev = NULL;
    head->next = NULL;
  }

  struct heap_block *curr = head;

  //Loop through free list and insert
  while (curr->next != NULL && head->memory_location) {
    stkwrite("Current location of the heap_block in free list --> ");
    stkprintf(head->memory_location);
    stkwrite("\n");

    stkwrite("Current location of the heap_block wanting to be inserted--> ");
    stkprintf(node->memory_location);
    stkwrite("\n");

    if (node->memory_location > head->memory_location) {
      stkwrite("Insert: I am now inserting your block!\n");
      node->next = curr->next;
      node->prev = curr;
      curr->next = curr;

      //Attempt to merge blocks together if they are in continuous memory
      merge_blocks(node);

      return;
    }
    head = head->next;
  }

  //Case: Block must be inserted at the end of the free list
  stkwrite("Insert: I am adding your block to the end of the free list!")
  curr->next = node;
  node->next = NULL;
  node->prev = curr;
  
  //Attempt to merge blocks
  merge_blocks(node);

  return;
}
