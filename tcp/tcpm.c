/*
 * tcpm --trivially copy a file via mmap/memcpy
 * Yanqiao Zhan 
 * CS631
 * Stevens Institute of Technology
 */

#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

int 
main(int argc, char**argv){
    if(argc!=3){
        printf("usage: %s <sourceFile><destinationFile>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int sfd,dfd;
    char *sourceFile=argv[1],*destFile=argv[2];
    if(strcmp(sourceFile,destFile)==0){
        printf("tcpm: %s and %s are identical (not copied).\n",sourceFile, destFile);
        exit(EXIT_FAILURE);
    }
    if(strcmp(sourceFile,".")==0||strcmp(sourceFile,"..")==0||strcmp(destFile,".")==0||strcmp(destFile,"..")==0){
        printf("One of %s and %s is illegal, ( . and .. are not allowed )",sourceFile,destFile);
        exit(EXIT_FAILURE);
    }
    struct stat sb;
    if((sfd=open(sourceFile,O_RDONLY))<0){
        fprintf(stderr,"Unable to open source file %s\n",sourceFile);
        exit(EXIT_FAILURE);
    }
    if(stat(sourceFile,&sb)==-1){
        perror("stat error");
        exit(EXIT_FAILURE);
    }

    while((dfd=open(destFile,O_RDWR|O_CREAT|O_TRUNC,sb.st_mode))==-1){
        if(errno==EISDIR){
            char*fileName=basename(sourceFile);
            destFile=strcat(destFile,"/");
            destFile=strcat(destFile,fileName);
        }else{
             fprintf(stderr,"Unable to open/write '%s':%s\n",destFile,strerror(errno));
             exit(EXIT_FAILURE);
        }
    }

    if(ftruncate(dfd,sb.st_size)<0){
        fprintf(stderr,"Ftruncate error\n");
        exit(EXIT_FAILURE);
    }

    off_t fsz=0;
    size_t copysz;
    void *src,*dst;
    while(fsz<sb.st_size){
        copysz=sb.st_size-fsz;
        if((src=mmap(0,copysz,PROT_READ,MAP_SHARED,sfd,fsz))==MAP_FAILED){
            fprintf(stderr,"mmap error for input!\n");
            exit(EXIT_FAILURE);
        }
        if((dst=mmap(0,copysz,PROT_READ|PROT_WRITE,MAP_SHARED,dfd,fsz))==MAP_FAILED){
            fprintf(stderr,"mmap error for output!\n");
            exit(EXIT_FAILURE);
        }
        memcpy(dst,src,copysz);
        munmap(src,copysz);
        munmap(dst,copysz);
        fsz+=copysz;
    }
    close(dfd);
    close(sfd);
    return EXIT_SUCCESS;
}
