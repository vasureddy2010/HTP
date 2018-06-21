// **********************************************************************************************
//                           P r o c S o c k M s g . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : ProcSockMsg.c
// Description:
// Date       : 04-11-2015 
// Author     : FirmWare Team
// Compilers  : Gcc / ARM 
// **********************************************************************************************

// *******************************
// Section : Includes 
// *******************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "smTrkCli.h"

// *******************************
// Section : Definitions 
// *******************************
#define   CMD_MAX_LEN  32

// *******************************
// Section : Function Prototypes 
// *******************************
int32_t procSockCmdMsg(int argc, char *argv[],int32_t sockfd);
int32_t procSockRespMsg(int argc, char *argv[],int32_t sockfd);
void postInvData();
void ackPendingInvData();
void *piDateTimeThreadFun(void *arg);
void PollingStatus(char *filename);
int ReceiveFile(char *filename);
int SendFile(char *filename,int stat);

// **********************************
// Section : Static & Local Variables 
// **********************************
typedef struct cmdFun
{
  char cmd[CMD_MAX_LEN];
  int32_t (*cmdfun) (int, char **,int32_t);
} cmdFun_t;

static cmdFun_t sockCmdMap[] = 
{
  {"rmtCmd",    procSockCmdMsg},
  {"resp",      procSockRespMsg},
};

// *******************************
// Section : Code 
// *******************************
int32_t procSockCmdMsg(int argc, char *argv[],int32_t sockfd)
{
  int32_t fd,trkrId,pid,sd,rc,ret,nid,tmpi,rsplen,ii;
  char msg[1024],info[100000],command[126];
  char message[40],num[4],rcmds[40];
  FILE *fp;

  if(argc < 2)
    return 1;
  
  printf("\nCOMMAND() : ");

  if(strcmp(argv[1],"put") == 0)
  {
    // Need To get Required File(argv[1] is Server File,argv[2] is Client File)
    strcpy(smTrkCli.srvfile, argv[2]);
    strcpy(smTrkCli.clifile, argv[3]);
    ret = ReceiveFile(smTrkCli.clifile);
    if(ret == 1)
      printf("\nReceiving File '%s' Fail\n",argv[3]);
    else 
      printf("\nReceiving File '%s' Success\n",argv[3]);
    return;
  }
  else if(strcmp(argv[1],"get") == 0)
  {
    // Need To Send Required File(argv[1] is Server File,argv[2] is Client File)
    strcpy(smTrkCli.srvfile, argv[2]);
    strcpy(smTrkCli.clifile, argv[3]);
    ret = SendFile(smTrkCli.clifile,2);
    if(ret == 1)
      printf("\nSending File '%s' Fail\n",argv[3]);
    else 
      printf("\nSending File '%s' Success\n",argv[3]);
    return;
  }
  else if(strcmp(argv[1],"poll") == 0)
  {
    // Need To Send Polling(Scanning) Status 
    strcpy(smTrkCli.srvfile, "none");
    strcpy(smTrkCli.clifile, "poll");
    PollingStatus("command.txt");
    ret = SendFile("command.txt",1);
    if(ret == 1)
      printf("\nSending File 'command.txt' Fail\n");
    else 
      printf("\nSending File 'command.txt' Success\n");
    return;
  }
  else if(strcmp(argv[1],"command") == 0)
  {
    // Need To Handle Shell Commands
    strcpy(command,argv[2]);
    if(strcmp(argv[2],"kill") == 0)
    {
      sprintf(command,"kill -9 %d",getpid()); 
      system(command);
      return;
    }
    for(ii=3;ii<argc;ii++)
    {
      strcat(command," ");
      strcat(command,argv[ii]);
    }
    strcpy(smTrkCli.clifile,command);
    strcpy(smTrkCli.srvfile,"none");
    strcat(command,"  > command.txt");
    system(command);
    printf("'%s' is executed\n",command);
    ret = SendFile("command.txt",1);
    if(ret == 1)
      printf("\nSending File '%s' Fail\n",command);
    else 
      printf("\nSending File '%s' Success\n",command);
    return;
  }
  else
  { 
   // need to handle Hexa based packed from socket
    smTrkCli.rmtCmds.flg = 1;
    strcpy(smTrkCli.rmtCmds.msg,argv[1]);
  }
}

int32_t procSockRespMsg(int argc, char *argv[],int32_t sockfd)
{
  int32_t     ii, statCode;
  char        msg[128];
  pthread_t   piTh;

  if(argc < 3)
  {
    printf("Invalid server response message\n");
    return 1;
  }
  printf("SERVER(): ");
  sscanf(argv[2],"%d",&statCode); 
  if(strcmp(argv[1],"site") == 0)
  {
    // Site Command Response either Success or Fail from All Servers
    if(smTrkCli.invsockFd == sockfd) 
    {
       if(statCode==SUCCESS)
       {
         printf("Site command response success for SMB\n");
         sprintf(msg,"date req\n");
         write(smTrkCli.invsockFd,msg,strlen(msg));
         smTrkCli.invsockTransState = SOCK_TRANS_IDLE;
       }
       else
         printf("Site command response fail for SMB\n");         
    }
  }
  else if(strcmp(argv[1],"data") == 0)
  {
    // Data Command Response either Success or Fail from All Servers
    if(smTrkCli.invsockFd == sockfd) 
    {
       if(statCode == 1)
            return 0;
       else if(statCode == 2)
            return 0;

          printf("Data command response success for SMB \n");
          smTrkCli.invsockTransState = SOCK_TRANS_IDLE;
          ackPendingInvData();
          postInvData();
    }   
  }
  else if(strcmp(argv[1],"date") == 0)
  {
    //Create Thread Here for Date Command process
    printf("Date Command Response Success\n");
    smTrkCli.invsockTransState = SOCK_TRANS_IDLE;
    sprintf(smTrkCli.serverdatetime,"%s %s",argv[2],argv[3]);
    pthread_create(&piTh, NULL, piDateTimeThreadFun, NULL);
  }
  else
  {
    printf("Response for unknown %s command\n",argv[1]);
  }
}

int32_t processSockMsg(char *msg,int32_t sockfd)
{
  char *tokens[10];
  int32_t ii, tkncnt;
  
  tkncnt=0;
  tokens[tkncnt] = strtok(msg, " \n"); 

  if(!tokens[tkncnt])
    return EMPTY_CMD;

  while(tokens[tkncnt])
    tokens[++tkncnt] = strtok(NULL, " \n"); 

  for(ii=0; ii<(sizeof(sockCmdMap)/sizeof(cmdFun_t)); ii++)
  {
    if(0 == strcmp(sockCmdMap[ii].cmd, tokens[0]))
      break;
  }
   
  if( ii == sizeof(sockCmdMap)/sizeof(cmdFun_t) )
    return INVALID_CMD;
  return sockCmdMap[ii].cmdfun(tkncnt, tokens,sockfd);
}



