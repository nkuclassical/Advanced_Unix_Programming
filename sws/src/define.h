/*
  define.h
  finalxcode

  Created by Yanqiao Zhan on 11/28/15.
  Copyright © 2015 Yanqiao Zhan. All rights reserved.
*/

#ifndef define_h
#define define_h

#include <time.h>
/**
    struct for server side
 **/

typedef struct netarg{
    int flag_c;
    int flag_d;
    int flag_i;
    int flag_l;
    int flag_p;
    char* cgidir;
    char* ipaddress;
    char* logfile;
    char* port;
    char* rootdir;
    int logfd;
}NetArg;

/**
    struct used for head
 **/
typedef struct head{
    time_t date; /*the current timestamp in GMT*/
    char* server; /*a string identifying this server and version*/
    time_t lastModified; /*the timestamp in GMT of the file's last modification date*/
    char* contentType; /*text/html*/
    int contentLength; /*the size in bytes of the data returned*/
    int status_code;
}Head;

/**
    struct for client side
 **/
typedef  struct netrequest{
    int type;/* 1->get 2->post 3->head*/
    char* requestdir;
    char* protocal;
    char* ipaddress;
    int filetype;/* 1-> normail file; 2-> cgi file; 3->directoy; 4->no such file*/
    int clientfd;
    char* postcontent; /* post content field*/
    Head*head;
}NetRequest;



#endif /* define_h */
