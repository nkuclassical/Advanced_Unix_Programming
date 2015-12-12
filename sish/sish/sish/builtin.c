//
//  builtin.c
//  sish
//
//  Created by Yanqiao Zhan on 12/10/15.
//  Copyright © 2015 Yanqiao Zhan. All rights reserved.
//
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


#include "builtin.h"

int cdCommand(char *path){
    /*
     Do we need to check '~','.','..'
     */
    if(access(path,R_OK)!=0){
        fprintf(stderr,"Can't access %s\n",path);
        return -1;
    }
    if(chdir(path) < 0){
        fprintf(stderr,"Change to directory: %s failed!\n",path);
        return -1;
    }
    /*
     return 0 if change directory successfully.
     */
    return 0;
}
void echoCommand(char *message){
    pid_t pid;
    
    if (strcmp(message,"$$") == 0) {
        pid = getpid();
       /* fprintf(stderr, pid); */
        return;
        
    }
    if (strcmp(message, "$?") == 0) {
        fprintf(stderr, "The exit status of last command is: ");
        return;
    }
    fprintf(stderr,"%s\n", message);
    return;
}


