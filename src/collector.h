/*
 * collector.h
 */

#ifndef COLLECTOR_H
#define COLLECTOR_H

#include "list.h"

void collect_garbage(List* roots);

void generational_gc(List*roots, ...);

#endif
