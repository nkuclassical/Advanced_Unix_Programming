/*
   net.c
   finalxcode

   Created by Yanqiao Zhan on 11/25/15.
   Copyright © 2015 Yanqiao Zhan. All rights reserved.
   */

#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <syslog.h>

#include "net.h"



int server_listen(NetArg *netArg){
    int sockfd, err, connfd, readn;
    struct addrinfo hint, *ailist;
    char readbuf[1024];
    char clientRequest[4096];
    char *clientIP;
    struct sockaddr_in *sin;
    int max_listen=128;
    memset(&hint,0,sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_UNSPEC;
    hint.ai_flags =  AI_PASSIVE;


    signal(SIGCHLD,SIG_IGN);
    /*
TODO: do we need to add those information into log if the connection is failed? or the log file cannot be saved due to the para is not sufficient
*/
    if(netArg->ipaddress == NULL){
        if((err=getaddrinfo(NULL,netArg->port,&hint,&ailist))<0){
            if ( netArg->flag_l ==1 ) {
                updateLog(netArg,"[ERROR]Get AddrInfo error!\n");
            }
            if(netArg->flag_d==1){
                fprintf(stderr, "[ERROR]Get AddrInfo error!\n");
                /* sendtoServer(netArg,NULL,"[ERROR]GET AddrInfo error!\n",500); */
            }
            exit(EXIT_FAILURE);
        }
    }else{
        if((err=getaddrinfo(netArg->ipaddress,netArg->port,&hint,&ailist))<0){
            if ( netArg->flag_l ==1 ) {
                updateLog(netArg,"[ERROR]Get AddrInfo error!\n");
            }
            if(netArg->flag_d==1){
                fprintf(stderr, "[ERROR]Get AddrInfo error!\n");
                /* sendtoServer(netArg,NULL,"[ERROR]Get AddrInfo error!\n",500); */
            } 
            exit(EXIT_FAILURE);
        }
    }


    if((sockfd=socket(ailist->ai_family, ailist->ai_socktype, ailist-> ai_protocol))<0){
        if ( netArg->flag_l ==1 ) {
            updateLog(netArg,"[ERROR]Create socket error\n");
        }
        if(netArg->flag_d==1){
            fprintf(stderr, "[ERROR]Create socket error\n");
            /* sendtoServer(netArg,NULL,"[ERROR]Create socket error\n",500); */
        }
        exit(EXIT_FAILURE);
    }

    if(bind(sockfd,ailist->ai_addr,ailist->ai_addrlen)<0){
        if ( netArg->flag_l ==1 ) {
            updateLog(netArg,"[ERROR]Bind error\n");
        }
        if(netArg->flag_d==1){
            fprintf(stderr, "[ERROR]Bind error\n");
        }
        exit(EXIT_FAILURE);

    }

    if(netArg->flag_d==1){
        max_listen=1;
    }
    if(listen(sockfd,max_listen)<0){
        if ( netArg->flag_l ==1 ) {
            updateLog(netArg,"[ERROR]Listen error\n");
        }
        if(netArg->flag_d==1){
            fprintf(stderr, "[ERROR]Listen error\n");
        }
        exit(EXIT_FAILURE);
    }

    if ( netArg->flag_l ==1 ) {
        updateLog(netArg, "\n[INFO]Server is Listening...\n\n");
    }
    if(netArg->flag_d==1){
        fprintf(stderr, "\n[INFO]Server is Listening...\n");
    }

    while(1){
        pid_t pid;
        struct sockaddr* ipAddress;
        socklen_t AddressLen;
        ipAddress = ailist->ai_addr;
        AddressLen = ailist->ai_addrlen;

        if((connfd = accept(sockfd,ipAddress,&AddressLen))<0){
            if (netArg->flag_l == 1) {
                updateLog(netArg,"[INFO]Connection closed\n");
            }
            if(netArg->flag_d == 1 ){
                fprintf(stderr,"[INFO]Connection closed\n");
            }
            exit(0);
        }

        if(netArg->flag_d==1){
            char *message =  (char*)malloc(256*sizeof(char));
            sin = (struct sockaddr_in *) ipAddress;
            clientIP=malloc(sizeof(char)*16);
            clientIP = inet_ntoa(sin->sin_addr);
            strcpy(message, "[INFO]Client ");
            strcat(message, clientIP);
            strcat(message, " is connected.\n");
            if (netArg->flag_l ==1) {
                updateLog(netArg,message);
            }
            if(netArg->flag_d == 1 ){
                fprintf(stderr,"[INFO]Client %s is connected.\n",clientIP);
            }

            memset(&readbuf,0,1024);
            memset(&clientRequest,0,4096);
            while((readn=(int)read(connfd, readbuf, 1024)) >2){
                strcat(clientRequest, readbuf);
                memset(&readbuf,0,1024);
            }
            handle(clientRequest,netArg,clientIP,connfd);
            close(connfd);
        }else{
            if((pid=fork())==0){/*in child process*/
                char *message =  (char*)malloc(256*sizeof(char));
                close(sockfd);
                sin = (struct sockaddr_in *) ipAddress;
                clientIP=malloc(sizeof(char)*16);
                clientIP = inet_ntoa(sin->sin_addr);
                strcpy(message, "[INFO]Client ");
                strcat(message, clientIP);
                strcat(message, " is connected.\n");
                if (netArg->flag_l ==1) {
                    updateLog(netArg,message);
                }
                if(netArg->flag_d == 1 ){
                    fprintf(stderr,"[INFO]Client %s is connected.\n",clientIP);
                }

                memset(&readbuf,0,1024);
                while((readn=(int)read(connfd, readbuf, 1024)) >2){
                    strcat(clientRequest, readbuf);
                    memset(&readbuf,0,1024);
                }
                handle(clientRequest,netArg,clientIP,connfd);
            }

            close(connfd);
        }
    }
    return connfd;
}


/**
  Valid server ip address only support ipv4
  */

int ipaddressvalid(char*ip){
    int dotnum=0;
    int curpart=-1;
    int i;
    for(i=0;i<strlen(ip);i++){
        if(ip[i]=='.'){
            dotnum++;
            if(dotnum==4||curpart==-1||curpart>255)return -1;
            curpart=-1;
        }else{
            if(ip[i]-'0'<0||ip[i]-'0'>9)return -1;
            if(curpart==-1)curpart=ip[i]-'0';
            else curpart=curpart*10+ip[i]-'0';
        }
    }
    if(curpart==-1||curpart>255||dotnum!=3)return -1;
    return 1;
}


/**
  parser client request:
  GET dir protocal
  */

int parseRequest(NetArg *netArg, NetRequest *netRequest, char*readbuf){
    char*type;
    int offset=0;
    int start=0;
    int i;
    for(i=0;i<strlen(readbuf);i++){
        if(readbuf[i]==' '){
            offset=i;
            break;
        }
    }
    type=malloc(sizeof(char)*offset);
    memcpy(type,&readbuf[0],offset);
    type[offset]='\0';
    if(strcmp(type,"GET")==0){
        netRequest->type=1;
    }else if(strcmp(type,"POST")==0){
        netRequest->type=2;
    }else if(strcmp(type,"HEAD")==0){
        netRequest->type=3;
    }else{
        netRequest->head->status_code=400;
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, readbuf);
        }
        if(netArg->flag_d == 1 ){
            sendtoServer(netArg, netRequest, readbuf);
        }
        return -1;
    }

    start=offset+1;
    offset=0;
    for(i=start;i<strlen(readbuf);i++){
        if(readbuf[i]==' '){
            offset=i-start;
            break;
        }
    }
    if(offset==0){
        offset=(int)strlen(readbuf)-start;
    }

    if(offset<=0){
        netRequest->head->status_code=400;
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, readbuf);
        }
        if(netArg->flag_d == 1 ){
            sendtoServer(netArg, netRequest, readbuf);
        }
        return -1;
    }

    netRequest->requestdir=(char*)malloc(sizeof(char)*(offset+1));
    memcpy(netRequest->requestdir,&readbuf[start],offset);
    netRequest->requestdir[offset]='\0';


    start=start+offset+1;
    for(i=start;i<strlen(readbuf);i++){
        if(readbuf[i]=='\r'||readbuf[i]==' '||readbuf[i]=='\n'){
            offset=i-start;
            break;
        }
    }
    if(offset<=0){
        netRequest->head->status_code=400;
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, readbuf);
        }
        if(netArg->flag_d == 1 ){
            sendtoServer(netArg, netRequest, readbuf);
        }
        return -1;
    }
    netRequest->protocal=(char*)malloc(sizeof(char)*(offset+1));
    memcpy(netRequest->protocal,&readbuf[start],offset);  /* original readbuf tailed with \n\0, we don't need this two escape character*/
    netRequest->protocal[offset]='\0';

    if(strcasecmp(netRequest->protocal,"HTTP/1.0")!=0){
        netRequest->head->status_code=505;
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, readbuf);
        }
        if(netArg->flag_d == 1 ){
            sendtoServer(netArg, netRequest, readbuf);
        }
        return -1;
    }

    return 1;

}

