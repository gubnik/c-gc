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

#include "../incl/gc.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* See gc.h */
/*
 * Implementation details:
 * - failed allocation results in an error 
 * - initializes mutex 
 */
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
/*
 * Implementation details:
 * - failed allocation results in a NULL return 
 * - initializes mutex 
 * - initalizes pointer fields to NULL 
 */
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
/*
 * Implementation details:
 * - failed gc_obj allocation returns NULL 
 * - failed object allocation via allocator returns NULL 
 */
void * cgc_gc_allocate(gc * garcol, size_t amount, allocator_t allocator)
{
  pthread_mutex_lock(&garcol->mutex);
  gc_obj * newobj = cgc_gcobj_new(amount);
  if (!newobj)
  {
    pthread_mutex_unlock(&garcol->mutex);
    return NULL;
  }
  newobj->memarea = (*allocator)(amount); //malloc(amount);
  if (!newobj->memarea)
  {
    pthread_mutex_destroy(&newobj->mutex);
    pthread_mutex_unlock(&garcol->mutex);
    free(newobj);
    return NULL;
  }
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
/*
 * Implementation details:
 * - marks every object 
 * TODO: implement either reference counting algorith AND/OR stack based algorith
 */
void cgc_gc_mark(gc * garcol)
{
  for (gc_obj * ptr = garcol->root; ptr; ptr = ptr->next)
  {
    pthread_mutex_lock(&ptr->mutex);
    ptr->marked = true;
    pthread_mutex_unlock(&ptr->mutex);
  }
}

/* See gc.h */
/*
 * Implementation details:
 * - frees every marked object 
 * - destroys every marked object's mutex
 */
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
      pthread_mutex_destroy(&obj->mutex);
      free(obj->memarea);
      free(obj);
    }
  }
  pthread_mutex_unlock(&garcol->mutex);
}

/* See gc.h */
/*
 * Implementation details:
 * - invokes mark and sweep functions, mutex locking and unlocking is done within
 *   those functions.
 */
void cgc_gc_collect(gc * garcol)
{
  cgc_gc_mark(garcol);
  cgc_gc_sweep(garcol);
}

/* See gc.h */
/*
 * Implementation details:
 * - frees every associated object
 * - destroys mutex 
 * - destroys every associated object's mutex 
 */ 
void cgc_gc_destroy(gc *garcol)
{
  if (!garcol) return;
  pthread_mutex_unlock(&garcol->mutex);
  pthread_mutex_destroy(&garcol->mutex);
  gc_obj * iter = garcol->root;
  while (iter)
  {
    gc_obj * temp = iter->next;
    pthread_mutex_destroy(&iter->mutex);
    free(iter->memarea);
    free(iter);
    iter = temp;
  }
  free(garcol);
}
