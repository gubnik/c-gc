/*
 * Macros for C-GC
 *
 * Entry point:
 * This header employs a main(...) macros that abstracts away
 * initialization, collection and destruction of garbage collector
 *
 * To disable this entry point, define NO_GC_ENTRY_POINT
 *
 * Macros:
 * allocate(Type) - allocates exactly one object of type on the heap through garbage collector 
 * allocate_area(Type, N) - allocates N objects of type on the heap through garbage collector
*/


#ifndef C_GC_ENTRY_POINT
#define C_GC_ENTRY_POINT

#include "gc.h"
#include <stdlib.h>

int CGCUserMain(int argc, char ** argv, gc * gc);

#ifdef CGC_ENTRY_POINT
#define main(...) \
  main (int argc, char ** argv) \
  { \
    gc * garcol = cgc_gc_init(); \
    int res = \
      CGCUserMain(argc, argv, garcol); \
    cgc_gc_collect (garcol); \
    cgc_gc_destroy (garcol); \
    return res; \
  } \
  int CGCUserMain(int argc, char ** argv, gc * garcol)
#endif

/*
 * Option to use a user-defined allocation method instead of malloc.
 * By default, malloc will be used in cgc_gc_type_allocate, with no fields 
 * initialization.
 */
#ifdef CGC_USE_INIT
  #define objalloc(Type) (allocator_t)cgc_init_##Type
#else
  #define objalloc(Type) (allocator_t)malloc
#endif

#define CGC_MAKE_DEFAULT_INIT_FUNC(Type) \
void * cgc_init_##Type (size_t N) \
{ return malloc(N); }

#define allocate(Type) (Type *) cgc_gc_allocate(garcol, sizeof(Type), objalloc(Type))
#define allocate_area(Type, N) (Type *) cgc_gc_allocate(garcol, sizeof(Type) * (size_t) N, objalloc(Type))

#endif // !C_GC_ENTRY_POINT
