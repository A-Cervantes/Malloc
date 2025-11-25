#include <stddef.h> //for size_t

/*
 * Struct Notes
 * ==> My struct will not hold the size of a previous node
 * ==> To be more memory efficent size and flag are combined
 * ==> If not in the free list, the memory region for prev and next will be reused for user use
 */

struct heap_block { 
  size_t size_and_flag; //Holds block size and "free or not" flag
  size_t memory_location; //Where it is in memory (To aid with coalescing)
  struct free_block *prev; //Pointer to prev node
  struct free_block *next; //Pointer to next node
}__attribute__((packed));

//Functions to help manage free list
void insert(struct heap_block *node); //Insert a heap_block into free list
struct heap_block *remove(size_t size); //Remove a heap_block from free list to be returned to the user
void merge_blocks(struct heap_block *node); //Merge adjacent blocks together
void stkprintf(void *mem_addr); //My own custom printf for memory addresses debugging 
void stkwrite(char *message); //Wrapper around write() for easier message prints
