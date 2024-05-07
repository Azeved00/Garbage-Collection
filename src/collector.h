/*
 * collector.h
 */

#ifndef COLLECTOR_H
#define COLLECTOR_H

#include "list.h"

void mark_sweep_gc(List* roots);

void mark_compact_gc(List* roots);

void copy_collection_gc(List* roots);

void generational_gc(List*roots, ...);

#endif
