/*
 * globals.h
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "list.h"
#include "heap.h"

extern List*    roots;
extern Heap*    heap;
extern int      MONO;
extern int      ALGO;
extern int      ITERATIONS;
extern int      VERBOSE;
extern unsigned int HEAP_SIZE;

#endif
