#ifndef MARK_COLLECT_ALGO
#define MARK_COLLECT_ALGO

#include <stdio.h>
#include "list.h"

void* mark_collect_malloc(unsigned int nbytes);
int mark_collect_gc(List* roots);

#endif
