/*
 * tcp --trivially copy a file
 * Yanqiao ZHAN
 * CS631
 * Stevens Institute of Technology
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#define BUFFSIZE	4096

int
main(int argc, char **argv) {
    if(argc!=3){
        printf("usage: %s <sourceFile><destinationFile>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    int sfd,dfd;
	char buf[BUFFSIZE];
	char *sourceFile=argv[1],*destFile=argv[2];
    if(strcmp(sourceFile,destFile)==0){
        printf("tcp: %s and %s are identical (not copied).\n",sourceFile, destFile);
        exit(EXIT_FAILURE);
    }
    if(strcmp(sourceFile,".")==0||strcmp(sourceFile,"..")==0||strcmp(destFile,".")==0||strcmp(destFile,"..")==0){
        printf("tcp: One of %s and %s is illegal (not allow . or ..)\n",sourceFile,destFile);
        exit(EXIT_FAILURE);
    }
    struct stat sb;
	if ((sfd = open(sourceFile, O_RDONLY)) == -1) {
		fprintf(stderr, "Unable to open '%s': %s\n",
				sourceFile, strerror(errno));
		exit(EXIT_FAILURE);
	}
    if(stat(sourceFile,&sb)==-1){
        perror("stat");
        exit(EXIT_FAILURE);
    }
    while((dfd=open(destFile,O_RDWR|O_CREAT|O_TRUNC,sb.st_mode))==-1){
        if(errno==EISDIR){
            char *fileName=basename(sourceFile);
            destFile=strcat(destFile,"/");
            destFile=strcat(destFile,fileName);
        }else{
            fprintf(stderr,"Unable to open/write '%s':%s\n",destFile,strerror(errno));
            exit(EXIT_FAILURE);
         }
    }

    ssize_t sfd_t,dfd_t;
    while((sfd_t=read(sfd,buf,BUFFSIZE))>0){
        dfd_t=write(dfd,buf,sfd_t);
        if(dfd_t!=sfd_t)
        {
            fprintf(stderr," Error during write");
            exit(EXIT_FAILURE);
        }
    }
	close(dfd);
	close(sfd);
    return EXIT_SUCCESS;
}

