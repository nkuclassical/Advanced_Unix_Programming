/*
   normalfile.c
   finalxcode

   Created by Yanqiao Zhan on 11/28/15.
   Copyright © 2015 Yanqiao Zhan. All rights reserved.
   */

#include "normalfile.h"
/**
  return value explain:
  -1 -> error
  1  -> success
 **/
int normalfileexe(NetRequest*netRequest,NetArg*netArg,char*reqLine){
    int fd,returnvalue=0;
    if(access(netRequest->requestdir,R_OK)!=0){
        netRequest->head->status_code=403;
        returnvalue = -1;
    }
    if(returnvalue>=0){
        if((fd=open(netRequest->requestdir, O_RDONLY))<0){
            netRequest->head->status_code=403;
            returnvalue= -1;
        }
    }

    if(netArg->flag_l==1){
        sendtoLog(netArg,netRequest,reqLine);
    }
    if(netArg->flag_d==1){
        sendtoServer(netArg,netRequest,reqLine);
    }
    sendtoClientHead(netArg,netRequest,reqLine);
    if(netRequest->type==3){
        return returnvalue;
    }

    sendtoClient(netRequest, fd);
    return returnvalue;
}
