/*
 * collector.c
 */

#include <stdio.h>

#include "list.h"
#include "bistree.h"
#include "globals.h"

_block_header* get_header(void* ptr){
    return (_block_header*) (ptr - sizeof(_block_header));
}

void mark_tree_node(BiTreeNode* node){
    if (node == NULL) return;

    _block_header* header =  get_header((void*) node);
    if (header->marked) return;
    header->marked = true;
    mark_tree_node(node->left);
    mark_tree_node(node->right);
    return;
}

void mark_sweep_gc(List* roots) {
    printf("-----------------------gcing()-------------------------------\n");

    if (roots->size <= 0) return;
    int cleaned = 0;

    //mark phase,
    //for each root,
    //  traverse tree and mark all nodes
    ListNode* root_now = roots->first;
    while (root_now != NULL){
         BiTreeNode* data = root_now->data;
         mark_tree_node(data);
         root_now = root_now->next;
    }

    /*
    * sweep phase:
    * go through entire heap,
    * add unmarked to free list
    */
    _block_header* heap_now = (_block_header*) heap->base;
    while(heap_now != (_block_header*) heap->top){
        if(!heap_now->marked){
            heap_now->ptr = heap->freeb;
            heap->freeb = (char*) heap_now;
            cleaned++;
        }

        heap_now->marked = false;
        // typecast everything to avoid C making weird things when doing pointer arithmetic
        heap_now = (_block_header*) ((void*) heap_now + sizeof(_block_header) + heap_now->size);
    }
    printf("cleaned: %d\n",cleaned);
    printf("-------------------------------------------------------------\n");
    return;
 }

void mark_compact_gc(List* roots) {
   /*
    * mark phase:
    * go throught all roots,
    * traverse trees,
    * mark reachable
    */

   /*
    * compact phase:
    * go through entire heap,
    * compute new addresses
    * copy objects to new addresses
    */
   printf("gcing()...\n");
   return;
 }

void copy_collection_gc(List* roots) {
   /*
    * go throught all roots,
    * traverse trees in from_space,
    * copy reachable to to_space
    */
   printf("gcing()...\n");
   return;
}

