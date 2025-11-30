#include <sys/mman.h> //for PROT_WRITE, PROT_READ, mmap
#include <stddef.h> //for size_t, NULL, PTRDIFF_MAX
#include <unitstd.h> //for getpagesize()
#include <stdio.h> //for puts
#include "malloc.h" //for free_block struct, and function declarations

//Macro function for rounding up to a multiple of 16
#define round16(x) ((x + 15) & ~15)
#define METADATA_SIZE 16

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
 * Function Notes:
 * ==> I will just allocate mmap space in this function
 * ==> Splitting and inserting will happen in the main malloc function
 * ==> requested_space includes METADATA_SIZE already
 * ==> Mmapped pages don't have to be continuously allocated together
 * ==> Thus, I can just focus on fusing those who are
 */

struct heap_block *memory_spawn (size_t requested_space)
{
  //Check first if we can find any heap_block that satisfies our requested_space
  struct heap_block free_find = remove(requested_space);
  if (free_find != NULL)
  {
    stkwrite("memory_spawn: found a block from our free linked list!\n");
    return free_find;
  }

  //If no heap_block is found then mmap more memory 
  // ==> mmap rounds up the len requested to getpagesize()
  // ==> Thus I normalize the len to reflect the number of getpagesize() allocations we need
  size_t page_allocations = (requested_space + getpagesize() - 1) / getpagesize();

  stkwrite("memory_spawn: I will allocated memory for you!\n");
  puts(page_allocations + '0');
  puts('\n');

  void *memory_find = mmap(NULL, page_allocations, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
  if (memory_find == MAP_FAILED)
  {
    stkwrite("memory_spawn: mmap failed! Please check logic!\n");
    return NULL;
  }

  //Set up struct in new mmap memory
  struct heap_block *mapped = memory_find;
  mapped->size_and_flag = requested_space; 
  mapped->memory_location = memory_find;

  //memory_save will insert a new shortened struct for us (if allowed)
  char saved = memory_save(mapped, requested_space);
  if (char) 
  {
    stkwrite("memory_spawn: You saved memory!\n");
  }
  else 
  {
    stkwrite("memory_spawn: You did not save memory!\n");
  }

  //return mmaped space + METADATA_SIZE (to not override meta data information!)
  return memory_find + METADATA_SIZE;
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
  //Add on METADATA_SIZE since it's constant for all heap_block
  size_t rounded_size = round16(size) + METADATA_SIZE;

  //Call function to allocate heap memory
  struct heap_block *mmem = memory_spawn(rounded_size);
  if (mmem == NULL)
  {
    stkwrite("ERROR: mmap inside of memory_spawn failed!\n");
    return NULL;
  }

  //memory_spawn already adds METADATA_SIZE offset so we can just return this to the user!
  return mmem;
}
