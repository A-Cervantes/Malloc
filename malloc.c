#include <sys/mman.h> //for PROT_WRITE, PROT_READ, mmap
#include <stddef.h> //for size_t, NULL, PTRDIFF_MAX
#include "malloc.h" //for free_block struct, and function declarations

#define round16(x) ((x + 15) & ~15)

/*
* @Author: Angel Cervantes
* Purpose: To improve on the malloc I created in CS94 @ Dartmouth College
*
* Overview: This is my implementation of malloc
* ==> I use a doubly linked list to keep track of free blocks.
* Goal: I hope to make this memory efficent and also functional :)
*
* Note: If there is any place where I can improve please let me know!
* ==> I want to improve in anyway I can so feedback is welcomed! 
*/

//Head of free list
struct heap_block *head = NULL;

//TODO: Finsh logic for memory mapping and initializing
void *memory_spawn (size_t requested_space)
{
  if (head == NULL)
  {
    stkwrite("memory_spawn: This could be your first allocation!");

    void *memory_find = mmap(NULL, );
    if (memory_find == MAP_FAILED)
    {
      stkwrite("memory_spawn: mmap failed! Please check logic!\n");
      return NULL;
    }
  }
}

/*
* Function Notes
* ==> My malloc will round to multiples of 16 to keep lower bit unset
* ==> My malloc will treat malloc(0) as an error and return NULL
*/

void *malloc(size_t size) 
{
  if (size == 0) 
  {
    stkwrite("ERROR: You entered size as zero!\n");
    //Treat as an Error [May change later]
    return NULL;
  }

  //Size can't negative
  if (size < 0) 
  {
    stkwrite("ERORR: you entered a size that is negative!\n");
    return NULL;
  }
  
  //A malloc of more than PTRDIFF_MAX is an error
  if (size > PTRDIFF_MAX) 
  {
    stkwrite("ERROR: you entered a size that is too big to malloc!\n");
    return NULL;
  }

  //Round the current size to a multiple of 16
  size_t rounded_size = round16(size);

  //Call function to allocate heap memory
  void *mmem = memory_spawn(rounded_size);

}
