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


/*
 * Macros for C-GC
 *
 * Entry point:
 * This header employs a main(...) macros that abstracts away
 * initialization, collection and destruction of garbage collector
 *
 * To disable this entry point, define NO_GC_ENTRY_POINT
 *
 * Macros:
 * allocate(Type) - allocates exactly one object of type on the heap through garbage collector 
 * allocate_area(Type, N) - allocates N objects of type on the heap through garbage collector
 */

#ifndef CGC_MACROS_H
#define CGC_MACROS_H

#include "gc.h"
#include <stdlib.h>
#include <stdio.h>

int CGCUserMain(int argc, char ** argv, gc * gc);

/*
 * Custom entry point macros 
 * Replaces main(...) entry point with a predefined one with initialization and
 * destruction of a garbage collector.
 * User-defined code will be placed into CGCUserMain(), with provided pointer to 
 * the initalized garbage collector by $garcol.
 */
#ifdef CGC_ENTRY_POINT
  #define main(...) \
    main (int argc, char ** argv) \
    { \
      gc * garcol = cgc_init_gc(); \
      int res = \
        CGCUserMain(argc, argv, garcol); \
      cgc_gc_collect (garcol); \
      cgc_gc_destroy (garcol); \
      return res; \
    } \
    int CGCUserMain(int argc, char ** argv, gc * garcol)
#endif

/*
 * Option to use a user-defined allocation method instead of malloc.
 * By default, malloc will be used in allocate macros, with no fields initialization.
 */
#ifdef CGC_USE_INIT
  #define objalloc(Type) (allocator_t)cgc_init_##Type
#else
  #define objalloc(Type) (allocator_t)malloc
#endif

/*
 * Macros for defining a default malloc() init function for a type.
 * Use with CGC_USE_INIT for types that don't require elaborate initialization.
 */
#define CGC_MAKE_DEFAULT_INIT_FUNC(Type) \
void * cgc_init_##Type (size_t N) \
{ return malloc(N); }

/*
 * Macros for easy type allocation via a garbage collector.
 * With custom entry point, it is not required to pass garbage collector pointer.
 */
#ifdef CGC_ENTRY_POINT
  #define allocate(Type) (Type *) cgc_gc_allocate(garcol, sizeof(Type), objalloc(Type))
  #define allocate_area(Type, N) (Type *) cgc_gc_allocate(garcol, sizeof(Type) * (size_t) N, objalloc(Type))
#else 
  #define allocate(gc, Type) (Type *) cgc_gc_allocate(gc, sizeof(Type), objalloc(Type))
  #define allocate_area(gc, Type, N) (Type *) cgc_gc_allocate(gc, sizeof(Type) * (size_t) N, objalloc(Type))
#endif

/*
 * Macro for implementing pseudo-constructors
 * Make sure that thee construsctor you call is defined in cgc_construct_$(typename) function 
 * with matching argument types
 */
#ifdef CGC_USE_INIT
  #ifdef CGC_USE_CONSTRUCTORS
    #ifdef CGC_ENTRY_POINT
      #define construct(objname, Type, ...) \
        Type * objname = allocate(Type); \
        if (!objname) \
        { \
          fprintf(stderr, "[Error] Constructor failed because allocator had failed\n"); \
          exit (1); \
        } \
        cgc_construct_##Type(objname, __VA_ARGS__);
    #else 
      #define construct(objname, gc, Type, ...) \
        Type * objname = allocate(gc, Type); \
        if (!objname) \
        { \
          fprintf(stderr, "[Error] Constructor failed because allocator had failed\n"); \
          exit (1); \
        } \
        cgc_construct_##Type(objname, __VA_ARGS__);
    #endif
  #endif
#endif

#define CGC_MAKE_DEFAULT_CONSTRUCTORS(Type) \
void cgc_construct_##Type(void * obj, size_t s1, size_t s2) \
{ \
  size_t size = sizeof(Type); \
  for (size_t i = 0; i < size / sizeof(size_t); i++) ((size_t*)obj)[i] &= 0; \
}

#endif // !CGC_MACROS_H
