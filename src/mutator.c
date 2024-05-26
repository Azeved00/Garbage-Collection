/*
 * the mutator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include "../lib/bool.h"
#include "../lib/heap.h"
#include "../lib/list.h"
#include "../lib/bistree.h"
#include "../lib/globals.h"

#define  MAX_NODES      5
#define  MIN_NODES      1
#define  MAX_KEY_VALUE  100

Heap* heap;
List* roots;
int MONO = 0;
int ALGO = 1;
int ITERATIONS = -1;
unsigned int HEAP_SIZE = 1000;
int VERBOSE = 1;

static int mutate;

static void sigint_handler() {mutate = 0;}

void print_usage(FILE* x, char* cmd){
    fprintf(x, "Usage: %s [--help]  [--verbose[<1|2>]] [--mono] [--algo <1|2|3>] [--iterations <itererations>] [--heap-size <heap_size>] <threshold>\n", cmd);
}

void main_loop(float threshold){
    /* build new bistree */
    while(mutate) {
        float toss = (float)random() / (float)RAND_MAX;
        if( toss > threshold || mutate == ITERATIONS) { // add nodes
            if(VERBOSE >= 2){
                fprintf(stdout, "------------------------------------------------------------\n");
            }
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
            if(VERBOSE >= 1){
                fprintf(stdout, "Adding Nodes %d\n", number_nodes);
            }
            for(int i = 0; i < number_nodes; i++) {
                /* populate tree with keys between 0-100 */
                bistree_insert(t, random() % MAX_KEY_VALUE);
            }
            if(VERBOSE >=2){
                fprintf(stdout, "tree size is %d\n", bistree_size(t));
                fprintf(stdout, "to clean %d\n",t->to_clean);
                if(!MONO){
                    fprintf(stdout, "number of trees %d\n",roots->size);
                }
                fprintf(stdout, "------------------------------------------------------------\n");
                bistree_fancy(t);
            }
        } 
        else { // remove nodes
             /* skip if there are no roots to manipulate */
            if (list_isempty(roots))
                continue;
            /* otherwise, choose random root to operate on */
            if(VERBOSE >= 2){
                fprintf(stdout, "------------------------------------------------------------\n");
            }
            int index = 0;
            BisTree* chosen;
            if(MONO) {
                chosen = list_get(roots, 0);
            }
            else {
                index = random() % list_size(roots);
                chosen  = list_get(roots, index);
            }
            int number_nodes = bistree_size(chosen);
            int number_tries = random() % number_nodes;
            if(VERBOSE >= 1){
                fprintf(stdout, "Deleting at most %d nodes from tree %d\n",number_tries, index);
            }
            for(int i = 0; i < number_tries; i++ ) {
                /* remove key from tree if key exists in it */
                /* this is checked in bistree_remove */
                bistree_remove(chosen, random() % MAX_KEY_VALUE);
            }
            if(VERBOSE >=2){
                fprintf(stdout, "tree size is %d\n", bistree_size(chosen));
                fprintf(stdout, "to clean %d\n", chosen->to_clean);
                if(!MONO){
                    fprintf(stdout, "number of trees %d\n",roots->size);
                }

                fprintf(stdout, "------------------------------------------------------------\n");
                bistree_fancy(chosen);
            }
        }
        mutate -= 1;
    }
    printf("Finished Iterations\n");
}

void setup(){
    heap  = (Heap*)malloc(sizeof(Heap));
    roots = (List*)malloc(sizeof(List));

    heap_init(heap, HEAP_SIZE, collect_garbage);
    list_init(roots);

    srandom(getpid());
    mutate = ITERATIONS;

    /* build first bistree */
    BisTree* t = (BisTree*)malloc(sizeof(BisTree));
    bistree_init(t);
    list_addlast(roots,t);
    int number_nodes = MIN_NODES + random() % (MAX_NODES - MIN_NODES);
    for(int i = 0; i < number_nodes; i++) {
        bistree_insert(t, random() % MAX_KEY_VALUE);
    }
}

float opts(int argc, char** argv){
    struct option long_options[] = {
        {"mono", no_argument, &MONO, 'm'},
        {"heap-size", required_argument, NULL, 's'},
        {"algo", required_argument, NULL, 'a'},
        {"iterations", required_argument, NULL, 'i'},
        {"help", no_argument, NULL, 'h'},
        {"verbose",optional_argument, NULL, 'v'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "ha:i:s:v::", long_options, NULL)) != -1) {
        switch (opt) {
            case 0:
                break;
            case 's':
                HEAP_SIZE = atoi(optarg);
                int res =  HEAP_SIZE % (sizeof(_block_header) + sizeof(BiTreeNode)) ;
                if (HEAP_SIZE == 0) {
                    fprintf(stderr,"Heap size should be positive\n");
                    exit(EXIT_FAILURE);
                }
                else if (res != 0) {
                    fprintf(stderr,"Heap Size should be a multiple of 40 (size of BlockHeader + size of BiTreeNode)\n");
                    fprintf(stderr,"Try Again with %d\n",HEAP_SIZE-res);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'a':
                ALGO = atoi(optarg);
                if (ALGO < 1 || ALGO > 3) {
                    fprintf(stderr, "Invalid value for --algo. Must be 1, 2, or 3.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                VERBOSE = 1;
                if (optarg != NULL) { // Check if optional argument is provided
                    VERBOSE = atoi(optarg);
                } 
                if (VERBOSE < 1 || VERBOSE > 3) {
                    fprintf(stderr, "Invalid verbosity level. Must be between 1 and 3.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'i':
                ITERATIONS = atoi(optarg);
                break;
            case 'h':
                print_usage(stdout, argv[0]);
                exit(EXIT_SUCCESS);
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected threshold argument after options\n");
        print_usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    float threshold = atof(argv[optind]);  /* a value in the interval (0,1) */
    return threshold;
}

int main(int argc, char** argv) {
    /*
    * ^C to stop the program 
    */
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        fprintf(stderr,"signal: cannot install handler for SIGINT\n");
        return 1;
    }
    
    float threshold = opts(argc, argv);
    setup();
    main_loop(threshold);

    /* caught ^C ! */
    printf("Exiting Gracefully :)\n");
    heap_destroy(heap);
    return 0;
}
