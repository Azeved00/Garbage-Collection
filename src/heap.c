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

//#define MARK_SWEEP
#define MARK_COMPACT
//#define COPY_COLLECT

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
#ifdef MARK_SWEEP
    int cleaned = mark_sweep_gc(roots);
#endif
#ifdef MARK_COMPACT
    int cleaned = mark_compact_gc(roots);
#endif
#ifdef COPY_COLLECT
    int cleaned = copy_collect_gc(roots);
#endif

    printf("cleaned: %d\n",cleaned);
    printf("-------------------------------------------------------------\n");
}

void* my_malloc(unsigned int nbytes) {
#ifdef MARK_SWEEP
    void* res = mark_sweep_malloc(nbytes);
#endif
#ifdef MARK_COMPACT
    void* res = mark_compact_malloc(nbytes);
#endif
#ifdef COPY_COLLECT
    void* res = copy_collect_malloc(nbytes);
#endif
    return res;
}
