#ifndef C_GC
#define C_GC

#include <stdbool.h>
#include <stddef.h>

#define MAX_REF_COUNT 1e+6

typedef struct gc gc;
typedef struct gc_obj gc_obj;

typedef struct gc_obj
{
  gc_obj *  next;
  size_t refcount;
  void * memarea;
  bool marked;
}
gc_obj;

typedef struct gc 
{
  gc_obj * root;
}
gc;

gc_obj * cgc_gcobj_new (size_t);

gc * cgc_gc_init ();
void * cgc_gc_allocate (gc * garcol, size_t allocsize);
void cgc_gc_mark (gc * garcol);
void cgc_gc_sweep (gc * garcol);
void cgc_gc_collect (gc * garcol);
void cgc_gc_destroy (gc * garcol);

#endif // !C_GC
