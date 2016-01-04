
/*
 util.c
 finalxcode
 
 Created by Yanqiao Zhan on 11/29/15.
 Copyright © 2015 Yanqiao Zhan. All rights reserved.
 */

#include "util.h"



int getStatusDetail(int stacode, char* Reason_Phrase){



	if(stacode == 200){		
		strcpy(Reason_Phrase,"200 OK");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';	
	}
	else if(stacode == 201){
		strcpy(Reason_Phrase,"201 Created");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';	
	}
	else if(stacode == 202){
		strcpy(Reason_Phrase,"202 Accepted");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';	
	}
	else if(stacode == 204){
		strcpy(Reason_Phrase,"204 No Content");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';		
	}
	else if(stacode == 301){
		strcpy(Reason_Phrase,"301 Moved Permanently");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';		
	}
	else if(stacode == 302){
		strcpy(Reason_Phrase,"302 Moved Temporarily");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';	
	}
	else if(stacode == 304){
		strcpy(Reason_Phrase,"304 Not Modified");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';	
	}
	else if(stacode == 400){
		strcpy(Reason_Phrase,"400 Bad Request");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';		
	}
	else if(stacode == 401){
		strcpy(Reason_Phrase,"401 Unauthorized");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';		
	}
	else if(stacode == 403){
		strcpy(Reason_Phrase,"403 Forbidden");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';		
	}
	else if(stacode == 404){
		strcpy(Reason_Phrase,"404 Not Found");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';	
	}else if(stacode == 405){
		strcpy(Reason_Phrase,"405 Method Not Allowed");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';	
	}else if(stacode == 406){
		strcpy(Reason_Phrase,"406 Not Acceptable");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';	
    }
	else if(stacode == 500){
		strcpy(Reason_Phrase,"500 Internal Server Error");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';
	}
	else if(stacode == 501){
		strcpy(Reason_Phrase,"501 Not Implemented");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';		
	}
	else if(stacode == 502){
		strcpy(Reason_Phrase,"502 Bad Gateway");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';	
	}
	else if(stacode == 503){
		strcpy(Reason_Phrase,"503 Service Unavailable");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';
	}
	else if(stacode == 505){
		strcpy(Reason_Phrase,"505 HTTP Version Not Supported");
		Reason_Phrase[strlen(Reason_Phrase)]='\0';
	}
	else{
		
		return -1;
	}
	return 0;
	
}



/*
 The second parameter is the file descriptor of the file that you want to send to client
 
 */
int sendtoClient(NetRequest*netRequest,int fd){
    char temp[1024];
    ssize_t fd_t;
    while((fd_t = read(fd,temp,1024)) > 0){
        if(write(netRequest->clientfd,temp,fd_t) != fd_t){
            fprintf(stderr,"write to client fail\n");
            return -1;
        }
    }
    write(netRequest->clientfd,"\n",1);
    
    return 1;
}

/*
 The second parameter is a char array that you want to send to client
 */
int sendtoClientContent(NetRequest*netRequest,char*content){
    if(write(netRequest->clientfd,content,strlen(content))!=strlen(content)){
        return -1;
    }
    write(netRequest->clientfd,"\n",1);
    return 1;
}
char* myasctime(struct tm*time){
    char*temp=asctime(time);
    char*result=malloc(sizeof(char)*strlen(temp));
    memcpy(result, temp, strlen(temp)-1);
    result[strlen(temp)-1]='\0';
    return result;
}

int sendtoLog(NetArg*netArg,NetRequest*netReq,char*reqLine){
    
    char *reqBuff=malloc(sizeof(char)*(strlen(reqLine)+1));
    time_t req_t;
    char code[32];
    char *message;
    char *conLength;
    int j = 0;
    int i = 0;
    for (i = 0; i < strlen(reqLine); i++) {
        if (reqLine[i] != '\n'&&reqLine[i]!='\r') {
            reqBuff[j] = reqLine[i];
            j++;
        }
    }

    reqBuff[j] = '\n';
    reqBuff[j]='\0';
    message = (char*)malloc(256*sizeof(char));
    conLength = (char*)malloc(256*sizeof(char));
    strcpy(message, netReq->ipaddress);
    strcat(message, " ");
    req_t= time(NULL);
    strcat(message, ctime(&req_t));
    strcat(message, " \"");
    strcat(message, reqBuff);
    strcat(message, "\" ");
    sprintf(code, "%d", netReq->head->status_code);
    strcat(message, code);
    strcat(message, " ");
    strcat(message, "Content-length: ");
    sprintf(conLength,"%d", netReq->head->contentLength);
    strcat(message, conLength);
    strcat(message, "\n\n");
    message[strlen(message)]='\0';
    if(write(netArg->logfd,message,strlen(message))!=strlen(message)){
        if ( netArg->flag_l ==1 ) {
            netReq->head->status_code=500;
        }else {
            fprintf(stderr, "[ERROR]Write to client fail\n");
        }
        return -1;
    }
   
    return 1;
}
/* int sendtoServer(NetArg*netArg,NetRequest*netReq,char*content){ */
int sendtoServer(NetArg*netArg,NetRequest*netReq,char*reqLine){
    
    char *reqBuff=malloc(sizeof(char)*(strlen(reqLine)+1));
    time_t req_t;
    char code[32];
    char *message;
    char *conLength;
    int j = 0;
    int i = 0;
    for (i = 0; i < strlen(reqLine); i++) {
        if (reqLine[i] != '\n') {
            reqBuff[j] = reqLine[i];
            j++;
        }
    }
    reqBuff[j] = '\n';
    message = (char*)malloc(256*sizeof(char));
    conLength = (char*)malloc(256*sizeof(char));
    strcpy(message, netReq->ipaddress);
    strcat(message, " ");
    req_t= time(NULL);
    strcat(message, ctime(&req_t));
    strcat(message, " \"");
    strcat(message, reqBuff);
    strcat(message, "\" ");
    sprintf(code, "%d", netReq->head->status_code);
    strcat(message, code);
    strcat(message, " ");
    strcat(message, "Content-length: ");
    /*
     TODO: do we need to check if Content-length is null or not?
     */
    sprintf(conLength,"%d", netReq->head->contentLength);
    strcat(message, conLength);
    strcat(message, "\n");
    message[strlen(message)]='\0';

    fprintf(stderr,"%s",message);
    return 1;
}

