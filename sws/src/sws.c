/*
   main.c
   finalxcode

   Created by Yanqiao Zhan on 11/25/15.
   Copyright © 2015 Yanqiao Zhan. All rights reserved.
   */

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "net.h"
#include "define.h"

#define fail EXIT_FAILURE
#define succ EXIT_SUCCESS

void usage(void){
    fprintf(stderr, "usage: sws [-dh][-c dir][-i address][-l file][-p port] dir\n");
    exit(fail);
}

void usage_detail(void){
    fprintf(stderr,"sws usage:\n -c dir Allow execution of CGIs from the given directory.\n-d Enter debugging mode.\n-h Print a short usage summary and exit.\n-i address Bind to the given IPv4 and IPv6 address. If not provided,sws will listen on all IPv4 and IPv6 addresses on this host.\n-l file Log all requests to the given file.\n-p port Listen on the given port. If not provided, sws will listen on port 8080.\n");
}


int main(int argc, char**argv){
    NetArg *netArg=malloc(sizeof(NetArg));
    char ch;
    struct stat st;
    if(argc==1)usage();
    netArg->cgidir = NULL;
    netArg->ipaddress = NULL;
    netArg->logfile = NULL;
    netArg->port = "8080";
    netArg->rootdir=NULL;
    netArg->flag_c=0;
    netArg->flag_d=0;
    netArg->flag_i=0;
    netArg->flag_l=0;
    netArg->flag_p=0;



    while((ch=getopt(argc,argv,"c:dhi:l:p:"))!=-1){
        switch(ch){
            case 'c': /*cgi*/
                netArg->cgidir=optarg;
                netArg->flag_c=1;
                fprintf(stderr,"[INFO]CGI dir: %s\n", netArg->cgidir);
                break;
            case 'd': /*debug model*/
                netArg->flag_d=1;
                fprintf(stderr,"[INFO]Debug model open\n");
                break;
            case 'h': /*print out usage*/
                usage_detail();
                exit(EXIT_SUCCESS);
                break;
            case 'i': /*bind to given ipv4/ipv6 address, by default all ipv4 and ipv6 addresses on this host.*/
                netArg->ipaddress=optarg;
                netArg->flag_i=1;
                if(ipaddressvalid(optarg)==-1){
                    fprintf(stderr, "[ERROR]Please enter a valid ip address!\n");
                    exit(EXIT_FAILURE);
                }
                fprintf(stderr,"[INFO]Try to listen on address %s\n", netArg->ipaddress);
                break;
            case 'l':{ /*log all requests to the given file*/
                netArg->logfile=optarg;
                netArg->flag_l=1;
                fprintf(stderr,"[INFO]Log file path: %s\n", netArg->logfile);
                break;
            }case 'p':{ /*listen on the given port, default port 8080*/
                int tempPort=atoi(optarg);
                if(tempPort>=0&&tempPort<=65535){
                    netArg->port=optarg;
                    fprintf(stderr,"[INFO]Try to listen on port %s\n", netArg->port);
                }else{
                    fprintf(stderr, "[ERROR]Port number should between 0~65535\n");
                    exit(fail);
                }
                break;
            }
            default:
                     usage();
        }
    }
    argc -= optind;
    argv += optind;



    if(argv[0]==NULL){
        fprintf(stderr, "[ERROR]Missing sws root dir!\n");
        usage();
        exit(fail);
    }
    netArg->rootdir=malloc(sizeof(char)*(int)strlen(argv[0]));
    strcpy(netArg->rootdir, argv[0]);
    if(netArg->rootdir==NULL){
        fprintf(stderr, "[ERROR]Missing root dir\n");
        exit(fail);
    }else {
        netArg->rootdir=simplifyPath(netArg->rootdir);
        if(stat(netArg->rootdir,&st)!=0){
            fprintf(stderr, "[ERROR]Root directory illegal!\n");
            exit(fail);
        }
    }
    if(netArg->flag_l!=0){

        if((netArg->logfd=open(netArg->logfile,O_RDWR|O_APPEND|O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))==-1){
            fprintf(stderr, "[ERROR]Can't open log file %s. Please retry!\n",netArg->logfile);
            exit(fail);
        }else{
            fprintf(stderr,"[INFO]Start logging to %s\n",netArg->logfile);
        }
    }
    if(netArg->flag_d == 0){
        if(  daemon(1,0) <0 ){
            fprintf(stderr, "[ERROR] Daemon fails!\n");
        }   
    }
    server_listen(netArg);
    exit(EXIT_SUCCESS);
}
