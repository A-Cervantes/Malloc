#include <sys/mman.h> //for PROT_WRITE, PROT_READ, mmap
#include <stddef.h> //for size_t, NULL, PTRDIFF_MAX
#include <unistd.h> //for getpagesize()
#include <stdio.h> //for puts
#include <stdint.h> //for PTRDIFF_MAX
#include <string.h> //for memcpy
#include <limits.h> //for SIZE_MAX
#include "malloc.h" //for free_block struct, and function declarations

//Macro function for rounding up to a multiple of 16
#define round16(x) ((x + 15) & ~15)
#define METADATA_SIZE 8

/*
* @Author: Angel Cervantes
* Purpose: To improve on the malloc I created in CS94 @ Dartmouth College
*
* Overview: This is my implementation of malloc
* ==> I use a doubly linked list to keep track of free blocks
* Goal: I hope to make this memory efficent and also functional :)
*
* Note: If there is any place where I can improve please let me know!
* ==> I want to improve in anyway I can so feedback is welcomed!
*/

//Head of free block list
struct heap_block *head = NULL;

/*
 * Function Notes:
 * ==> Mmapped pages don't have to be continuously allocated together in memory
 * ==> requested_space is METADATA_SIZE + user space
 */

struct heap_block *memory_spawn(size_t requested_space)
{
  //Check first if we can find any free heap_block that satisfies our requested_space
  //Under the hood, remove_block calls memory_save
  struct heap_block *free_find = remove_block(requested_space);

  // If we found a block then return it
  if (free_find != NULL)
  {
    return free_find;
  }

  //If no heap_block is found then mmap more memory
  // ==> mmap rounds up the len requested to getpagesize()
  // ==> Thus, I normalize the len to reflect the number of getpagesize() allocations we need

  size_t page_allocations = (requested_space + getpagesize() - 1) / getpagesize();
  void *memory_find = mmap(NULL, (page_allocations * getpagesize()), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  if (memory_find == MAP_FAILED)
  {
    stkwrite("Memory_spawn: mmap failed! Please check logic!\n");

    return NULL;
  }

  //Set up struct in new mmap memory
  struct heap_block *mapped = memory_find;

  //Make the mapped size_and_flag equal to the number of pages we normalized
  mapped->size_and_flag = ((page_allocations * getpagesize()));

  //memory_save will insert a new shortened struct for us 
  memory_save(mapped, requested_space);

  //return mmaped space + METADATA_SIZE (to not override meta data information!)
  return (struct heap_block *)((char *)mapped + METADATA_SIZE);
}

/*
* Function Notes
* ==> My malloc will round size to a multiple of 16 to keep the rightmost bit unset
* ==> My malloc will treat malloc(0) equal to malloc(1)
*/

void *malloc(size_t size) 
{
  if (size == 0) 
  {
    stkwrite("Malloc: You entered size as zero!\n");
    size = 1;
  }

  //A malloc of more than PTRDIFF_MAX is an error
  if (size > PTRDIFF_MAX) 
  {
    stkwrite("ERROR: You entered a size that is too big for malloc!\n");

    return NULL;
  }

  //Round the current size to a multiple of 16
  //Add on METADATA_SIZE since it's constant for all heap_blocks
  size_t rounded_size = round16(size) + METADATA_SIZE;

  //Call function to get a heap_block that satisfies rounded_size
  struct heap_block *mmem = memory_spawn(rounded_size);

  if (mmem == NULL)
  {
    stkwrite("ERROR: mmap inside of memory_spawn failed!\n");

    return NULL;
  }

  //memory_spawn already adds METADATA_SIZE offset so we can just return this to the user!
  return mmem;
}

void free(void *pointer)
{
  if (pointer == NULL)
  {
    stkwrite("Free: You passed in a NULL pointer to free!\n");

    return;
  }

  //Adjust to place before metadata
  struct heap_block *block_point = (struct heap_block *)((char *)pointer - METADATA_SIZE);

  //Insert into free list
  insert(block_point);

  stkwrite("Current Free List:\n");
  struct heap_block *curr = head;
  while (curr != NULL) 
  {
    stkprintf(curr);
    stkwrite("\n");
    curr = curr->next;
  }
}

void *realloc(void *p, size_t size)
{
  if (p == NULL)
  {
    stkwrite("Realloc: Pointer p is equal to NULL!\n");

    return malloc(size);
  }

  if (p != NULL && size == 0)
  { 
    stkwrite("Realloc: Pointer p is not null and size is 0! Freeing!\n");

    free(p); 

    return NULL;
  }

  //Adjust to place before metadata
  struct heap_block *current_heap_block = (struct heap_block *)((char *)p - METADATA_SIZE);

  size_t block_size = current_heap_block->size_and_flag;

  if (block_size < size)
  {
    stkwrite("Realloc: Growing the size of your current memory block!\n");
    
    void *biggerBlock = malloc(size);

    if (biggerBlock == NULL)
    {
      return NULL;
    }

    //Memcpy old data into new block
    memcpy(biggerBlock, current_heap_block, block_size);

    //Free the old block
    free(p);

    return biggerBlock;
  }

  else
  {
    stkwrite("Realloc: Shrinking the size of your current memory block!\n");

    void *smallerBlock = malloc(size);

    if (smallerBlock == NULL)
    {
      return NULL;
    }

    //Memcpy old data into new block
    memcpy(smallerBlock, current_heap_block, size);

    //Free the old block
    free(p);

    return smallerBlock;
  }

  //This should never reach
  stkwrite("Realloc: ERROR! This should never reach!");
  return NULL;
}

void *calloc(size_t n, size_t size)
{
  if (n == 0 || size == 0)
  {
    //Calloc must return a "unique pointer" that can be passed to free
    return malloc(1);
  }

  if ((n * size) > SIZE_MAX)
  {
    stkwrite("Calloc: Your input surpasses the maximum amount!\n");

    return NULL;
  }

  //Malloc memory and then zero it out
  void *zeroOut = malloc(n * size);
  
  if (zeroOut == NULL)
  {
    stkwrite("Calloc: ERROR! Malloc failed!\n");
    
    return NULL;
  }

  memset(zeroOut, 0, round16(n * size));

  return zeroOut;
}
