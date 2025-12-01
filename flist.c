#include <stddef.h> //for size_t
#include <unistd.h> //for write(), STDOUT_FILENO
#include "malloc.h" //for heap_block struct, function declarations
#include <stdint.h> //for uintptr_t
#include <string.h> //for strlen

//Macro function that extracts value of size, excluding the flag bit
#define clear_flag(x) (x & ~1)
#define METADATA_SIZE 8
#define MIN_ALLOC 32

//Head to the global free list
extern struct heap_block *head;

/*
 * Funcion Notes
 * ==> Function that writes out memory address for a pointer
 * ==> This is due to printf calling malloc internally and causing issues
 */

void stkprintf(void *mem_addr)
{ 
  if (mem_addr == NULL)
  {
    stkwrite("Stkprintf: The memory address you entered was NULL! ERROR!\n");
    return;
  }

  char buffer[20];
  char *hex = "0123456789abcdef";

  uintptr_t addr = (uintptr_t)mem_addr;

  //Loop backwards, read each byte at a time, store in buffer
  int len = 0;
  for (int i = (sizeof(void*) * 2) - 1; i >= 0; --i)
  {
      buffer[i] = hex[addr % 16];
      addr /= 16;
      len++;
  }

  buffer[0] = '0';
  buffer[1] = 'x';

  //Write out the memory address
  write(STDOUT_FILENO, buffer, len);
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
  * ==> I will be sorting the free list based on memory location
  * ==> Consequently, free block look up time will effected
  * ==> The size_and_flag field will be set prior to calling this function
  * ==> size_and_flag will be composed of user space + METADATA_SIZE
  * ==> Inserted heap_block will start at an offset of 0, NOT by + METADATA_SIZE
  */

void insert(struct heap_block *node)
{
  if (node == NULL)
  {
    stkwrite("Insert: ERROR! The node that you entered was NULL!\n");
    return;
  }

  //Set the free flag and ensure that node pointers are NULL
  node->size_and_flag |= 1;
  node->next = NULL;
  node->prev = NULL;

  //Case: The free list is empty
  if (head == NULL) 
  {
    stkwrite("Insert: Head is NULL! Making this block the head!\n");

    head = node;
    head->prev = NULL;
    head->next = NULL;

    stkwrite("Insert: This is the memory_location for the new head node -->  ");
    stkprintf((void *) node);
    stkwrite("\n");

    stkwrite("Insert: This is the memory_location for head (to make sure) -->  ");
    stkprintf((void *) head);
    stkwrite("\n");

    return;
  }

  //Case: The inserted block's memory location comes before the current heads memory location
  if ((uintptr_t)node < (uintptr_t)head)
  {
    stkwrite("Insert: Your inserted block will now be the new head!\n");

    node->prev = NULL;
    node->next = head;
    head->prev = node;
    head = node;

    stkwrite("Insert: This is the memory_location of the new head (comes before head) -->  ");
    stkprintf((void *)node);
    stkwrite("\n");

    stkwrite("Insert: This is the memory_location for head (to make sure) -->  ");
    stkprintf((void *) head);
    stkwrite("\n");

    merge_blocks(node);

    return;
  }

  struct heap_block *curr = head;

  //Loop through free list and insert
  while (curr != NULL && curr->next != NULL) 
  {
    stkwrite("Insert: Current location of the head of heap_block in free list --> ");
    stkprintf((void *)head);
    stkwrite("\n");

    stkwrite("Insert: Current location of the heap_block wanting to be inserted--> ");
    stkprintf((void *)node);
    stkwrite("\n");

    //Insert by increasing memory address
    if ((uintptr_t)curr < (uintptr_t)node && (uintptr_t)node < (uintptr_t)curr->next) 
    {
      stkwrite("Insert: I am now inserting your block (between blocks)!\n");
      node->next = curr->next;
      node->prev = curr;

      stkwrite("Insert: I am now inserting your block (between blocks)!\n");
      curr->next->prev = node;
      
      stkwrite("Insert: I am now inserting your block (between blocks)!\n");
      curr->next = node;

      stkwrite("I am going to call merge_blocks!\n");

      //Attempt to merge blocks together if they are in continuous memory
      merge_blocks(node);

      return;
    }
    curr = curr->next;
  }

  //Case: Block must be inserted at the end of the free list
  stkwrite("Insert: I am adding your block to the end of the free list!\n");
  curr->next = node;
  node->next = NULL;
  node->prev = curr;
  
  //Attempt to merge blocks
  merge_blocks(node);

  return;
}

/*
 * Function Notes
 * ==> size is composed of user requested size + METADATA_SIZE (conforming with memory_spawn)
 */

struct heap_block *remove_block(size_t size)
{
  stkwrite("Remove: Start of remove function!\n");

  if (head == NULL)
  {
    stkwrite("Remove: Head is NULL!\n");
    return NULL;
  }

  //Case: size we are are looking for is found at the head node for free linked list
  if (size <= clear_flag(head->size_and_flag) - METADATA_SIZE)
  {
    stkwrite("Remove: Great! The head of the free linked list satisfies the size requested!\n");

    struct heap_block *removed = head;

    //Before attemtping to return, check if the block can be split up to save memory
    char split = memory_save(removed, size);

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

    //Minus METADATA_SIZE since size and memory_location fields will not be accounted for full size
    removed->size_and_flag = clear_flag(removed->size_and_flag) - (METADATA_SIZE);

    stkwrite("This is the block that we removed -->  ");
    stkprintf(removed);
    stkwrite("\n");

    return removed;
  }

  struct heap_block *curr = head;

  //Loop through the free linked list and find a free block that satisfies size requested
  //I will be using a first fit approach for block finding
  while (curr != NULL && curr->next != NULL)
  {
    if (size < clear_flag(curr->size_and_flag) - METADATA_SIZE)
    {
      stkwrite("Remove: I found a block that had your requested amount of space!\n");
    
      //Before attemtping to return, check if the block can be split up to save memory
      char split = memory_save(curr, size);

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

      if (curr->next == NULL)
      {
        stkwrite("Remove: Error! curr->next is NULL!\n");
      }

      curr->next->prev = curr->prev;
      curr->next = NULL;
      curr->prev = NULL;
      curr->size_and_flag = clear_flag(curr->size_and_flag) - (METADATA_SIZE);

      stkwrite("This is the block that we removed -->  ");
      stkprintf(curr);
      stkwrite("\n");


      return curr;
    }
    curr = curr->next;
  }
  //Map more memory if not block satisfies user size request
  //Note: This is not an error! We just need to map more memory.
  stkwrite("I was not able to find a block with your requested size!\n");
  stkwrite("I will mmap for more memory now!\n");
  return NULL;
}
/*
 * Function Notes
 * ==> This function is called after remove and memory_spawn which subtract - METADATA_SIZE from size_and_flag
 * ==> Hence, size_and_flag only represents useable memory space that the user can use
 * ==> MIN_ALLOC is only 32 bytes since that is the full size for a heap_block struct
 * ==> 16 bytes get reused when heap_block "in use" since pointer fields become NULL
 */

char memory_save(struct heap_block *node, size_t requested_size)
{
  if (node == NULL)
  {
    stkwrite("Memory_save: ERROR! The node that you entered was NULL!\n");
    return 0; //for false
  }

  if (MIN_ALLOC < (clear_flag(node->size_and_flag) - requested_size))
  {
    stkwrite("Memory_save: I can split up your current chunk to save space!\n");

    //locate next struct location
    //requested_size is already user requested memory + METADATA_SIZE
    struct heap_block *saved_chunk = (struct heap_block *)((char *)node + requested_size);

    stkwrite("This is the block that we created to save space -->  ");
    stkprintf(saved_chunk);
    stkwrite("\n");

    //Add METADATA_SIZE since size_and_flag does not include it but requested_size does
    saved_chunk->size_and_flag = node->size_and_flag - requested_size + METADATA_SIZE; 

    //update the node size 
    node->size_and_flag = requested_size;

    insert(saved_chunk);

    return 1; //for true
  }

  stkwrite("Sorry this chunk can not be split!\n");
  return 0; // for false
}

/*
  * Function Notes
  * ==> merge_blocks is only called in insert
  * ==> size_and_flag includes usable user memory space (this includes pointer memory regions for prev and next)
  * ==> Inserted heap_blocks will also have + METADATA_SIZE added to ensure correct grouping for heap blocks
  * ==> In all other cases size_and_flag would only includes useable space for user
  */

void merge_blocks(struct heap_block *node)
{
  stkwrite("Merge_blocks: This is the start of merge_blocks!\n");

  if (node == NULL)
  {
    stkwrite("Merge_blocks: ERROR! You passed in NULL to merge blocks!\n");
    return;
  }
  //for debugging
  if (node->next != NULL)
  {
    stkwrite("Merge_blocks: This is the memory_address of node->next -->  ");
    stkprintf(node->next);
    stkwrite("\n");
  }
  //Check if we can merge with the next node first
  if (node->next != NULL && (node + clear_flag(node->size_and_flag)) == node->next)
  {
    stkwrite("merge_blocks: We can merge the current node with next node together to make a mega block!\n");

    //Update size of node and remove next node 
    struct heap_block *infront = node->next;

    node->size_and_flag += (clear_flag(infront->size_and_flag));
    node->next = infront->next;

    if (infront->next != NULL)
    {
      infront->next->prev = node;
    }

    //Disconnect the node
    infront->next = NULL;
    infront->prev = NULL;
    infront->size_and_flag = 0;
  }

  //for debugging
  if (node->prev != NULL){
    stkwrite("Merge_blocks: This is the memory_address of node->next -->  ");
    stkprintf(node->prev);
    stkwrite("\n");
  }

  //Check if we can merge with our prev node now
  if (node->prev != NULL && (node->prev + clear_flag(node->prev->size_and_flag)) == node)
  {
    stkwrite("merge_blocks: We can merge the current node with the prev node together to make a mega block!");

    //Update size of node and remove next node 
    struct heap_block *backup = node->prev;

    backup->size_and_flag += (clear_flag(node->size_and_flag));
    backup->next = node->next;

    if (node->next != NULL)
    {
      node->next->prev = backup;
    }

    //Disconnect the node
    node->next = NULL;
    node->prev = NULL;
    node->size_and_flag = 0;
  }

  stkwrite("Done with merging blocks!\n");
  return;
}