/**
  If request path is under sws root path, then return 1;
  otherwise return 1;
 **/
int underrootdir(char *rootdir, char* requestdir){
    int i;
    for(i=0;i<strlen(rootdir);i++){
        if(requestdir[i]!=rootdir[i])return 0;
    }
    return 1;
}

/*
   check file type and set header
   */
void checkFileType(NetArg*netArg, NetRequest*netRequest,char *reqLine){
    char*requestdir=malloc(sizeof(char)*strlen(netRequest->requestdir));
    const char s[2]="/";
    char*subpath;
    struct stat info;
    char *username=malloc((int)sizeof(char)*256);
    char *newpath;
    strcpy(requestdir,netRequest->requestdir);
    subpath=strtok(requestdir, s);

    if(subpath==NULL){
        subpath=""; /*if subpath is NULL, then the following strcmp will crash unless we allocate memory to subpathj*/
    }
    if(strcmp(subpath, "cgi-bin")==0){
        netRequest->filetype=2; /* the path is a cgi file directory */
        netRequest->requestdir=adjunct(netArg->rootdir, netRequest->requestdir);
        return;
    }

    if(subpath[0]!='~'){
        netRequest->requestdir=adjunct(netArg->rootdir, netRequest->requestdir);
    }else{
        /*
           If client request is start with ~, then replace request path to /home/[username]/sws/...
           */

        username=memcpy(username,&subpath[1],strlen(subpath)-1);
        username=adjunct("/home/", username); /*username is like jschauma or yzhan14*/
        username=adjunct(username, "/sws");
        newpath=malloc(sizeof(char)*(strlen( netRequest->requestdir)+30));
        newpath=username;
        subpath=strtok(NULL,s);
        while(subpath!=NULL){
            newpath=adjunct(newpath, "/");
            newpath=adjunct(newpath,subpath);
            subpath=strtok(NULL, s);
        }
        netRequest->requestdir=newpath;

    }

    if(underrootdir(netArg->rootdir, netRequest->requestdir)==0){
        netRequest->head->status_code=403;
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, reqLine);
        }
        if(netArg->flag_d == 1 ){
            sendtoServer(netArg, netRequest, reqLine);
        }
        netRequest->filetype=4; /* illegal*/
        return;
    }

    if(stat(netRequest->requestdir,&info)!=0){
        netRequest->filetype=4; /*no such file or dir */
        netRequest->head->status_code=404;
        if(netArg->flag_l==1){
            sendtoLog(netArg, netRequest, reqLine);
        }
        if(netArg->flag_d==1){
            sendtoServer(netArg, netRequest, reqLine);
        }
        return;
    }

    netRequest->head->lastModified=info.st_mtime;
    if(S_ISDIR(info.st_mode)){
        newpath=adjunct(netRequest->requestdir,"/");
        newpath=adjunct(newpath, "index.html");
        if(stat(newpath,&info)==0){ /* directory contains index.html file*/
            netRequest->requestdir=newpath;
            netRequest->filetype=1;
        }
        else{
            netRequest->filetype=3; /* the path is a directory */
            return;
        }
    }
    if(netRequest->filetype==2){
        return;
    }

    netRequest->head->contentLength=(int)info.st_size; /*header content length*/
    netRequest->filetype=1; /* the path is a normal file*/
}

