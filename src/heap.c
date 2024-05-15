/*
 * the heap
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "heap.h"
#include "globals.h"
#include "collector.h"


void heap_init(Heap* heap, unsigned int size, void (*collector)(List*)){
    heap->base  = mmap ( NULL, size, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
    heap->size  = size;
    heap->limit = heap->base + size;
    heap->top   = heap->base;
    heap->_freeb = (List*)malloc(sizeof(List));
    list_init(heap->_freeb);
    heap->freeb = NULL;
    heap->collector = collector;
    return;
}

void heap_destroy(Heap* heap) {
    munmap(heap->base, heap->size);
    return;
}

void* my_malloc(unsigned int nbytes) {
    if( heap->top + sizeof(_block_header) + nbytes < heap->limit ) {
        _block_header* q = (_block_header*)(heap->top);
        q->marked = false;
        q->size   = nbytes;
        q->ptr    = (char*) -1;
        char *p = heap->top + sizeof(_block_header);
        heap->top = heap->top + sizeof(_block_header) + nbytes;
        return p;
    } 
    if (heap->freeb != NULL) {
        // return this block and set the pointer to the next free block
        char* rtn = heap->freeb;
        heap->freeb = ((_block_header*) heap->freeb)->ptr;

        //the returned pointer should point to the end of the header
        return rtn+sizeof(_block_header);
    }
    else {
        printf("my_malloc: not enough space, performing GC...\n");
        heap->collector(roots);
        if (heap->freeb == NULL) {
            printf("my_malloc: not enough space after GC...\n");
            return NULL;
        }
       
        // return this block and set the pointer to the next free block
        char* rtn = heap->freeb;
        heap->freeb = ((_block_header*) heap->freeb)->ptr;

        //the returned pointer should point to the end of the header
        return rtn+sizeof(_block_header);
        /*
        if ( list_isempty(heap->_freeb) ) {
          printf("my_malloc: not enough space after GC...");
          return NULL;
        }
        return list_getfirst(heap->_freeb);
        */
    }
}
