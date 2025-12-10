# Malloc

## How to Run
```bash
1. git clone <repo>
2. make ; make test
3. Run with LD_PRELOAD:

   LD_PRELOAD=path/to/malloc.so <Unix command>
   # Example:
   #   LD_PRELOAD=./malloc.so ls
   #   LD_PRELOAD=./malloc.so strings
   #   LD_PRELOAD=./malloc.so cat

4. You can also create your own programs that call or use malloc and pass the executable
   as the argument after LD_PRELOAD. Run ./mtest to use my simple test!

5. make clean ; clear

6. Repeat and have fun! :)!
```

## Purpose
I wrote this for fun and also to prove to myself that I could implement `malloc` using the minimal amount of overhead possible!

## Important Note
I tried to limit the number of comments that print while running my malloc with Unix commands, so the program output stays clear!

Although I cut most comments out, I still left a few to show the movement of the program and that it *actually works*.  
It may be difficult to spot the allocator’s output for `ls`, but you shouldn't have to scroll up in the terminal to see it.

Here is example output from mine:
```
Realloc: Pointer p is equal to NULL!
Remove: Head is NULL!
Memory_save: I can split up your current chunk to save space!
Insert: Head is NULL! Making this block the head!
Remove: Great! The head of the free list satisfies the size requested!
Memory_save: I can split up your current chunk to save space!
Insert: I am adding your block to the end of the free list!
flist.c  flist.o  Makefile  malloc.c  malloc.h  malloc.o  malloc.so  malloc_test.c  mtest  README.md <-- {OUTPUT FOR LS}
Insert: Head is NULL! Making this block the head!
Current Free List:
0x007f9929343088
Free: You passed in a NULL pointer to free!
Insert: Your inserted block will now be the new head!
Current Free List:
0x007f9929342148
0x007f9929343088
```

## Logic and Thought Process
I use a doubly linked list as my data structure of choice for maintaining the free block list.

This simplified adding and removing blocks from the free list, but as a result, the time complexity for inserting and removing is O(n). I sort blocks based on increasing memory address. 

My `size_and_flag` struct field uses the rightmost bit as the free flag, which will always be unset unless I manually flip it when putting the block into the free list-something guaranteed by rounding sizes to a multiple of 16 (8 could have worked as well).

I repurposed the `next` and `prev` fields to be user memory space when a heap_block was in use by the user, since these fields would be NULL when not in the free list. In the free list, these fields are populated and actually point to other nodes. This was one trick I used to be as memory-efficient as I possibly could be.

You might wonder why I return `struct heap_block + METADATA_SIZE` instead of the start of the struct. This is to avoid the user overwriting the actual `size_and_flag` field, which unlike the `prev` and `next` fields, will never be repurposed and should only change when saving memory further.

## Debugging
Throughout the process of creating my LD_PRELOAD-able malloc, I relied heavily on my custom print functions to debug!

I also did a lot of manual calculations using pen and paper for metadata calculations and for visualizing the logic in a simpler way.

I also used gdb, which is a vital tool for viewing memory locations of variables, register values, and function return values.

These are the steps I took to set up the environment for GDB:
```
gdb --args ls
(gdb) set environment LD_PRELOAD=/path/to/malloc.so
(gdb) r
```

There are many helpful resources out there to learn all the gdb commands, so I recommend learning more about it!

This may be a helpful website to start off with: https://www.gdbtutorial.com/gdb_commands

## Weird Bug
You might ask yourself: Why didn't he use `printf` in his code?

The reason I didn’t is because `printf` under the hood will call `malloc` since it’s buffered!

This is a very tricky bug to figure out and will cause your program to break and not run at all.

My advice is to stick to using the `write` syscall and also `snprintf`, since neither of those call `malloc`!

You can also use the functions I created in my code to make life easier!

## Final Thoughts
This was a lot of fun to create, but it also required a lot of time and careful mathematical precision!

I encourage everyone to create their own malloc to truly appreciate the wonderful functions we as developers have access to, and that we generally don’t have to worry about failing or not working!

If you have any questions feel free to connect with me on LinkedIn and message me!

