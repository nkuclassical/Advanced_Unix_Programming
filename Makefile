# -Wall in CFLAGS
ls: ls.h ls.c 
	gcc -Wall -o ls ls.c  -lbsd

