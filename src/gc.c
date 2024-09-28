#include "../incl/gc.h"

#include <stdio.h>
#include <stdlib.h>

gc * cgc_init_gc ()
{
  gc * garcol = (gc *) malloc(sizeof(gc));
  if (!garcol)
  {
    fprintf(stderr, "[Error/255] Garbage collector failed to initialize\n");
    exit(255);
  }
  garcol->root = NULL;
  return garcol;
}

gc_obj * cgc_gcobj_new (size_t amount)
{
  gc_obj * nobj = malloc(sizeof(gc_obj));
  if (!nobj)
  {
    fprintf(stderr, "[Error/255] Garbage collector object failed to initialize\n");
    exit(255);
  }
  nobj->next = NULL;
  nobj->marked = false;
  return nobj;
}

void * cgc_gc_allocate(gc * garcol, size_t amount, allocator_t allocator)
{
  gc_obj * newobj = cgc_gcobj_new(amount);
  newobj->memarea = (*allocator)(amount); //malloc(amount);
  gc_obj * iter = garcol->root;
  if (!iter)
  {
    garcol->root = newobj;
    return newobj->memarea;
  }
  while(iter->next)
  {
    iter = iter->next;
  }
  iter->next = newobj;
  return newobj->memarea;
}

void cgc_gc_mark(gc * garcol)
{
  for (gc_obj * ptr = garcol->root; ptr; ptr = ptr->next)
  {
    ptr->marked = true;
  }
}

void cgc_gc_sweep(gc * garcol)
{
  gc_obj ** ref = &garcol->root;
  while (*ref)
  {
    gc_obj * obj = *ref;
    if (!obj->marked)
    {
      obj->marked = false;
      ref = &obj->next;
    }
    else 
    {
      *ref = obj->next;
      free(obj->memarea);
      free(obj);
    }
  }
}

void cgc_gc_collect(gc * garcol)
{
  cgc_gc_mark(garcol);
  cgc_gc_sweep(garcol);
}

void cgc_gc_destroy(gc *garcol)
{
  if (!garcol) return;
  free(garcol);
}
