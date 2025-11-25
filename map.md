0x20 -----> 0x60 -----> 0x120
            ++++
          size == 0x40
          if(mem_location + clear_flag(size) == next->mem_location)
          {
          node->size += next->size;
          remove next node (update pointer and all that)
          clear out its memory with memset 
return(struct heap_block *) (mapper + 16);
          }
          if(mem_location - clear_flag(size) ==  prev->mem_location)
          {
          //Update the size of prev
          prev->size += node->size; 
          }
