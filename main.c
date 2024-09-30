#include "incl/gc.h"
#include <pthread.h>
#include <stdio.h>

#define CGC_USE_INIT
#define CGC_ENTRY_POINT
#define CGC_USE_CONSTRUCTORS

#include "incl/marcos.h"

void * cgc_init_int (size_t N)
{
  int * I = malloc(N);
  *I = 42;
  return I;
}

typedef struct big_stuff
{
  size_t M;
  size_t N;
} big_stuff;

void * cgc_init_big_stuff (size_t N)
{
  big_stuff * obj = malloc(N);
  return obj;
}

void cgc_construct_big_stuff (void * objptr, size_t A, size_t B)
{
  big_stuff * bs = (big_stuff *) objptr;
  bs->M = A;
  bs->N = B;
}

//CGC_MAKE_DEFAULT_CONSTRUCTORS(big_stuff)

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
  printf("Multithreading tests finished!\n");
  // Constructor test
  construct(BS, big_stuff, 42, 69);
  printf("BS constructed: %lu %lu\n", BS->M, BS->N);
  return 0;
}
