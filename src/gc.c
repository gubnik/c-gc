#include "../incl/gc.h"

#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* See gc.h */
gc * cgc_init_gc ()
{
  gc * garcol = (gc *) malloc(sizeof(gc));
  if (!garcol)
  {
    fprintf(stderr, "[Error/255] Garbage collector failed to initialize\n");
    exit(255);
  }
  garcol->root = NULL;
  pthread_mutex_init(&garcol->mutex, NULL);
  return garcol;
}

/* See gc.h */
gc_obj * cgc_gcobj_new (size_t amount)
{
  gc_obj * nobj = malloc(sizeof(gc_obj));
  if (!nobj)
  {
    return NULL;
  }
  nobj->next = NULL;
  nobj->memarea = NULL;
  nobj->marked = false;
  pthread_mutex_init(&nobj->mutex, NULL);
  return nobj;
}

/* See gc.h */
void * cgc_gc_allocate(gc * garcol, size_t amount, allocator_t allocator)
{
  pthread_mutex_lock(&garcol->mutex);
  gc_obj * newobj = cgc_gcobj_new(amount);
  newobj->memarea = (*allocator)(amount); //malloc(amount);
  gc_obj * iter = garcol->root;
  if (!iter)
  {
    garcol->root = newobj;
    pthread_mutex_unlock(&garcol->mutex);
    return newobj->memarea;
  }
  while(iter->next)
  {
    iter = iter->next;
  }
  iter->next = newobj;
  pthread_mutex_unlock(&garcol->mutex);
  return newobj->memarea;
}

/* See gc.h */
void cgc_gc_mark(gc * garcol)
{
  pthread_mutex_lock(&garcol->mutex);
  for (gc_obj * ptr = garcol->root; ptr; ptr = ptr->next)
  {
    pthread_mutex_lock(&ptr->mutex);
    ptr->marked = true;
    pthread_mutex_unlock(&ptr->mutex);
  }
  pthread_mutex_unlock(&garcol->mutex);
}

/* See gc.h */
void cgc_gc_sweep(gc * garcol)
{
  pthread_mutex_lock(&garcol->mutex);
  gc_obj ** ref = &garcol->root;
  while (*ref)
  {
    gc_obj * obj = *ref;
    pthread_mutex_lock(&obj->mutex);
    if (!obj->marked)
    {
      obj->marked = false;
      ref = &obj->next;
      pthread_mutex_unlock(&obj->mutex);
    }
    else 
    {
      *ref = obj->next;
      free(obj->memarea);
      free(obj);
    }
  }
  pthread_mutex_unlock(&garcol->mutex);
}

/* See gc.h */
void cgc_gc_collect(gc * garcol)
{
  cgc_gc_mark(garcol);
  cgc_gc_sweep(garcol);
}

/* See gc.h */
void cgc_gc_destroy(gc *garcol)
{
  if (!garcol) return;
  pthread_mutex_destroy(&garcol->mutex);
  gc_obj * iter = garcol->root;
  while (iter)
  {
    gc_obj * temp = iter->next;
    pthread_mutex_destroy(&iter->mutex);
    free(iter);
    iter = temp;
  }
  free(garcol);
}
