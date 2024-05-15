/*
 * the mutator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "../lib/bool.h"
#include "../lib/heap.h"
#include "../lib/list.h"
#include "../lib/bistree.h"
#include "../lib/globals.h"

#define  MAX_NODES      5
#define  MIN_NODES      1
#define  MAX_KEY_VALUE  100
#define  MAX_ITER       -1
#define  MONO           true

#define HEAP_SIZE    (1024)

Heap* heap;
List* roots;

static int mutate;

static void sigint_handler() {mutate = 0;}

int main(int argc, char** argv) {
   /*
    * ^C to stop the program 
    */
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        printf("signal: cannot install handler for SIGINT\n");
        return 1;
    }

    float threshold = atof(argv[1]);  /* a value in the interval (0,1) */

    heap  = (Heap*)malloc(sizeof(Heap));
    roots = (List*)malloc(sizeof(List));

    heap_init(heap, HEAP_SIZE, collect_garbage);
    list_init(roots);

    srandom(getpid());
    mutate = MAX_ITER;

    /* build first bistree */
    BisTree* t = (BisTree*)malloc(sizeof(BisTree));
    bistree_init(t);
    list_addlast(roots,t);
    int number_nodes = MIN_NODES + random() % (MAX_NODES - MIN_NODES);
    for(int i = 0; i < number_nodes; i++) {
        bistree_insert(t, random() % MAX_KEY_VALUE);
    }

    /* build new bistree */
    while(mutate) {
        float toss = (float)random() / (float)RAND_MAX;
        if( toss > threshold || mutate == MAX_ITER) { // add nodes
            fprintf(stdout, "------------------------------------------------------------\n");
            fprintf(stdout, "Adding Nodes\n");
            BisTree* t;
            if (MONO) {
                t = list_get(roots, 0); 
            }
            else { 
                /* build new bistree */
                t = (BisTree*)malloc(sizeof(BisTree));
                bistree_init(t);
                /* preserve root */
                list_addlast(roots,t);
            }
            /* prepare to insert up to 100 nodes, a minimum of 5 */
            int number_nodes = MIN_NODES + random() % (MAX_NODES - MIN_NODES);
            for(int i = 0; i < number_nodes; i++) {
                /* populate tree with keys between 0-100 */
                bistree_insert(t, random() % MAX_KEY_VALUE);
            }
            fprintf(stdout, "tree size is %d\n", bistree_size(t));
            //fprintf(stdout, "number of trees %d\n",roots->size);
            fprintf(stdout, "------------------------------------------------------------\n");
            bistree_fancy(t);
        } 
        else { // remove nodes
             /* skip if there are no roots to manipulate */
            if (list_isempty(roots))
                continue;
            /* otherwise, choose random root to operate on */
            fprintf(stdout, "------------------------------------------------------------\n");
            fprintf(stdout, "deleting nodes from tree\n");
            BisTree* chosen;
            if(MONO) {
                chosen = list_get(roots, 0);
            }
            else {
                int index = random() % list_size(roots);
                chosen  = list_get(roots, index);
            }
            int number_nodes = bistree_size(chosen);
            int number_tries = random() % number_nodes;
            for(int i = 0; i < number_tries; i++ ) {
                /* remove key from tree if key exists in it */
                /* this is checked in bistree_remove */
                bistree_remove(chosen, random() % MAX_KEY_VALUE);
            }
            //fprintf(stdout, "number of trees %d\n",roots->size);
            fprintf(stdout, "tree size is %d\n", bistree_size(chosen));
            fprintf(stdout, "------------------------------------------------------------\n");
            bistree_fancy(chosen);
        }
        mutate -= 1;
    }
   /* caught ^C ! */
   printf("Finished Iterations\n");
   printf("Exiting Gracefully :)\n");
   heap_destroy(heap);
   return 0;
}
