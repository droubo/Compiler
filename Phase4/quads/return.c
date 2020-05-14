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
 */

#include "return.h"

returnStack* newReturnStack(int key, int value){
    returnStack* temp;
    temp = (returnStack*) malloc(sizeof(returnStack));
    temp->key = key;
    temp->value = value;
    temp->next = NULL;
    return temp;

}