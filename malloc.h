#include <stddef.h> //for size_t

/*
 * Struct Notes
 * ==> My struct will not hold the size of a previous node
 * ==> To be more memory efficent size and flag are combined
 * ==> If not in the free list, the memory region for prev and next will be reused for user use
 * ==> Total struct size: 32 bytes 
 */

struct heap_block { 
  size_t size_and_flag; //Holds block size and "free or not" flag {size_and_flag only includes pointer memory region (16 bytes)}
  size_t memory_location; //Where it is in memory to aid with coalescing {Points to the start of the struct}
  struct heap_block *prev; //Pointer to prev node
  struct heap_block *next; //Pointer to next node
}__attribute__((packed));

//Functions to help manage free list
void insert(struct heap_block *node); //Insert a heap_block into free list
struct heap_block *remove_block(size_t size); //Remove a heap_block from free list to be returned to the user
void merge_blocks(struct heap_block *node); //Merge adjacent blocks together
char memory_save(struct heap_block *node, size_t requested_size); //Splits heap_block if possible to be more memory efficent
void stkprintf(void *mem_addr); //My own custom printf for memory addresses debugging 
void stkwrite(char *message); //Wrapper around write() for easier message prints

//Functions for malloc and memory memory_location
struct heap_block *memory_spawn(size_t requested_size);

