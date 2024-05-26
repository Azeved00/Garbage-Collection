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

/*
 *  Copy and Collect Malloc:
 *  - Check if you can allocate a block
 *      this check depends on wich half you are using currently
 *  - If you cant allocate then GC 
 *  - If you didnt free anything in the GC then return NULL
 */
void* copy_collect_malloc(unsigned int nbytes) {
    //check in wich half we are allocating
    if (heap->base > heap->to_space) {
        //if not enough space then call gc
        if( heap->top + sizeof(_block_header) + nbytes >= heap->limit ) {
            printf("my_malloc: not enough space, performing GC...\n");
            heap->collector(roots);
            //heap free space and base switch after gcing
            if (heap->top + sizeof(_block_header) + nbytes >= heap->to_space) {
                printf("my_malloc: not enough space after GC...\n");
                return NULL;
            }
        } 
    }
    else {
        if( heap->top + sizeof(_block_header) + nbytes >= heap->to_space ) {
            printf("my_malloc: not enough space, performing GC...\n");
            heap->collector(roots);
            //heap free space and base switch after gcing
            if (heap->top + sizeof(_block_header) + nbytes >= heap->limit) {
                printf("my_malloc: not enough space after GC...\n");
                return NULL;
            }
        } 
    }

    _block_header* q = (_block_header*)(heap->top);
    q->marked = false;
    q->size   = nbytes;
    q->ptr    = NULL;
    char *p = heap->top + sizeof(_block_header);
    heap->top = heap->top + sizeof(_block_header) + nbytes;
    return p;
}

/* 
 *  Copy a BiTreeNode object to the heap free pointer
 *      move the heap free pointer
 *      returns a pointer to the copied data
 */
_block_header* copy(_block_header* from_ptr){
    char* to_ptr = heap->freeb;
    //move free pointer
    heap->freeb = heap->freeb + from_ptr->size + sizeof(_block_header);

    //copy block header
    _block_header* bh_to_ptr = (_block_header*) to_ptr;
    bh_to_ptr->size = from_ptr->size;
    bh_to_ptr->ptr = NULL;
    
    //copy the data 
    BiTreeNode* data_to_ptr =(BiTreeNode*) (to_ptr + sizeof(_block_header));
    BiTreeNode* data_from_ptr = (BiTreeNode*)(((char*) from_ptr) + sizeof(_block_header));
    *data_to_ptr = *data_from_ptr;

    //make old data point to new data
    from_ptr->ptr =(char*) bh_to_ptr;
    
    //add data to worklist
    list_addfirst(heap->worklist, to_ptr);
    return bh_to_ptr;
}

/* 
 * get the new location
 * if it is yet to be copied to a new location, copy it
 */
BiTreeNode* forward(BiTreeNode* fromRef){
    _block_header* bh_from_ptr = get_header((char*) fromRef);
    _block_header* bh_to_ptr = (_block_header*)(bh_from_ptr->ptr);
    if (bh_to_ptr == NULL){
        bh_to_ptr = copy(bh_from_ptr);
    }
    return (BiTreeNode*)(((char*)bh_to_ptr) + sizeof(_block_header));
}

/*
 * update fld to its new location
 */
void process(BiTreeNode** fld){
    if (*fld != NULL) {
        *fld = forward(*fld);
    }
}

/*
 * Copy and Collect GC:
 * 1. Change to th other, empty, segment
 * 2. Process each of the roots
 * 3. Process each Node in the worklist until it is empty
 */
int copy_collect_gc(List* roots) {
    char* t = heap->base;
    heap->base = heap->to_space;
    heap->to_space = t;
    heap->freeb = heap->base;

    list_init(heap->worklist);
    
    //process each root
    ListNode* root_now = roots->first;
    while (root_now != NULL){
         BisTree* data = root_now->data;
         data->to_clean = 0;
         process(&data->root);
         root_now = root_now->next;
    }

    //work through the worklist
    while(!list_isempty(heap->worklist)){
        char* bh_ref = list_getfirst(heap->worklist);
        list_removefirst(heap->worklist);

        BiTreeNode* data_ref = (BiTreeNode*)(bh_ref + sizeof(_block_header));
        process(&data_ref->left);
        process(&data_ref->right);
    } 

    int cleaned = ((heap->top - heap->to_space) - (heap->freeb - heap->base)) / (sizeof(_block_header)+sizeof(BiTreeNode));
    heap->top = heap->freeb;
    return cleaned; 
}
