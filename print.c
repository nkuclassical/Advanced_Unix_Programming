#include <stdio.h>
#include "ls.h"
#define SIZE 1024
void printname(FTSENT*p, int len){
	int i;
	char tmp[SIZE];
	strcpy(tmp, p->fts_name);
	printf("%s\n", tmp);
}

void printscol(FTSENT*p){
	struct stat *sp;
	sp = p->fts_statp;
	LEN *c = (LEN*)p->fts_pointer;
	printname()
}
