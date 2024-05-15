#ifndef MARK_SWEEP_ALGO
#define MARK_SWEEP_ALGO

#include <stdio.h>
#include "list.h"

void* mark_sweep_malloc(unsigned int nbytes);
int mark_sweep_gc(List* roots);

#endif
