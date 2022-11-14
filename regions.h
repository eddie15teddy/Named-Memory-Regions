/**
 * A4
 *
 * COMP 2160 SECTION A02
 * INSTRUCTOR    John Braico
 * ASSIGNMENT    Assignment 4,
 * AUTHOR        Eddie Butkaliuk, 007910587
 * DATE          Dec 13th, 2021
 *
 * PURPOSE:  To make the fucntions in regions.c visible to other files
 */

#ifndef _REGIONS_H
#define _REGIONS_H

#include <stdlib.h>
#include <assert.h>

typedef enum {
  FALSE,
  TRUE
} Boolean;

typedef unsigned long rsize_t;

Boolean rinit(const char *region_name, rsize_t region_size);

Boolean rchoose(const char *region_name);

const char *rchosen(void);

void *ralloc(rsize_t block_size);

rsize_t rsize(void *block_ptr);

Boolean rfree(void *block_ptr);

void rdestroy(const char *region_name);

void rdump(void);

#endif
