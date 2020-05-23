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