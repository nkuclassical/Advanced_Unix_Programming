
#define NO_PRINT	1
#define BY_NAME     10
#define BY_SIZE     11
#define BY_TIME     12
#define SIZE        1024

static int (*sortfcn)(const FTSENT*, const FTSENT*);
static void (*printfcn)(FTSENT *);
void usage(void);
int name_compare(const FTSENT*a, const FTSENT*b);
int size_compare(const FTSENT*a, const FTSENT*b);
int st_ctime_compare(const FTSENT*a, const FTSENT*b);
int st_mtime_compare(const FTSENT*a, const FTSENT*b);
int st_atime_compare(const FTSENT*a, const FTSENT*b);
int r_name_compare(const FTSENT *a, const FTSENT *b);
int r_size_compare(const FTSENT *a, const FTSENT *b);
int r_st_ctime_compare(const FTSENT *a, const FTSENT *b);
int r_st_mtime_compare(const FTSENT*a, const FTSENT*b);
int r_st_atime_compare(const FTSENT*a, const FTSENT*b);
void print_time(time_t rawtime);
int print_name(FTSENT*p);
void print_simple(FTSENT*p);
void print_long(FTSENT *p);
void display(FTSENT *p, FTSENT *list);
int compare(const FTSENT **a, const FTSENT **b);
void traverse(int argc, char *argv[], int options);


#define	S_IFWHT  0160000		/* whiteout */


