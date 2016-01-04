CFLAGS=-g -D_BSD_SOURCE -Wall -pedantic-errors
sws: sws.o net.o cgi.o stack.o process.o cgi.o dir.o normalfile.o util.o 
	$(CC)  -o sws sws.o net.o cgi.o stack.o  normalfile.o dir.o process.o util.o 
sws.o: sws.c net.h define.h process.h
	$(CC) -c ${CFLAGS} sws.c
net.o: net.c net.h stack.h define.h process.h
	$(CC) -c ${CFLAGS} net.c
process.o:process.c process.h cgi.h normalfile.h dir.h define.h
	$(CC) -c ${CFLAGS} process.c
cgi.o:cgi.c cgi.h define.h
	$(CC) -c ${CFLAGS} cgi.c
normalfile.o:normalfile.c normalfile.h define.h util.h
	$(CC) -c ${CFLAGS} normalfile.c
dir.o:dir.c dir.h define.h util.h
	$(CC) -c ${CFLAGS} dir.c
util.o:util.c util.h define.h
	$(CC) -c ${CFLAGS} util.c
stack.o: stack.c stack.h 
	$(CC) -c ${CFLAGS} stack.c
clean: 
	rm *o sws
