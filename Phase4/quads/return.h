/**
 * CS-340 Compilers
 * Spring 2020
 * 
 * Project Phase 3: Intermediate code Generation
 * 
 * Antonis Droubogiannis    csd4014@csd.uoc.gr
 * Georgios Marios Kokiadis csd3962@csd.uoc.gr
 * Nikolaos Gounakis        csd3932@csd.uoc.gr
 */

#ifndef _RETURN_H
#define _RETURN_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


typedef struct returnStack{
	int key;
	int value;
        struct returnStack* next;
} returnStack;


returnStack* newReturnStack(int key, int value);

#endif