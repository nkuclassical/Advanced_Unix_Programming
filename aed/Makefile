CFLAGS= -Wall 
SSL=-lcrypto -lssl
aed: aed.o 
	$(CC) -o aed aed.o -lbsd ${SSL}
aed.o:aed.c
	$(CC) -c ${CFLAGS} aed.c
clean: 
	rm *o aed
