#include <stdio.h>

#define CGC_USE_INIT

#include "incl/entry_point.h"

CGC_MAKE_DEFAULT_INIT_FUNC(int);

int main (int argc, char ** argv)
{
  int * intarray = allocate_area(int, 10);
  for (int i = 0; i < 10; i++)
  {
    intarray[i] = 10;
    printf("%d\n", i);
  }
  return 9;
}
