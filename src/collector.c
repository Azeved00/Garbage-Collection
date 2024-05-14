/*
 * collector.c
 */

#include <stdio.h>

#include "list.h"
#include "bistree.h"
#include "globals.h"

//-----------------------------------
//HELPER FUNCTIONS
//-----------------------------------
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

//-----------------------------------
// GC ALGORITHMS
//-----------------------------------
int mark_sweep_gc(List* roots) {
    if (roots->size <= 0) return 0;
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

    return cleaned;
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
         mark_tree_node(data);
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

int copy_collection_gc(List* roots) {
    /*
     * go throught all roots,
     * traverse trees in from_space,
     * copy reachable to to_space
     */
    int cleaned = 0;
    return cleaned;
}


//-----------------------------------
// MAIN FUNCTIONS
//-----------------------------------
void collect_garbage(List* roots){
    printf("-----------------------gcing()-------------------------------\n");
    int cleaned = mark_sweep_gc(roots);

    printf("cleaned: %d\n",cleaned);
    printf("-------------------------------------------------------------\n");
}
