#ifndef _STATEMENTSLIST_H
#define _STATEMENTSLIST_H

typedef struct stmt_t{
    int breakList, contList;

} stmt_t;
typedef struct loopcounterstack{
	int flag;
	struct loopcounterstack* next;
} loopcounterstack;

void make_stmt(stmt_t *s);
int newlist(int i);
int mergelist(int l1, int l2);
void patchlist(int list, int label);
void push_lpstack(loopcounterstack **lcs, int loopcounter);
int pop_lpstack(loopcounterstack **lcs);

#endif
