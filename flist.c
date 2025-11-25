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

  //Case: size we are are looking for is found at the head node for free linked list
  if (size <= clear_flag(head->size_and_flag))
  {
    stkwrite("Remove: Great! The head of the free linked list matches the size requested!\n");

    struct heap_block *removed = head;

    //Before attemtping to return, check if the block can be split up to save memory
    char split = memory_save(removed);

    if (split)
    {
      stkwrite("Remove: We were able to split the heap_block!\n");
    }
    else
    {
      stkwrite("Remove: We were not able to split the heap_block\n");
    }

    //Update pointers and clear free flag
    head = head->next;
    removed->next = NULL;
    removed->size_and_flag = clear_flag(size_and_flag);

    return removed;
  }

  struct heap_block *curr = head;

  //Loop through the free linked list and find a free block that satisfies size requested
  //I will be using a first fit approach for block finding
  while (curr->next != NULL)
  {
    if (size < clear_flag(curr->size_and_flag))
    {
      stkwrite("Remove: I found a block that had your requested amount of space!\n");
    
      //Before attemtping to return, check if the block can be split up to save memory
      char split = memory_save(curr);

      if (split)
      {
        stkwrite("Remove: We were able to split heap_block!\n");
      }
      else
      {
        stkwrite("Remove: We were not able to split heap_block\n");
      }
      //Update pointers a clear free flag
      curr->prev->next = curr->next;
      curr->next->prev = curr->prev;
      curr->next = NULL;
      curr->prev = NULL;
      removed->size_and_flag = clear_flag(size_and_flag);

      return curr;
    }
  }
  //Map more memory if not block satisfies user size request
  //Note: This is not an error! We just need to map more memory.
  stkwrite("I was not able to find a block with your requested size!\n");
  stkwrite("I will mmap for more memory now!\n");
  return NULL;
}

void merge_blocks(struct heap_block *node)
{

}
