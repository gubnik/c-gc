#ifndef C_GC
#define C_GC

#include <stdbool.h>
#include <stddef.h>

typedef struct gc gc;
typedef struct gc_obj gc_obj;

typedef void* (*allocator_t)(size_t);

/*
 * Garbage collector object
 *
 * Structure that containst necessary information to manage
 * the object via garbage collector.
 * Acts as a linked list.
 *
 * next    - next object
 * memarea - allocated memory
 * marked  - is object marked for deletion
 *
*/
typedef struct gc_obj
{
  gc_obj *  next;
  void * memarea;
  bool marked;
}
gc_obj;

/*
 * Garbage collector 
 *
 * Wrapper struct that implements a garbage collector 
 *
 * root - root of a linked list consisting of garbage collector objects
 *
*/
typedef struct gc 
{
  gc_obj * root;
}
gc;

/*
 * Allocates a new gc_obj on the heap and initializes its fields.
 *
 * Params:
 * allocsize - amount of memory to allocate
 *
 * Return:
 * Allocated an object
*/
gc_obj * cgc_gcobj_new (size_t allocsize);

/*
 * Initializes garbage collector 
 *
 * Returns:
 * garbage collector pointer
*/
gc * cgc_gc_init ();

/*
 * Allocates memory via garbage collector.
 * When this function is called, allocated memory is known to GC.
 *
 * Params:
 * garcol - garbage collector 
 * allocsize - amount of memory to allocate 
*/
void * cgc_gc_allocate (gc * garcol, size_t allocsize, allocator_t allocator);

/* TODO - implement a better marking mechanism
 *
 * Marks objects for deletion.
 *
 * Params:
 * garcol - garbage collector
*/
void cgc_gc_mark (gc * garcol);

/*
 * Destroys marked objects 
 *
 * Params:
 * garcol - garbage collector
*/
void cgc_gc_sweep (gc * garcol);

/*
 * Invokes mark & sweep
 *
 * Params:
 * garcol - garbage collector
*/
void cgc_gc_collect (gc * garcol);

/*
 * Destroys the garbage collector
 *
 * Params:
 * garcol - garbage collector
*/
void cgc_gc_destroy (gc * garcol);

#endif // !C_GC
