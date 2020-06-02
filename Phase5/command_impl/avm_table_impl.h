#ifndef AVM_TABLE_IMPL_H
#define AVM_TABLE_IMPL_H

#define TABLE_SIZE 64
#include <stdio.h>
#include "../avm/avm.h"
#include <stdlib.h>
#include <string.h>

avm_table * avm_newTable(){
    avm_table * ret;
    int i;
    ret = (avm_table *) malloc(sizeof(avm_table));
    ret->contentTable = (avm_memcell *) malloc(sizeof(avm_memcell) * TABLE_SIZE);
    ret->indexTable = (avm_memcell *) malloc(sizeof(avm_memcell) * TABLE_SIZE);
    ret->refcounter = 0;
    ret->currIndex = 0;
    for(i = 0; i < TABLE_SIZE; i++) {
        ret->contentTable[i].type = undef_m;
        ret->indexTable[i].type = undef_m;
    }
    return ret;
}

avm_memcell * avm_table_getElement(avm_table * table, avm_memcell * index){
    int i;
    int found = 0;
    for(i = 0; i < table->currIndex; i++){
        if(table->indexTable[i].type == index->type){
            switch(index->type){
                case number_m:  found = table->indexTable[i].data.numVal == index->data.numVal; break;
                case string_m:  found = strcmp(table->indexTable[i].data.strVal, index->data.strVal) == 0; break;
                case bool_m:    found = table->indexTable[i].data.boolVal == index->data.boolVal; break;
                default: printf("ERROR\n"); //TODO: AVM_ERROR
            }
            
        }
        if(found) return &(table->contentTable[i]);
        
    }
    return NULL;
}


void avm_table_setElement(avm_table * table, avm_memcell * index, avm_memcell * content){
    //TODO: CHECK INDEX FOR TYPE OF NUM OR STRING.
    avm_memcell * elem = avm_table_getElement(table, index);
    if(elem){
        avm_assign(elem, content);
    }
    else {
        avm_assign(&(table->indexTable[table->currIndex]), index);
        avm_assign(&(table->contentTable[table->currIndex]), content);
        table->currIndex++;
    }
}

void print_table(avm_table * table){
    
}

#endif