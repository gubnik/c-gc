#ifndef C_GC_ENTRY_POINT
#define C_GC_ENTRY_POINT

#include "gc.h"

int CGCUserMain(int argc, char ** argv, gc * gc);

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
  int CGCUserMain(int argc, char ** argv, gc * gc)

#ifdef CGC_MACROS
#define allocate(Type) (Type *) cgc_gc_allocate(gc, sizeof(Type))
#define allocate_area(Type, N) (Type *) cgc_gc_allocate(gc, sizeof(Type) * (size_t) N)
#endif

#endif // !C_GC_ENTRY_POINT