int handle(char*readbuf,NetArg* netArg,char*clientIP,int clientfd){
    NetRequest *netRequest=malloc(sizeof(NetRequest));
    netRequest->clientfd=clientfd;

    netRequest->head=malloc(sizeof(Head));
    time(&netRequest->head->date);/*set hedaer date*/
    netRequest->head->server=malloc(sizeof(char)*128);/*set server name*/
    gethostname(netRequest->head->server, sizeof(netRequest->head->server)); /*server identifier*/
    netRequest->head->contentType=malloc(sizeof(char)*32);
    strcpy(netRequest->head->contentType,"text/html");/* assign head content/type*/

    netRequest->head->status_code=200;/*default status code is 200, it will be replace to other code later*/
    if(clientIP==NULL){
        netRequest->head->status_code=501;
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, readbuf);
        }
        if(netArg->flag_d == 1 ){
            sendtoServer(netArg, netRequest, readbuf);
        }
        return -1;
    }else{
        netRequest->ipaddress=clientIP;
    }
    if(parseRequest(netArg,netRequest, readbuf)<0){
        sendtoClientHead(netArg,netRequest,readbuf);
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, readbuf);
        }
        if(netArg->flag_d == 1 ){
            sendtoServer(netArg, netRequest, readbuf);
        }
        return -1;
    }
    netRequest->requestdir=simplifyPath(netRequest->requestdir);

    checkFileType(netArg,netRequest,readbuf);

    if(netRequest->type==4){/*not get/post/head 501*/
        sendtoClientHead(netArg,netRequest,readbuf); 
    }

    if(netRequest->filetype==4){ /*no such file or dir*/

        netRequest->head->status_code=404;
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, readbuf);
        }
        if(netArg->flag_d == 1 ){
            sendtoServer(netArg, netRequest, readbuf);
        }

        sendtoClientHead(netArg,netRequest,readbuf);
        return -1;
    }

    fprintf(stderr,"\n");
    proc(netArg,netRequest,readbuf);
    return 1;

}