/* 
RFC:
Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
Date           = "Date" ":" HTTP-date
Server         = "Server" ":" 1*( product | comment )
Last-Modified  = "Last-Modified" ":" HTTP-date
Content-Type   = "Content-Type" ":" media-type
Content-Length = "Content-Length" ":" 1*DIGIT

example:
HTTP/1.0 200 OK 
Date: Tue, 15 Nov 1994 08:12:31 GMT
Server: CERN/3.0 libwww/2.17
Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT
Content-Type: text/html
Content-Length: 3495
*/

int sendtoClientHead(NetArg*netArg,NetRequest*netRequest,char*readLine){
    Head*head=netRequest->head;
    
    char number[100];
    char*temp=malloc(sizeof(char)*1024);
    
    /*Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF*/
   char *reason = malloc(sizeof(char)*40);
   strcpy(temp,"HTTP/1.0 ");
		
   if(getStatusDetail(netRequest->head->status_code,reason) == 0){	
		strcat(temp,reason);
   }
   else{
	   fprintf(stderr,"No such status code exit!");
   }
	
    	
    if(sendtoClientContent(netRequest, temp)==-1){
       
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, readLine);
            
        }else {
            fprintf(stderr, "[ERROR]Send to client error!\n");
        }
        return -1;
    }

    strcpy(temp, "Date: ");
    strcat(temp, myasctime(gmtime(&head->date)));
    if(sendtoClientContent(netRequest, temp)==-1){
       
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netRequest, readLine);
            
        }else {
            fprintf(stderr, "[ERROR]Send to client error!\n");
        }
        return -1;
    }
    
    if(head->server!=NULL){
        strcpy(temp,"Server: ");
        strcat(temp,head->server);
        if(sendtoClientContent(netRequest, temp)==-1){
            if ( netArg->flag_l ==1 ) {
                sendtoLog(netArg, netRequest, readLine);
            }else {
                fprintf(stderr, "[ERROR]Send to client error!\n");
            }
            return -1;
        }
    }
    
    strcpy(temp,"Last-Modified: ");
    strcat(temp,myasctime(gmtime(&head->lastModified)));
    if(sendtoClientContent(netRequest, temp)==-1){
        if ( netArg->flag_l ==1 ) {
                netRequest->head->status_code=888;
            sendtoLog(netArg, netRequest, readLine);
        }else {
            fprintf(stderr, "[ERROR]Send to client error!\n");
        }
        return -1;
    }
    
    
    if(head->contentType!=NULL){
        strcpy(temp,"Content-Type: ");
        strcat(temp,head->contentType);
        if(sendtoClientContent(netRequest, temp)==-1){
            if ( netArg->flag_l ==1 ) {
                netRequest->head->status_code=888;
                sendtoLog(netArg, netRequest, readLine);
            }else {
                fprintf(stderr, "[ERROR]Send to client error!\n");
            }
            return -1;
        }
        
    }
    
    strcpy(temp,"Content-Length: ");
    sprintf(number, "%d", head->contentLength);
    strcat(temp,number);
    if(sendtoClientContent(netRequest, temp)==-1){
        
        if ( netArg->flag_l ==1 ) {
            netRequest->head->status_code=8888;
            sendtoLog(netArg, netRequest, readLine);
        }else {
            fprintf(stderr, "[ERROR]Send to client error!\n");
        }
        return -1;
        
    }
    
    return 1;
}
/*
 Update log information, like "[INFO]Connection closed\n", this function is different from sendtolog, this doesn't need ip or time information
 */
int updateLog(NetArg*netArg,char*message){
    
    if(write(netArg->logfd,message,strlen(message))!=strlen(message)){
        /*
        if ( netArg->flag_l ==1 ) {
            sendtoLog(netArg, netReq, reqLine, 888);
        }else {
            fprintf(stderr, "[ERROR]Write to client fail\n");
        }
         */
        return -1;
    }
    
    return 1;

}
