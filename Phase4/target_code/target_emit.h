/**
 * CS-340 Compilers
 * Spring 2020
 * 
 * Project Phase 3: Intermediate code Generation
 * 
 * Antonis Droubogiannis    csd4014@csd.uoc.gr
 * Georgios Marios Kokiadis csd3962@csd.uoc.gr
 * Nikolaos Gounakis        csd3932@csd.uoc.gr
 * 
 * quads.h
 * Implementation of quad and expression data
 * structures, used to create intermediate code.
 */

#ifndef _TARGET_EMIT_H
#define _TARGET_EMIT_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "target.h"

#define VAR_TRUE_I 1
#define VAR_FALSE_I 0

#define EXPAND_SIZE_I 1024
#define CURR_SIZE_I (total*sizeof(instruction))
#define NEW_SIZE_I (EXPAND_SIZE * sizeof(instruction) + CURR_SIZE)


#endif
