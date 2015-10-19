/*
 *
 Name: ls -list directory contents
 Version: 0.1[snapshot version]
 Author: Yanqiao Zhan (yzhan14@stevens.edu)
 Date Oct/4/2015/18:30:13
 Support options:   -A -a -c -F -f -i -k -l -n -r -S -s -t -u -1 -d -h -q -w -R -x -C
 *
 */

#include <bsd/stdlib.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <unistd.h>
#include <fts.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <getopt.h>
#include <ctype.h>
#include <math.h>
#include "ls.h"

int flag_A=0;
int flag_a=0; 
int flag_C=0;
int flag_c=0;
int flag_d=0;
int flag_F=0; 
int flag_f=0;
int flag_h=0;
int flag_i=0;
int flag_k=0;
int flag_l=0;
int flag_n=0;
int flag_q=0;
int flag_R=0;
int flag_r=0;
int flag_S=0;
int flag_s=0;
int flag_t=0;
int flag_u=0;
int flag_w=0;
int flag_x=0;
int flag_1=0;

int termWidth=80;

int success=EXIT_SUCCESS;
int fail=EXIT_FAILURE;

int sortKey=BY_NAME;
int fts_options=FTS_PHYSICAL;

int maxLen=0;
long maxInode=0;
long maxBlock=0;
int BlockSize=512;
int chcnt=0;
int col=0;
int row=0;
int colwidth=0;
int numcols=0;
int moreRow=0;


void usage(void){
	fprintf(stderr,"usage: ls [-AacCdFfhiklnqRrsSTUWXL] [file ...]\n");
	exit(fail);
} 

