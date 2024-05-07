/*
 * collector.c
 */

#include <stdio.h>

#include "list.h"
#include "bistree.h"
#include "globals.h"

_block_header* get_header(void* ptr){
    return (_block_header*) ptr - sizeof(_block_header);
}
void mark_tree_node(BiTreeNode* node){
    if (node != NULL) return;

    _block_header* header =  get_header((void*) &node);
    header->marked = true;
    mark_tree_node(node->left);
    mark_tree_node(node->right);
    return;
}

void mark_sweep_gc(List* roots) {
    if (roots->size <= 0) return;
    int cleaned = 0;

    //mark phase,
    //for each root, traverse the tree and mark all true
    ListNode* root_now = roots->first;
    while (root_now != NULL){
         BiTreeNode* data = root_now->data;
         mark_tree_node(data);
         root_now = root_now->next;
    }

    _block_header* heap_now = (_block_header*) heap->base;
    void* last_free = heap->freeb;
    while(heap_now != NULL){
        if(heap_now->marked == false){
            heap_now->ptr = last_free;
            last_free = heap_now;
            cleaned++;
        }

        heap_now->marked = false;
        heap_now += sizeof(_block_header*) + heap_now->size;
    }


   /*
    * sweep phase:
    * go through entire heap,
    * add unmarked to free list
    */
   printf("gcing()...\n");
   printf("cleaned: %d\n",cleaned);
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