char* simplifyPath(char*path){
    const char s[2]="/";
    char *retPath;
    char*subpath;
    char*tpath;
    tpath=malloc(sizeof(char)*(strlen(path)+1));
    strcpy(tpath,path);
    subpath=strtok(tpath, s);
    while(subpath!=NULL){
        char *temppath=subpath;
        subpath=strtok(NULL, s);
        if(strcmp(temppath, "..")==0){
            if(isEmpty()){
                continue;
            }else{
                pop();
            }
        }else if(strcmp(temppath, ".")==0){
            continue;
        }else{
            push(temppath);
        }
    }
    retPath=(char*)malloc(sizeof(char)*strlen(path));
    retPath[0]='\0';
    while(!isEmpty()){
        size_t len1 = strlen(retPath), len2 = strlen(top());
        char *concat = malloc(len1 + len2 + 2);
        strcpy(concat,"/");
        strcat(concat,pop());
        strcat(concat,retPath);
        concat[len1+len2+1]='\0';
        retPath=concat;
    }
    if(retPath[0]=='\0'){
        retPath="/";
    }
    return retPath;
}

char *adjunct(char *rootpath, char *filepath){
    char *result;
    int len=(int)(strlen(rootpath)+strlen(filepath));
    result = (char *)malloc((len+1) *sizeof(char));
    strcpy(result,rootpath);
    strcat(result,filepath);
    result[len]='\0';
    return result;
}
