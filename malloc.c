#include <sys/mman.h> //for PROT_WRITE, PROT_READ, mmap
#include <stddef.h> //for size_t, NULL, PTRDIFF_MAX
#include <unistd.h> //for getpagesize()
#include <stdio.h> //for puts
#include <stdint.h> //for PTRDIFF_MAX
#include "malloc.h" //for free_block struct, and function declarations
#include <string.h> //for memcpy
#include <limits.h> //for SIZE_MAX

//Macro function for rounding up to a multiple of 16
#define round16(x) ((x + 15) & ~15)
#define METADATA_SIZE 8

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

struct heap_block *memory_spawn(size_t requested_space)
{
  //Check first if we can find any heap_block that satisfies our requested_space
  struct heap_block *free_find = remove_block(requested_space);
  if (free_find != NULL)
  {
    stkwrite("memory_spawn: found a block from our free linked list!\n");
    return free_find;
  }

  //If no heap_block is found then mmap more memory 
  // ==> mmap rounds up the len requested to getpagesize()
  // ==> Thus I normalize the len to reflect the number of getpagesize() allocations we need
  size_t page_allocations = (requested_space + getpagesize() - 1) / getpagesize();

  stkwrite("memory_spawn: I will allocate memory for you!\n");

  void *memory_find = mmap(NULL, (page_allocations * getpagesize()), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (memory_find == MAP_FAILED)
  {
    stkwrite("memory_spawn: mmap failed! Please check logic!\n");
    return NULL;
  }

  //Set up struct in new mmap memory
  struct heap_block *mapped = memory_find;

  //Size would be total page_allocations minus METADATA_SIZE, since we reserve that space for meta data
  mapped->size_and_flag = ((page_allocations * getpagesize()) - METADATA_SIZE);
  stkwrite("This is memory_location -->    ");
  stkprintf((void *)mapped);
  stkwrite("\n");

  //memory_save will insert a new shortened struct for us (if allowed)
  char saved = memory_save(mapped, requested_space);
  if (saved) 
  {
    stkwrite("memory_spawn: You saved memory!\n");
  }
  else 
  {
    stkwrite("memory_spawn: You did not save memory!\n");
  }

  //return mmaped space + METADATA_SIZE (to not override meta data information!)
  return (struct heap_block *)((char *)mapped + 8);
}

/*
* Function Notes
* ==> My malloc will round to multiples of 16 to keep lower bit unset
* ==> My malloc will treat malloc(0) as an error and return NULL
*/

void *malloc(size_t size) 
{
  stkwrite("Malloc: You are at the start of the malloc function!\n");

  if (size == 0) 
  {
    stkwrite("ERROR: You entered size as zero! Returning NULL!\n");
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

  //Call function to get a heap_block that satisfies rounded_size
  struct heap_block *mmem = memory_spawn(rounded_size);
  if (mmem == NULL)
  {
    stkwrite("ERROR: mmap inside of memory_spawn failed!\n");
    return NULL;
  }
  stkwrite("Malloc: You are leaving the malloc function!\n");

  //memory_spawn already adds METADATA_SIZE offset so we can just return this to the user!
  return mmem;
}


void free(void *pointer)
{
  stkwrite("Free: This is the start of the free function!\n");

  if (pointer == NULL)
  {
    stkwrite("ERROR: You passed in a NULL pointer to free!\n");
    return;
  }

  stkwrite("Free: Pointer before metadata adjustment: ");
  stkprintf(pointer);
  stkwrite("\n");
  
  struct heap_block *block_point = (struct heap_block *)((char *)pointer - METADATA_SIZE);

  stkwrite("Free: Block address: ");
  stkprintf(block_point);
  stkwrite("\n");

  //block_point->size_and_flag += METADATA_SIZE;

  stkwrite("Free: This is the size_and_flag value for memory_location (digits)---> ");
  print_digits(block_point->size_and_flag);
  stkwrite("\n");

  insert(block_point);

  stkwrite("Just added your heap_block into free linked list!\n");
  stkwrite("Thank you!\n\n\n\n");

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

  //Adjust to point before metadata
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

  memset(zeroOut, 0, n * size);

  return zeroOut;

}
