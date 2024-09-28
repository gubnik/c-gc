#include <stdio.h>

#include "incl/entry_point.h"


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
