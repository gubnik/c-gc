#include "incl/gc.h"
#include <pthread.h>
#include <stdio.h>

#define CGC_USE_INIT
#define CGC_ENTRY_POINT

#include "incl/marcos.h"

void * cgc_init_int (size_t N)
{
  int * I = malloc(N);
  *I = 42;
  return I;
}

#define TALLOCS 5

void * threadfunc (void * garcol)
{
  for (int i = 0; i < TALLOCS; i++)
  {
    cgc_gc_allocate(garcol, sizeof(int), cgc_init_int);
  }
  return NULL;
}

int main (int argc, char ** argv)
{
  pthread_t threads [8];
  for (int i = 0; i < 8; i++)
  {
    pthread_create(&threads[i], NULL, threadfunc, garcol);
  }
  for (int i = 0; i < 8; i++)
  {
    pthread_join(threads[i], NULL);
  }
  gc_obj * iter = garcol->root;
  while (iter)
  {
    printf("%p :: %d\n", iter, *(int*)iter->memarea);
    iter = iter->next;
  }
  return 0;
}
