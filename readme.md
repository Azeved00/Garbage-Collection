# Simple Implementation Garbage Collector
Implementation of 3 simple Garbage Collection(GC) Algorithms.
The Algorithms are:
1. [Mark & Sweep](./src/mark_sweep.c)
2. [Mark & Compact](./src/mark_compact.c)
2. [Copy & Collect](./src/copy_collect.c)

## Compile The Program
To compile the program use the command:
```
make -f makefile all
```

## Running the Program
Runing the code is done by using the code

```
    ./bin/mutator [--help]  [--verbose[<1|2>]] [--mono] [--algo <1|2|3>] [--iterations <itererations>] [--heap-size <heap_size>] <threshold>
```

- `-h` or `--help`: Display the usage message.
- `-v` or `--verbose`: Display extra messages as such:
  - no verbosity : Display only messages when the GC starts running and termination messages
  - verbosity 1: Display the operations (add and remove nodes), as well as the number of blocks cleaned by the GC 
  - verbosity 2: Display all information, tree size, number of nodes to be cleaned in the tree, the tree and
    GC phases.
    
  Note that: there is no space between the flag and the number of verbosity `-v1` and `-v2` is correct, `-v 1` is **not**
- `--mono`: Use only 1 tree for all operations.
- `-a` or `--algo`: use x algorithm for GC,
  - `1`: Mark & Sweep
  - `2`: Mark & Compact
  - `3`: Copy & Collect
    
  the default value is 1, a.k.a., Mark & Sweep
- `-i` or `--iterations`: the limit number of base operations (add and remove nodes from a tree),
  if you give a negative number it will be infinite,
- `-s` or `--heap-size`: Change the heap_size,
  the default is 1000 bytes,
  the heap-size should be a multiple of `(sizeof(BisTreeNode) + sizeof(_block_header)`. 
- `threshold`: a float like `0.9`, this is a required parameter 
 
## CleanUp
Run the following command to do cleanup:
```
 make -f makefile clean
```
