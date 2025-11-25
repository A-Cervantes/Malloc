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

/*
* Function Notes
* ==> My malloc will round to multiples of 16 to keep lower bit unset
* ==> My malloc will treat malloc(0) as an error and return NULL
*/

void *malloc(size_t size) 
{
  if (size == 0) 
  {
    //Treat as an Error [May change later]
    return NULL;
  }

  //Size can't negative
  if (size < 0) 
  {
    return NULL;
  }
  
  //A malloc of more than PTRDIFF_MAX is an error
  if (size > PTRDIFF_MAX) 
  {
    return NULL;
  }

  //Round the current size to a multiple of 16
  size_t rounded_size = round16(size);

  //Call function to allocate heap memory

}
