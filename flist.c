#include <stddef.h> //for size_t
#include <unistd.h> //for write(), STDOUT_FILENO
#include "malloc.h" //for heap_block struct, function declarations

//Macro function that extracts value of size, excluding the flag bit
#define clear_flag(x) (x & ~1)

//Head to the global free list
extern struct heap_block *head;

/*
 * Funcion Notes
 * ==> Function that writes out memory address for a pointer
 * ==> This is due to printf calling malloc internally and causing issues
 */

void stkprintf(void *mem_addr)
{ 
  char buffer[20];
  char *hex = "0123456789abcdef";

  //Loop backwards, read each byte at a time, store in buffer
  int len = 0;
  for (int i = sizeof(void*) * 2 - 1; i >= 0; --i)
  {
      buffer[i] = hex[(uintptr_t)address % 16];
      address = (void*)((uintptr_t)address / 16);
      len++;
  }

  buffer[0] = '0';
  buffer[1] = 'x';

  //Write out the memory address
  write(STDOUT_FILENO, buffer, len + 2);
}

/*
 * Function Notes
 * ==> Custom printf for regular messages
 */

void stkwrite(char *message)
{
  write(STDOUT_FILENO, message, strlen(message));
}

 /*
  * Function Notes
  * ==> I will be sorting free list based on memory location
  * ==> This will ensure that coalescing is easier
  * ==> The "free" flag will be set for node prior to function being called
  * ==> Consequently, free block look up time will effected
  */

void insert(struct heap_block *node) 
{
  //Case: The free list is empty
  //Note: Size and memory field will be set prior to calling this
  if (head == NULL) 
  {
    stkwrite("Insert: Head is NULL! Making this block the head!\n")
    head = node;
    head->prev = NULL;
    head->next = NULL;
  }

  //Case: The inserted block's memory location comes before the current heads memory location
  if (node->memory_location < head->memory_location)
  {
    stkwrite("Insert: Your inserted block will now be the new head!\n");

    head->prev = node;
    node->prev = NULL;
    node->next = head;
    head = node;

    return;
  }


  struct heap_block *curr = head;

  //Loop through free list and insert
  while (curr->next != NULL) 
  {
    stkwrite("Current location of the heap_block in free list --> ");
    stkprintf(head->memory_location);
    stkwrite("\n");

    stkwrite("Current location of the heap_block wanting to be inserted--> ");
    stkprintf(node->memory_location);
    stkwrite("\n");

    //Insert by increasing  memory address
    if (curr->memory_location < node->memory_location) 
    {
      stkwrite("Insert: I am now inserting your block!\n");
      node->next = curr->next;
      node->prev = curr;
      curr->next = curr;

      //Attempt to merge blocks together if they are in continuous memory
      merge_blocks(node);

      return;
    }
    curr = curr->next;
  }

  //Case: Block must be inserted at the end of the free list
  stkwrite("Insert: I am adding your block to the end of the free list!\n")
  curr->next = node;
  node->next = NULL;
  node->prev = curr;
  
  //Attempt to merge blocks
  merge_blocks(node);

  return;
}

struct heap_block *remove(size_t size)
{
  stkwrite("Remove: Start of remove function!\n");

  //Case: size we are looking for matches size of the head node
  if (size == clear_flag(head->size_and_flag))
  {
    stkwrite("Remove: Great the head free block matches the size requested!\n");
    struct heap_block *removed = head;
    head = head->next;
    removed->next = NULL;
    
    return removed;
  }

}

void merge_blocks(struct heap_block *node)
{

}
