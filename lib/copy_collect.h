#ifndef COPY_COLLET_ALGO
#define COPY_COLLET_ALGO

#include <stdio.h>
#include "list.h"

void* copy_collect_malloc(unsigned int nbytes);
int copy_collect_gc(List* roots);

#endif
