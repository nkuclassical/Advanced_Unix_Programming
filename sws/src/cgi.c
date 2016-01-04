/*
   cgi.c
   finalxcode

   Created by Yanqiao Zhan on 11/25/15.
   Copyright © 2015 Yanqiao Zhan. All rights reserved.
   */

#include "cgi.h"
/**
  cgi request path may look like cgi-bin/env.cgi?name=xyz&age=10
  so we need to store parameter into environmnet variable stringquery

  return value: -2: no such cgi file
  -1: execute error
 **/
int under(char*root, char*cgi){
    int curroot=0, curcgi=0;
    while(curroot<strlen(root)&&curcgi<strlen(cgi)){
        if(root[curroot]=='/'){
            curroot++;
            continue;}
        if(cgi[curcgi]=='/'){
            curcgi++;
            continue;
        }
        if(root[curroot]==cgi[curcgi]){
            curroot++;
            curcgi++;
        }else return -1;
    }
    if(curroot<strlen(root))return -1;
    return 1;
}
int cgiexe(NetRequest*netRequest,NetArg*netArg,char*reqLine){
    int childpid;
    char*emptylist[2];
    char*tempstr;
    char*cgipath=malloc(sizeof(char)*strlen(netRequest->requestdir));
    char*parameter=malloc(sizeof(char)*strlen(netRequest->requestdir));
    int i=0,j=0,findmark=0;
    struct stat info;   
    int returnvalue=0;

    signal(SIGCHLD,SIG_IGN);
    for(i=0;i<strlen(netRequest->requestdir);i++){
        if(netRequest->requestdir[i]=='?'){
            findmark=1;
            break;
        }
        cgipath[i]=netRequest->requestdir[i];
    }
    cgipath[i]='\0';
    if(netArg->flag_c==1&&under(netArg->cgidir,cgipath)==-1){
       netRequest->head->status_code=403;
       returnvalue=-2;
    }
    if(findmark==1){
        for(j=i+1;j<strlen(netRequest->requestdir);j++){
            parameter[j-i-1]=netRequest->requestdir[j];
        }
        parameter[j-i-1]='\0';
    }else{
        parameter[0]='\0';
    }

    if(stat(cgipath,&info)!=0){
        netRequest->head->status_code=403;
        returnvalue = -2;
    }

    if(netArg->flag_l==1){
        sendtoLog(netArg,netRequest,reqLine);
    }

    if(netArg->flag_d==1){
        sendtoServer(netArg,netRequest,reqLine);
    }
    sendtoClientHead(netArg,netRequest,reqLine);
    if(netRequest->type==3){
        return 0;
    }
    if(returnvalue>=0){
        if((childpid=fork())==0){
            tempstr=malloc(sizeof(char)*1024);
            sprintf(tempstr, "QUERY_STRING=%s",parameter);
            emptylist[0]=tempstr;
            emptylist[1]=NULL;
            dup2(netRequest->clientfd,STDOUT_FILENO);

            if(execle(cgipath,"",NULL,emptylist)<0){
                returnvalue=-1;
            }else{
                returnvalue=1;
            }
        }
    }
    return returnvalue;
}
