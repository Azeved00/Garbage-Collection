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

_block_header* get_header(void* ptr){
    return (_block_header*) (((char*)ptr) - sizeof(_block_header));
}

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

void collect_garbage(List* roots){
    printf("-----------------------gcing()-------------------------------\n");
    int cleaned = mark_sweep_gc(roots);

    printf("cleaned: %d\n",cleaned);
    printf("-------------------------------------------------------------\n");
}

void* my_malloc(unsigned int nbytes) {
    return mark_sweep_malloc(nbytes);
}
