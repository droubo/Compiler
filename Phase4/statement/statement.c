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
 * statement.c
 * Statement manager
 */

#include "statement.h"
#include <stdlib.h>
#include <stdio.h>
#include "../quads/quads.h"

void make_stmt(stmt_t *s){
	s->breakList = s->contList = 0;
}

int newlist(int i){
	quads[i].label = 0;
	return i;
}

int mergelist(int l1, int l2){
	if(!l1) return l2;
	else if(!l2) return l1;
	else {
		int i = l1;
		while(quads[i].label)
		{
			i = quads[i].label;
		}
		quads[i].label = l2;
		return l1;
	}
}

void patchlist(int list, int label){
	while(list){
		int next = quads[list].label;
		quads[list].label = label;
		list = next;

	}
}

void push_lpstack(loopcounterstack **lcs, int loopcounter){
	loopcounterstack *tmp = (loopcounterstack*)malloc(sizeof(loopcounterstack));
	tmp->flag = loopcounter;
	tmp->next = (*lcs);
	(*lcs) = tmp;
}
int pop_lpstack(loopcounterstack **lcs){
	loopcounterstack *tmp = (*lcs);
	if(lcs != NULL) (*lcs) = (*lcs)->next;
	if(tmp == NULL) return 0;
	int result = tmp->flag;
	free(tmp);
	return result;
}
