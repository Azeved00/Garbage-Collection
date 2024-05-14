/*
 * the mutator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "bool.h"
#include "heap.h"
#include "list.h"
#include "bistree.h"
#include "globals.h"
#include "collector.h"

#define  MAX_NODES      100
#define  MIN_NODES      5
#define  MAX_KEY_VALUE  100

//#define HEAP_SIZE    (1024 * 1024)  /* 1 MByte */
#define HEAP_SIZE    (1024 * 20)  /* 20 KByte */

Heap* heap;
List* roots;

static bool mutate;

static void sigint_handler() {
   mutate = false;
}

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
   mutate = true;
   while(mutate) {
        float toss = (float)random() / (float)RAND_MAX;
        if( toss > threshold ) { // add nodes
            fprintf(stdout, "------------------------------------------------------------\n");
            fprintf(stdout, "creating a new tree\n");
            fprintf(stdout, "------------------------------------------------------------\n");
            /* build new bistree */
            BisTree* t = (BisTree*)malloc(sizeof(BisTree));
            bistree_init(t);
            /* preserve root */
            list_addlast(roots,t);
            /* prepare to insert up to 100 nodes, a minimum of 5 */
            int number_nodes = MIN_NODES + random() % (MAX_NODES - MIN_NODES);
            for(int i = 0; i < number_nodes; i++) {
                /* populate tree with keys between 0-100 */
                bistree_insert(t, random() % MAX_KEY_VALUE);
            }
            fprintf(stdout, "tree size is %d\n", bistree_size(t));
            fprintf(stdout, "number of trees %d\n",roots->size);
            bistree_fancy(t);
        } 
        else { // remove nodes
             /* skip if there are no roots to manipulate */
            if (list_isempty(roots))
                continue;
            /* otherwise, choose random root to operate on */
            int index = random() % list_size(roots);
            fprintf(stdout, "------------------------------------------------------------\n");
            fprintf(stdout, "deleting nodes from tree %d\n", index);
            fprintf(stdout, "------------------------------------------------------------\n");
            BisTree* chosen  = list_get(roots, index);
            int number_nodes = bistree_size(chosen);
            int number_tries = random() % number_nodes;
            for(int i = 0; i < number_tries; i++ ) {
                /* remove key from tree if key exists in it */
                /* this is checked in bistree_remove */
                bistree_remove(chosen, random() % MAX_KEY_VALUE);
            }
            fprintf(stdout, "number of trees %d\n",roots->size);
            fprintf(stdout, "tree size is %d\n", bistree_size(chosen));
            bistree_fancy(chosen);
        }
    }
   /* caught ^C ! */
   printf("quiting...\n");
   heap_destroy(heap);
   return 0;
}
