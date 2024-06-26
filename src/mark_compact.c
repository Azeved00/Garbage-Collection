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
/*
 *  Mark Nodes recursively
 */
void mark_tree_node_mc(BiTreeNode* node){
    if (node == NULL) return;

    _block_header* header =  get_header((char*) node);
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

/*
 *  Mark and compact malloc:
 *  1. Check if you can allocate a block normally
 *  2. You can not allocate and therefore perform GC,
 *      if no block was freed by the GC then return NULL
 */ 
void* mark_compact_malloc(unsigned int nbytes) {
    //if not enough space then call gc
    if( heap->top + sizeof(_block_header) + nbytes >= heap->limit ) {
        printf("my_malloc: not enough space, performing GC...\n");
        heap->collector(roots);
        if (heap->top + sizeof(_block_header) + nbytes >= heap->limit) {
            printf("my_malloc: not enough space after GC...\n");
            return NULL;
        }
    } 
    _block_header* q = (_block_header*)(heap->top);
    q->marked = false;
    q->size   = nbytes;
    q->ptr    = (char*) -1;
    char *p = heap->top + sizeof(_block_header);
    heap->top = heap->top + sizeof(_block_header) + nbytes;
    return p;
}

/*
 * Returns the new address of the BiTreeNode Given
 */
BiTreeNode* get_new_pointer(BiTreeNode* prev){
    char* new_ptr = (char*) (get_header((char*)prev)->ptr);
    new_ptr = new_ptr + sizeof(_block_header);
    return (BiTreeNode*) new_ptr;
}

/*
 *  Mark & compact Garbage Collector:
 *  1. Mark Phase:
 *      Mark Nodes of each of the Root Tree
 *
 *  2. Compact Phase:
 *      a. Compute the new addresses:
 *      b. Update Pointers
 *          I. Update Roots
 *          II.Update Internal References of Nodes
 *      c. Relocate Objects to their new positions
 */
int mark_compact_gc(List* roots) {
    if (roots->size <= 0) return 0;
    int cleaned = 0;

    if(VERBOSE >= 2){
        printf("Mark phase\n");
    }
    ListNode* root_now = roots->first;
    while (root_now != NULL){
         BisTree* data = root_now->data;
         mark_tree_node_mc(data->root);
         root_now = root_now->next;
    }

    if(VERBOSE >= 2){
        printf("Compact phase\n");
        printf("- Computing new addresses\n");
    }
    char* scan = heap->base;
    char* free = scan;
    while (scan <  heap->top) {
        _block_header* bh = (_block_header*) scan;
        if (bh->marked) {
            bh->ptr =  (char*) free;
            free += sizeof(_block_header) + bh->size;
        }
        scan += sizeof(_block_header) + bh->size;
    }


    if(VERBOSE >= 2){
        printf("- Compact phase\n");
        printf("\t- Update roots\n");
    }
    root_now = roots->first;
    while (root_now != NULL){
         BisTree* data = root_now->data;
         data->root = get_new_pointer(data->root);
         root_now = root_now->next;
    }

    if(VERBOSE >= 2){
        printf("\t- Update internal references\n");
    }
    scan = heap->base;
    while (scan <  heap->top) {
        _block_header*  bh = (_block_header*) scan;
        BiTreeNode*   data = (BiTreeNode*) (scan+sizeof(_block_header));
        if (bh->marked) {
            if(data->left  != NULL)
                data->left  = get_new_pointer(data->left);
            if(data->right != NULL)
                data->right = get_new_pointer(data->right);
        }
        scan += sizeof(_block_header) + bh->size;
    }

    if(VERBOSE >= 2){
        printf("- Relocate Objects\n");
    }
    scan = heap->base;
    while (scan <  heap->top) {
        _block_header*  bh = (_block_header*) scan;
        BiTreeNode*   data_old = (BiTreeNode*) (scan+sizeof(_block_header));
        BiTreeNode*   data_new = get_new_pointer(data_old);
        if (bh->marked) {
            *data_new = *data_old;
        }
        else {
            cleaned++;
        }
        bh->marked = false;
        scan += sizeof(_block_header) + bh->size;
    }
    heap->top = free;

    return cleaned;
 }
