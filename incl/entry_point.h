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

int CGCUserMain(int argc, char ** argv, gc * gc);

#ifndef NO_GC_ENTRY_POINT
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

#define allocate(Type) (Type *) cgc_gc_allocate(garcol, sizeof(Type))
#define allocate_area(Type, N) (Type *) cgc_gc_allocate(garcol, sizeof(Type) * (size_t) N)

#endif // !C_GC_ENTRY_POINT
