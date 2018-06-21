// **********************************************************************************************
//                           P r o c S e r M s g . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : ProcSerMsg.c
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "smTrkCli.h"

// *******************************
// Section : Definitions 
// *******************************
#define   CMD_MAX_LEN  32

// *******************************
// Section : Function Prototypes 
// *******************************
int32_t procSerDataMsg(int argc, char *argv[]);
int32_t sendNextScanReq(int serFd);

// **********************************
// Section : Static & Local Variables 
// **********************************
typedef struct cmdFun
{
  char cmd[CMD_MAX_LEN];
  int32_t (*cmdfun) (int, char **);
} cmdFun_t;

static cmdFun_t serCmdMap[] = 
{
  {"data",    procSerDataMsg},
};

// **********************************
// Section : Code
// **********************************
int32_t procSerDataMsg(int argc, char *argv[])
{
  char msg[300];
  char datastr[300],stow[300];
  char *tokens[50];
  int32_t ii, tkncnt;

  strcpy(datastr,argv[0]);
  for(ii=1; ii<argc; ii++)
    strcat(datastr,argv[ii]);

  sprintf(msg,"%s",datastr);
  printf("DataStr: %s\n",datastr);
  tkncnt=0;
  tokens[tkncnt] = strtok(datastr, ":\n");
  while(tokens[tkncnt])
    tokens[++tkncnt] = strtok(NULL, ":\n");
  
  if(strstr(tokens[0],"SMB"))
     InvfileQWrite(msg);

  if(smTrkCli.invFlag == 1)
     smTrkCli.stats[smTrkCli.curDevIx].invdataRespCnt++;
  else
     smTrkCli.cmdRespCnt++;

  sendNextScanReq(smTrkCli.serStrm.fdNo);
}


int32_t processSerMsg(char *msg)
{
  char *tokens[30];
  int32_t ii, tkncnt;
  
  tkncnt=0;
  tokens[tkncnt] = strtok(msg, " \n"); 

  if(!tokens[tkncnt])
    return EMPTY_CMD;

  while(tokens[tkncnt])
    tokens[++tkncnt] = strtok(NULL, " \n"); 

  for(ii=0; ii<(sizeof(serCmdMap)/sizeof(cmdFun_t)); ii++)
  {
    if(0 == strcmp(serCmdMap[ii].cmd, tokens[0]))
      break;
  }
    
  if( ii == sizeof(serCmdMap)/sizeof(cmdFun_t) )
  {
    procSerDataMsg(tkncnt, tokens);
    return INVALID_CMD;
  }
  return serCmdMap[ii].cmdfun(tkncnt, tokens);
}




