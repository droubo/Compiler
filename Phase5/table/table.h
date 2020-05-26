/**
 * CS-340 Compilers
 * Spring 2020
 * 
 * Project Phase 5: AVM Implementation
 * 
 * Antonis Droubogiannis    csd4014@csd.uoc.gr
 * Georgios Marios Kokiadis csd3962@csd.uoc.gr
 * Nikolaos Gounakis        csd3932@csd.uoc.gr
 */ 
#ifndef TABLE_H
#define TABLE_H

#include "../avm/avm.h"

void avm_refcounter_incr(avm_table * t){
    t->refcounter++;
}

void avm_refcounter_decr(avm_table * t){
    t->refcounter--;
}

#endif