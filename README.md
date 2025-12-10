# Malloc




#Debugging 
Throughout the process of the creating Malloc I relied a lot on using my custom print functions to debug!
I also did a lot of manual calculation using pen and paper for metadata calculations and also visualizing the logic in a simpler manner.
I also use gdb which is a vital tool that can be used to view memory location of variables, values in registers, and return value of functions.

gdb --args python3
set environment LD_PRELOAD=/home/stockzy/Projects/C/memory/malloc.so
r
