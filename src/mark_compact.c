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
#include "../lib/mark_compact.h"

void mark_tree_node_mc(BiTreeNode* node){
    if (node == NULL) return;

    _block_header* header =  get_header((void*) node);
    if (header->marked) {
        printf("ERROR: already marked @ collector.c:24\n");
        exit(1);
        return;
    }

    header->marked = true;
    mark_tree_node_mc(node->left);
    mark_tree_node_mc(node->right);
    return;
}

void* mark_compact_malloc(unsigned int nbytes) {
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

int mark_compact_gc(List* roots) {
    if (roots->size <= 0) return 0;
    int cleaned = 0;

    //mark phase:
    //for each root,
    //  traverse tree and mark all nodes
    ListNode* root_now = roots->first;
    while (root_now != NULL){
         BiTreeNode* data = root_now->data;
         mark_tree_node_mc(data);
         root_now = root_now->next;
    }

    /*
    * compact phase:
    * go through entire heap,
    * compute new addresses
    * copy objects to new addresses
    */
    // compute new addresses 
    _block_header* scan = (_block_header*) heap->base;
    _block_header* free = scan;
    while (scan < (_block_header*) heap->top) {
        if (scan->marked) {
            scan->ptr =  (char*) free;
            free = free + scan->size;
        }
        scan = scan + scan->size;
    }
    //update

    return cleaned;
 }
