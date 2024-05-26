/*
 * the heap
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "../lib/heap.h"
#include "../lib/globals.h"
#include "../lib/mark_sweep.h"
#include "../lib/mark_compact.h"
#include "../lib/copy_collect.h"

_block_header* get_header(char* ptr){
    return (_block_header*) (ptr - sizeof(_block_header));
}

void heap_init(Heap* heap, unsigned int size, void (*collector)(List*)){
    heap->base  = mmap ( NULL, size, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
    heap->size  = size;
    heap->limit = heap->base + size;
    heap->top   = heap->base;
    heap->_freeb = (List*)malloc(sizeof(List));
    heap->worklist = (List*)malloc(sizeof(List));
    list_init(heap->_freeb);
    heap->freeb = NULL;
    heap->collector = collector;
    if(ALGO == 3){
        heap->to_space = heap->base + ((heap->limit - heap->base) / 2);
    }
    return;
}

void heap_destroy(Heap* heap) {
    munmap(heap->base, heap->size);
    return;
}

void collect_garbage(List* roots){
    if(VERBOSE>=0){
        printf("-----------------------gcing()-------------------------------\n");
    }
    int cleaned = 0;
    switch(ALGO){
        case 1:
            cleaned = mark_sweep_gc(roots);
            break;
        case 2:
            cleaned = mark_compact_gc(roots);
            break;
        case 3:
            cleaned = copy_collect_gc(roots);
            break;
        default:
            printf("Invalid GC algorithm %d, exiting...\n", ALGO);
            exit(EXIT_FAILURE);
    }

    if(VERBOSE>=1){
        printf("cleaned: %d\n",cleaned);
        printf("-------------------------------------------------------------\n");
    }
}

void* my_malloc(unsigned int nbytes) {
    void* res;
    switch(ALGO){
        case 1:
            res = mark_sweep_malloc(nbytes);
            break;
        case 2:
            res = mark_compact_malloc(nbytes);
            break;
        case 3:
            res = copy_collect_malloc(nbytes);
            break;
        default:
            printf("Invalid GC algorithm %d, exiting...\n", ALGO);
            exit(EXIT_FAILURE);
    }
    return res;
}
