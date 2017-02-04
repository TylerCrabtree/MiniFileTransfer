

//#define _BSD_SOURCE


#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

// Macro section:
#define BUFFERSIZE 100
#define TEXT 450
#define TRUE 1
#define FALSE -1
#define replyLength 2
#define NORMAL 0


#ifndef MFTP_H
#define MFTP_H

// both files (takes different params) 
//void get(int datafd,int connectfd, char* path);
/*
// client
void helperFunction();
char* grabMessage(int socketfd);
int setup(char* serverID, int port);
void EXIT();
void ls();
void rls(int fd);
void cd(char* path);

//server
char* grabTime();							// Used function in daytime.c 
char* readBuffer(char word, char* buffer , int connectfd );
int sense(int port);
struct sockaddr_in createServer(int port);
int rcd(char* path);
void rls(int datafd);
int dataConnection(int listenfd);
*/
#endif
