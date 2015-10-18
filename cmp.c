#include <sys/types.h>
#include <sys/stat.h>

#include <fts.h>
#include <string.h>

#include "ls.h"
int name_compare(const FTSENT *a, const FTSENT *b){
	return (strcmp(a->fts_name, b->fts_name));
}

int size_compare(const FTSENT *a, const FTSENT *b){
	if(a->fts_statp->st_size < b->fts_statp->st_size)return 1;
	else if(a->fts_statp->st_size > b->fts_statp->st_size) return -1;
	else return name_compare(a,b);
}
int st_ctime_compare(const FTSENT *a, const FTSENT *b){
	if(a->fts_statp->st_ctime < b->fts_statp->st_ctime)return 1;
	else if(a->fts_statp->st_ctime > b->fts_statp->st_ctime)return -1;
	else return name_compare(a,b);
}
int st_mtime_compare(const FTSENT*a, const FTSENT*b){
	if(a->fts_statp->st_mtime < b->fts_statp->st_mtime)return 1;
	else if(a->fts_statp->st_mtime > b->fts_statp->st_mtime)return -1;
	else return name_compare(a,b);
}