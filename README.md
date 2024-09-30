# C-GC

A basic garbage collector implementation in C. It supports multithreading.

## Production usage
This garbage collector is not entirely suited for production usage (yet), because:
1. Marking algorithm is as primitive as possible and marks _every_ object independant of 
its reference count, usage etc.
2. Advanced features rely heavily on macros that in turn rely on user-defined functions,
which introduces a level of boilerplate and abstraction, especially during compilation.
Using modern LSPs somewhat mitigates potential drawbacks of this but it still may be 
undesirable for a large codebase.
3. Multithreading has potential for deadlocks in marking and sweeping functions, which I was 
not able to confirm nor deny. Use with caution.


## Example
```c 
#include <stdlib.h>

int main (int argc, char ** argv)
{
    gc * garcol = cgc_init_gc ();
    int * Buf1 = cgc_gc_allocate(garcol, sizeof(int) * 42, malloc);
    
    // Do some stuff

    cgc_gc_collect (garcol);
    cgc_gc_destroy (garcol);
    return 0;
}
```

# Macros
Aside from garbage collector, C-GC provides a set of macros 
defined in `macros.h`. Those macros are designed to reduce boilerplate
of initializing garbage collector, carrying its pointer etc.

They do so by assuming a new entry point, replacing `main(...)`
with a custom one, where GC is already initialized and then destroyed.

## Example
```c 

#define CGC_USE_INIT
#define CGC_ENTRY_POINT

#include "macros.h"
#include <stdlib.h>

CGC_MAKE_DEFAULT_INIT_FUNC(int);

typedef struct big_stuff
{
    size_t num1;
    size_t num2;
    size_t num3;
    size_t num4;
} big_stuff;

// Initializes big_stuff struct with random values
void * cgc_init_big_stuff (size_t N)
{
    big_stuff * bs = malloc(N);
    bs->num1 = rand();
    bs->num2 = rand();
    bs->num3 = rand();
    bs->num4 = rand();
    return bs;
}

int main (int argc, char ** argv)
{
    int * Buf1 = allocate_area(int, 42);
    big_stuff * Buf1 = allocate(big_stuff);
    return 0;
}

```
