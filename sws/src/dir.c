/*
   dir.c
   finalxcode

   Created by Yanqiao Zhan on 11/28/15.
   Copyright © 2015 Yanqiao Zhan. All rights reserved.
   */

#include "dir.h"
int compare(const FTSENT**,const FTSENT**);
int direxe(NetRequest*netRequest,NetArg*serverArg,char*reqLine){
    FTS*ftsp=NULL;
    FTSENT*p=NULL;
    char*dir[2];
    dir[0]=netRequest->requestdir;
    dir[1]=NULL;
    if(access(netRequest->requestdir,R_OK)!=0){
        if (serverArg->flag_l == 1) {
            netRequest->head->status_code=403;
            sendtoLog(serverArg, netRequest, reqLine);
        }

        netRequest->head->status_code=403;
        sendtoServer(serverArg, netRequest, reqLine);
        sendtoClientHead(serverArg,netRequest,reqLine); 
        return -1;
    }else {

        if (serverArg->flag_d == 1) {
            netRequest->head->status_code=200;
            sendtoServer(serverArg, netRequest, reqLine);
        }
        if (serverArg->flag_l == 1) {
            netRequest->head->status_code=200;
            sendtoLog(serverArg, netRequest, reqLine);
        }
    }

    sendtoClientHead(serverArg,netRequest,reqLine);
    if(netRequest->type==3){
        return 0;
    }
    ftsp= fts_open(dir,FTS_PHYSICAL,&compare);
    if(ftsp!=NULL){
        if(fts_read(ftsp)!=NULL){
            p=fts_children(ftsp,0);
            while(p!=NULL&&p->fts_link!=NULL){
                p=p->fts_link;
                if (sendtoClientContent(netRequest, p->fts_name) < 0) {
                    return -1;
                }
            }
        }
        fts_close(ftsp);
    }

    return 1;
}

int compare(const FTSENT** one, const FTSENT** two){
    return (strcasecmp((*one)->fts_name, (*two)->fts_name));
}
