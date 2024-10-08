/*
        C-GC, a basic C garbage collector
        Copyright (C) 2024  Nikolay Gubankov (nikgub)

        This program is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
 * Garbage Collector 
 *
 * Implements a basic mark-and-sweep garbage collector
 * Includes a support for multithreading
 * 
 * TODO: Marking algorithm is currently WIP and marks everything for removal.
 * TODO: Implement a reference counting algorithm AND/OR stack approach
 *
 * For macros, see marcos.h
 */

#ifndef CGC_GC_H
#define CGC_GC_H

#include <pthread.h>
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
 * @mutex   - pthread mutex
 * @next    - next object
 * @memarea - allocated memory
 * @marked  - is object marked for deletion
 *
*/
typedef struct gc_obj
{
  pthread_mutex_t mutex;
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
 * @mutex - pthread mutex
 * @root  - root of a linked list consisting of garbage collector objects
 *
*/
typedef struct gc 
{
  pthread_mutex_t mutex;
  gc_obj * root;
}
gc;

#ifdef __cplusplus
extern "C" {
#endif

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
gc * cgc_init_gc ();

/*
 * Allocates memory via garbage collector.
 * When this function is called, allocated memory is known to GC.
 *
 * Params:
 * garcol    - garbage collector 
 * allocsize - amount of memory to allocate 
 * allocator - allocator function
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

#ifdef __cplusplus
}
#endif

#endif // !CGC_GC_H
