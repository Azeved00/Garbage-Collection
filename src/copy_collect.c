#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include "../lib/list.h"
#include "../lib/bistree.h"
#include "../lib/globals.h"
#include "../lib/heap.h"
#include "../lib/globals.h"
#include "../lib/copy_collect.h"

void mark_tree_node_cc(BiTreeNode* node){
    if (node == NULL) return;

    _block_header* header =  get_header((void*) node);
    if (header->marked) {
        printf("ERROR: already marked @ collector.c:24\n");
        exit(1);
        return;
    }

    header->marked = true;
    mark_tree_node_cc(node->left);
    mark_tree_node_cc(node->right);
    return;
}

void* copy_collect_malloc(unsigned int nbytes) {
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

int copy_collect_gc(List* roots) {
       /*
     * go throught all roots,
     * traverse trees in from_space,
     * copy reachable to to_space
     */
    int cleaned = 0;
    return cleaned; 
}
