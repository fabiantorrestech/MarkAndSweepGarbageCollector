#include "memlib.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mm.h"

struct memory_region{
  size_t * start;
  size_t * end;
};

struct memory_region global_mem;
struct memory_region stack_mem;

void walk_region_and_mark(void* start, void* end);

// PROVIDED BY US (DO NOT CHANGE)
// ------------------------------
// grabbing the address and size of the global memory region from proc 
void init_global_range(){
  int next = 0;
  char file[100];
  char * line=NULL;
  size_t n=0;
  size_t read_bytes=0;
  size_t start, end;


  sprintf(file, "/proc/%d/maps", getpid());
  FILE * mapfile  = fopen(file, "r");
  if (mapfile==NULL){
    perror("opening maps file failed\n");
    exit(-1);
  }

  int counter=0;
  while ((read_bytes = getline(&line, &n, mapfile)) != -1) {
    // .data of hw4 executable
    if (strstr(line, "hw4") != NULL && strstr(line,"rw-p")){
      sscanf(line, "%lx-%lx", &start, &end);
      global_mem.start = (void*) start;
      global_mem.end = (void*) end;
      break;
    }

  }
  fclose(mapfile);
}

// marking related operations

int is_marked(unsigned int * chunk) {
  return ((*chunk) & 0x2) > 0;
}

void mark(unsigned int * chunk) {
  //fprintf(stderr, "Marked!\n");
  (*chunk)|=0x2;
}

void clear_mark(unsigned int * chunk) {
  (*chunk)&=(~0x2);
}

// chunk related operations 
#define chunk_size(c)  ((*((unsigned int *)c))& ~(unsigned int)7 ) 
void* next_chunk(void* c) { 
  if(chunk_size(c) == 0) {
    fprintf(stderr,"Panic, chunk is of zero size.\n");
  }
  if((c+chunk_size(c)) < mem_heap_hi())
    return ((void*)c+chunk_size(c));
  else 
    return 0;
}


//  checks if a chunk is ALLOCATED.
// - give it a chunk in form of (void *c)
// - does this by checking if Chunk's header's 1st bit is 1.
int in_use(void *c) { 
  return *(unsigned int *)(c) & 0x1;
}

// FOR YOU TO IMPLEMENT
// --------------------
// the actual collection code
void sweep() {
  void* chunk = mem_heap_lo();
  // TODO

  //fprintf(stderr, "sweeping for chunk %p..\n", chunk);
  // The rule:
  // - free blocks that are UNMARKED and ALLOCATED
  // - if there is a valid chunk, it will have a non-zero chunk size.
  while(chunk_size(chunk))
  {
      // if marked, unmark it.
      if(is_marked(chunk))
      {
        clear_mark(chunk);
      }
      else if(in_use(chunk))
      {
        mm_free(chunk+4);
      }
      
    chunk = next_chunk(chunk);
  }
}

// determine if what "looks" like a pointer actually points to an 
// in use block in the heap. if so, return a pointer to its header 
void * is_pointer(void* ptr) {
  // Here's a helpful print statement that shows some available memory operations and working with pointers
  //printf("checking for pointeryness of %p between %p and %p\n",ptr, mem_heap_lo(), mem_heap_hi());
  
  // TODO
  // 1) if its in the heap...
  // 1a) NOT in heap, return NULL.
  // 1b) YES in heap, where?
  // 1b-i) if in heap, return header to the chunk that contains the given ptr.

  // base case: the given ptr is out of heap bounds, no way it can be here.
  void* curr_heap_chunk = mem_heap_lo();
  void* bottom_heap = mem_heap_hi();
  if(ptr >= bottom_heap || ptr < curr_heap_chunk)
  {
    //fprintf(stderr, "NOT IN THE HEAP!\n");
    return NULL;
  }

  // general algo: it must be in the heap at this point.
  while(chunk_size(curr_heap_chunk))
  {
    // it is somewhere between 2 chunks.
    // ptr's addr is found between current chunk AND current_chunk's footer.
    if(ptr < curr_heap_chunk + chunk_size(curr_heap_chunk) && ptr >= curr_heap_chunk)
    {
      if(in_use(curr_heap_chunk))
      {
         return curr_heap_chunk;
      }
    }
    curr_heap_chunk = next_chunk(curr_heap_chunk);
  }

  return NULL;
}

// walk through memory specified by function arguments and mark
// each chunk
void walk_region_and_mark(void* start, void* end) {
  //fprintf(stderr, "walking %p - %p\n", start, end);

  // TODO
  while(start < end-8)
  {
    // the -8 is to skip over the footer.
    // curr_ptr = header ptr to a chunk in either stack or global (initially)
    //            (on recursion, it will be within the heap)
    // - if isptr RETURNS NULL, not in heap
    // - if is_pointer DOES NOT return NULL, is in heap

    //void* curr_ptr = is_pointer(*(size_t*) start);
    //void* curr_ptr = is_pointer((void *)*(size_t *)start);
    void* curr_ptr = is_pointer((void*)*(size_t *)start);

    // valid curr ptr.
    if(curr_ptr)
    {
      // if our curr_ptr is in the heap, unmarked, but in use, mark it!
      if(is_marked(curr_ptr) == 0)
      {
        if(in_use(curr_ptr) == 1)
        {
          mark(curr_ptr);
          // then recursively find if theres more ptrs to other
          // places in heap via recursion...
          walk_region_and_mark(curr_ptr+4, curr_ptr+chunk_size(curr_ptr));
        }
      }
    }

    // traverse to the next chunk over in the given stack, heap, or global.
    start = start + 4;
  }

  return;
}

// PROVIDED BY US (DO NOT CHANGE)
// ------------------------------
// standard initialization 

void init_gc() {
  size_t stack_var;
  init_global_range();
  // since the heap grows down, the end is found first
  stack_mem.end=(size_t *)&stack_var;
}

void gc() {
  size_t stack_var;
  // grows down, so start is a lower address
  stack_mem.start=(size_t *)&stack_var;
  // walk memory regions
  walk_region_and_mark(global_mem.start,global_mem.end);
  walk_region_and_mark(stack_mem.start,stack_mem.end);
  sweep();
}