int name_compare(const FTSENT *a, const FTSENT *b){
	return (strcasecmp(a->fts_name, b->fts_name));
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
int st_atime_compare(const FTSENT*a, const FTSENT*b){
	if(a->fts_statp->st_atime < b->fts_statp->st_atime)return 1;
	else if(a->fts_statp->st_atime > b->fts_statp->st_atime)return -1;
	else return name_compare(a,b);
}

int r_name_compare(const FTSENT *a, const FTSENT *b){
	return -(strcasecmp(a->fts_name, b->fts_name));
}

int r_size_compare(const FTSENT *a, const FTSENT *b){
	if(a->fts_statp->st_size > b->fts_statp->st_size)return 1;
	else if(a->fts_statp->st_size < b->fts_statp->st_size) return -1;
	else return r_name_compare(a,b);
}
int r_st_ctime_compare(const FTSENT *a, const FTSENT *b){
	if(a->fts_statp->st_ctime > b->fts_statp->st_ctime)return 1;
	else if(a->fts_statp->st_ctime < b->fts_statp->st_ctime)return -1;
	else return r_name_compare(a,b);
}
int r_st_mtime_compare(const FTSENT*a, const FTSENT*b){
	if(a->fts_statp->st_mtime > b->fts_statp->st_mtime)return 1;
	else if(a->fts_statp->st_mtime < b->fts_statp->st_mtime)return -1;
	else return r_name_compare(a,b);
}
int r_st_atime_compare(const FTSENT*a, const FTSENT*b){
	if(a->fts_statp->st_atime > b->fts_statp->st_atime)return 1;
	else if(a->fts_statp->st_atime < b->fts_statp->st_atime)return -1;
	else return r_name_compare(a,b);
}
void print_time(time_t rawtime){
	int i;
	const char *t;
	if ((t = ctime(&rawtime)) == NULL) {
		t = "????????????????????????";
	}
	for (i = 4; i < 11; i++){
		(void)putchar(t[i]);
	}
	
	int sixMonths;
    sixMonths=(366/2)*60*60*24;

	if(time(0)+sixMonths>rawtime &&time(0)-sixMonths<rawtime){
		for(i = 11; i < 16; i++)putchar(t[i]);
	}else{
		putchar(' ');
		for(i = 20; i < 24; i++)putchar(t[i]);
	}
	putchar(' ');
}
/**
p is the parent directory of the display list
**/
int print_name(FTSENT*p){

	char temp[1024];
	int chcnt=0;
	strcpy(temp,p->fts_name);
	if(flag_q==1||flag_w==0){
		int i=0;
		for(i=0;i<strlen(temp);i++){
			if(!isprint(temp[i])){
				temp[i]='?';
			}
		}
	}
	chcnt+=printf("%s", temp);
	
	if(flag_F){				//-f display a slash ...
		struct stat *sb  = p->fts_statp;
			switch(sb->st_mode & S_IFMT){
			case S_IFDIR:
				chcnt+=printf("/");
				return chcnt;
			case S_IFLNK:
				chcnt+=printf("@");
				return chcnt;
			case S_IFWHT:
				chcnt+=printf("%%");
				return chcnt;
			case S_IFSOCK:
				chcnt+=printf("=");
				return chcnt;
			case S_IFIFO:
				chcnt+=printf("|");
				return chcnt;
			}
		if(sb->st_mode & (S_IXUSR|S_IXGRP|S_IXOTH))
			chcnt+=printf("*");

	}

	return chcnt;
	
}



void print_simple(FTSENT*p){
	if(flag_i){
		printf("%ju ", p->fts_statp->st_ino);
	}
	if(flag_s)
		printf("%ju ", p->fts_statp->st_blocks*512/BlockSize);
	print_name(p);
	printf("\n");
}

void print_across(FTSENT*p){ /*-x*/
	chcnt=0;
	if (col >= numcols) {
		chcnt = col = 0;
		(void)putchar('\n');
	}
	if(flag_i){
		chcnt+=printf("%ju ", p->fts_statp->st_ino);
	}
	if(flag_s)
		chcnt+=printf("%ju ", p->fts_statp->st_blocks*512/BlockSize);
	chcnt+=print_name(p);
	chcnt+=printf(" ");
	
	while (chcnt++ < colwidth)
		(void)putchar(' ');
	col++;
	
}

void print_simple_pro(FTSENT*p){
	chcnt=0;
	if(moreRow!=0&&row>=moreRow&&col>=numcols-1){
		chcnt = col = 0;
		(void)putchar('\n');
		row++;
	}
	if (col >= numcols) {
		chcnt = col = 0;
		(void)putchar('\n');
		row++;
	}
	if(flag_i){
		chcnt+=printf("%ju ", p->fts_statp->st_ino);
	}
	if(flag_s)
		chcnt+=printf("%ju ", p->fts_statp->st_blocks*512/BlockSize);
	chcnt+=print_name(p);
	chcnt+=printf(" ");
	if (col >= numcols) {
			chcnt = col = 0;
			(void)putchar('\n');
			row++;
	}
	while (chcnt++ < colwidth)
		(void)putchar(' ');
	col++;
}

void print_long(FTSENT *p){
	struct stat *sp;
	sp = p->fts_statp;

	
	if(flag_i)
		printf("%ju \t", sp->st_ino);
	if(flag_s)
		printf("%ju \t", sp->st_blocks*512/BlockSize);
	
	char perm[SIZE];
	strmode(sp->st_mode, perm);
	(void)printf("%s ", perm);
	(void)printf("%ju \t", sp->st_nlink);
	
	struct passwd *pw = getpwuid(sp->st_uid);
    struct group *gr = getgrgid(sp->st_gid);
	if(flag_n){
		(void)printf("%u\t", pw->pw_uid);
		(void)printf("%u\t", gr->gr_gid);
	}else{
		(void)printf("%s\t", pw->pw_name);	
		(void)printf("%s\t", gr->gr_name);
	}

	if(flag_k){
		(void)printf("%d\t", ((int)sp->st_size)/1024+ ((((int)sp->st_size)%1024 > 512)?1:0));
	}else if(flag_h){
		char *unit={" KMGT"};
		int i=0;
		int fileSize=((int)sp->st_size);
		while(fileSize>1024){
			fileSize/=1024;
			i++;
		}
		(void)printf("%d%c\t", fileSize,unit[i]);
	}else(void)printf("%d\t", (int)sp->st_size);
	
	if(flag_u){
		(void)print_time(sp->st_atime);
	}
	else if(flag_c){
		(void)print_time(sp->st_ctime);
	}
	else {
		(void)print_time(sp->st_mtime);
	}
	

	(void)print_name(p);
	if((sp->st_mode & S_IFMT) == S_IFLNK){
		char lnk[SIZE], tmp[SIZE];
		int res;
		sprintf(lnk, "%s/%s", p->fts_path, p->fts_accpath);
		if((res = readlink(lnk, tmp, sizeof(lnk))) == -1){
			printf("error!\n");
			exit(EXIT_FAILURE);
		}
		else
			printf(" -> %s", tmp);
	}
	printf("\n");
}

void display(FTSENT *p, FTSENT *list){
	
	FTSENT*cur;
	int entriesNum;
	int num;
	int Row;
	int Col;
	int numrows;
	entriesNum=0;
	maxLen=0;
	numcols=0;
	colwidth=0;
	num=0;
	Row=0;
	Col=0;
	numrows=0;


	for(cur = list; cur; cur = cur->fts_link){
		
		if(p == NULL && cur->fts_info ==FTS_D && !(flag_d)){
			cur->fts_number = NO_PRINT;
			continue;
		}
		if( p && cur->fts_name[0] == '.' && !(flag_a) ){ 
			cur->fts_number = NO_PRINT;
			continue;
		}
		if(cur->fts_namelen>maxLen){
			maxLen=cur->fts_namelen;
		}
		entriesNum++;
		
	}
	if(flag_x||flag_C){
		colwidth = maxLen;
		if (flag_i)
			colwidth +=  6+1;
		if (flag_s) {
			if (flag_h)
				colwidth += 6+1;
			else
				colwidth += 10+1;
		}

		if (flag_F)
			colwidth += 1;
		colwidth += 1;
		
		if (termWidth < 2 * colwidth) {
			numcols=1;
			numrows=entriesNum;

		}else{
			numcols = termWidth / colwidth;
			colwidth = termWidth / numcols;		/* spread out if possible */
			numrows = (int)(ceil((double)entriesNum / numcols));
			chcnt = col = 0;

		}
	}

	if(flag_C){
		if(numrows>0){
		FTSENT *array[32768], *newarray[32768];
		for(cur=list;cur;cur=cur->fts_link){
			if(cur->fts_number==NO_PRINT)continue;
			newarray[num++]=cur;
		
		}
	while(entriesNum - numrows*(numcols-1) <=0){
			numcols--;
			numrows = (int)(ceil((double)entriesNum / numcols));
		}
		
		num=0;
		
		moreRow=entriesNum - numrows*(numcols-1);
		int base=0;
		if(moreRow==0){
			for(Row=0;Row<numrows;Row++){
				for(Col=0;Col<numcols;Col++){
					array[base++]=newarray[Col*numrows+Row];
				}
			}	
		}else{
			for(Row=0;Row<moreRow;Row++){
				for(Col=0;Col<numcols;Col++){
					array[base++]=newarray[Col*numrows+Row];
				}
			}
			
			for(Row=moreRow;Row<numrows;Row++){
				for(Col=0;Col<numcols-1;Col++){
					array[base++]=newarray[Col*numrows+Row];
				}
			}
		}
		
		row=0;
		for(num=0;num<entriesNum;num++){
			printfcn(array[num]);	
		}
		printf("\n");
		return;
		}
	}


	
	for(cur = list; cur; cur = cur->fts_link){
		if(cur->fts_number == NO_PRINT)continue;
		printfcn(cur);
	}
	if(printfcn==print_across)
	printf("\n");
	
	
}
int cmp(const FTSENT **a, const FTSENT **b){
	int a_info, b_info;
	a_info = (*a)->fts_info;
	b_info = (*b)->fts_info;
	if(a_info == FTS_ERR || b_info == FTS_ERR || 
	   a_info == FTS_DNR || b_info == FTS_DNR)
		return 0;
	if(a_info == FTS_NS && b_info == FTS_NS)
		return name_compare(*a, *b);
	else if(a_info == FTS_NS || b_info == FTS_NS)
		return (a_info==FTS_NS)?1:-1;
	
	return (sortfcn(*a, *b));
}

void traverse(int argc, char *argv[], int options){
	FTSENT *p;
	FTS *ftsp;
	if ((ftsp =fts_open(argv, options, flag_f ? NULL : cmp)) == NULL){
		printf("fts_open failed!\n");
		exit(EXIT_FAILURE);
	}
	int has_print=0;
	display(NULL, fts_children(ftsp, 0));
	if(flag_d){
		fts_close(ftsp);
		return;
	}

	while ((p = fts_read(ftsp)) != NULL) {
		switch(p->fts_info) {
			case FTS_DC:
				(void)fprintf(stderr, "%s: Directory cycle detected.",
				  p->fts_name);
				exit(1);	
				break;
			case FTS_ERR:
			case FTS_DNR:
				(void)fprintf(stderr, "%s: FTS directory error.",
				  p->fts_name);
				break;
			case FTS_D:
				/*- -a and -A option -*/
				if(p->fts_level != FTS_ROOTLEVEL && 
			   		p->fts_name[0] == '.' && !flag_a ) {
					fts_set(ftsp, p, FTS_SKIP);
					break;
				}
				if(has_print)
					(void)printf("\n%s\n", p->fts_path);
				else if (argc > 1){
					(void)printf("%s\n", p->fts_path);
				}
				display(p, fts_children(ftsp, 0));
				if(!flag_R){
					fts_set(ftsp, p, FTS_SKIP);
				}
				has_print = 1;
					break;	
			default:
				break;
			}
	}
	fts_close(ftsp);
}


int main(int argc, char*argv[]){

	if(argc==0)return 0;
	char ch;
	/* struct winsize win;*/
	char *defualt_dir[] = {".", NULL};
	flag_A = (getuid()==0)?1:0;
	if(isatty(STDOUT_FILENO)){ /*Output is terminal*/
		
		flag_q=1;
	}else{
		flag_w=1;  /*Output is not terminal*/
	}

	flag_C=1;
	
	while ((ch = getopt(argc, argv, "AacCdFfhiklnqRrSstuwx1"))!= -1) {
		switch(ch){
			case 'w':
				flag_w=1;
				flag_q=0;
				break;
			case 'q':
				flag_w=0;
				flag_q=1;
				break;
			case 'l':
				flag_l=1;
				flag_C=0;
				flag_1=0;
				flag_n=0;
				flag_x=0;
				break;
			case 'C':
				flag_l=0;
				flag_C=1;
				flag_1=0;
				flag_n=0;
				flag_x=0;
				break;
			case '1':
				flag_l=0;
				flag_C=0;
				flag_1=1;
				flag_n=0;
				flag_x=0;
				break;
			case 'n':
				flag_l=1;
				flag_C=0;
				flag_1=0;
				flag_n=1;
				flag_x=0;
				break;
			case 'x':
				flag_l=0;
				flag_C=0;
				flag_1=0;
				flag_n=0;
				flag_x=1;
				break;
			case 'c':
				flag_c=1;
				flag_u=0;
				sortKey = BY_TIME;
				break;
			case 'u':
				flag_c=0;
				flag_u=1;
				sortKey = BY_TIME;
				break;
			case 'A':
				flag_A=1;
				break;
			case 'a':
				flag_a=1;
				fts_options = FTS_SEEDOT;
				break;
			case 'd':
				flag_d=1;
				flag_R=0;
				break;
			case 'F':
				flag_F=1;
				break;
			case 'f':
				flag_f=1;
				break;
			case 'h':
				flag_h=1;
				flag_k=0;
				break;
			case 'i':
				flag_i=1;
				break;
			case 'k':
				flag_k=1;
				flag_h=0;
				break;
			case 'R':
				flag_R=1;
				break;
			case 'r':
				flag_r=1;
				break;
			case 'S':
				sortKey=BY_SIZE;
				flag_S=1;
				break;
			case 's':
				flag_s=1;
				if(getenv("BLOCKSIZE")==NULL){
					BlockSize=512;
				}else{
					BlockSize=atoi(getenv("BLOCKSIZE"));
				}
				break;
			case 't':
				sortKey=BY_TIME;
				flag_t=1;
				break;
			default:
				usage();
			
		}
	}

	argc-=optind;
	argv+=optind;
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	if(w.ws_row!=0){
		termWidth=w.ws_col;
	}


	if(flag_l){
		printfcn = print_long;
	}else if(flag_x){
		printfcn = print_across;
	}else if(flag_C){
		printfcn=print_simple_pro;
	}else {
		printfcn = print_simple;
	}

	if(flag_r){
		switch(sortKey){
			case BY_TIME:
				if(flag_c) sortfcn = r_st_ctime_compare;
				else if(flag_t) sortfcn = r_st_mtime_compare;
				else if(flag_u) sortfcn = r_st_atime_compare;
				else sortfcn = r_name_compare;
				break;
			case BY_SIZE:
				sortfcn = r_size_compare;
				break;
			default:
				sortfcn = r_name_compare;
		}
	}else{
		switch(sortKey){
			case BY_TIME:
				if(flag_c) sortfcn = st_ctime_compare;
				else if(flag_t) sortfcn = st_mtime_compare;
				else if(flag_u) sortfcn = st_atime_compare;
				else sortfcn = name_compare;
				break;
			case BY_SIZE:
				sortfcn = size_compare;
				break;
			default:
				sortfcn = name_compare;
		}
	}
	
	if(argc>0)
		traverse(argc,argv,fts_options);
	else traverse(1, defualt_dir, fts_options);

	return success;
}
