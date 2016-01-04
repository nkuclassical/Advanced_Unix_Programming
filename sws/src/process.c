/*
   process.c
   finalxcode

   Created by Yanqiao Zhan on 11/28/15.
   Copyright © 2015 Yanqiao Zhan. All rights reserved.
   */

#include "process.h"

/**
  if error occurs, return -1;
  else execute a certain process module and return 1;
 **/


/* int proc(NetArg*serverArg,NetRequest*clientArg){ */
int proc(NetArg*serverArg,NetRequest*clientArg,char*reqLine){
    if(clientArg->filetype==2){ /*cgi file*/
        cgiexe(clientArg,serverArg,reqLine);
    }else if(clientArg->filetype==1){ /*normal file*/
        normalfileexe(clientArg,serverArg,reqLine);
    }else if(clientArg->filetype==3){ /*print directory tree*/
        direxe(clientArg, serverArg, reqLine);
    }else { /* error input*/
        /* fprintf(stderr, "[ERROR]request file path error!\n"); */
        clientArg->head->status_code=404;
        if ( serverArg->flag_l ==1 ) {
            sendtoLog(serverArg, clientArg, reqLine);
        }else {
            fprintf(stderr, "[ERROR]Request file path error!\n");
        }
        sendtoClientHead(serverArg,clientArg,reqLine);
        return -1;
    }
    return 1;
}
