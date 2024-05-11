/*
 * heap.h
 */

#ifndef HEAP_H
#define HEAP_H

#include "list.h"

typedef struct {
   unsigned int marked;
   unsigned int size;
   char* ptr;
} _block_header;

typedef struct {
   unsigned int size;
   char*        base;
   char*        top;
   char*        limit;
   List*        _freeb;
   char*        freeb;
   void (*collector)(List*);
} Heap;

void heap_init(Heap* heap, unsigned int size, void (*collector)(List*));

void heap_destroy(Heap* heap);

void* my_malloc(unsigned int nbytes);

#endif
