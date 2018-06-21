// **********************************************************************************************
//                           u s r i n t f c . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : usrintfc.c
// Description:
// Date       : 04-11-2015 
// Author     : FirmWare Team
// Compilers  : Gcc / ARM 
// **********************************************************************************************

// *******************************
// Section : Includes 
// *******************************
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "smTrkCli.h"

// *******************************
// Section : Definitions 
// *******************************
#define   CMD_MAX_LEN       32 


// *******************************
// Section : Function Prototypes 
// *******************************
int  printHelp(int argc, char *argv[]);
int  printStats(int argc, char *argv[]);
int  printFQfun(int argc, char *argv[]);
int  printCfgFun(int argc, char *argv[]);
int  printSockState(int argc, char *argv[]);
int  quitCmd(int argc, char *argv[]);
int  versionCmd(int argc, char *argv[]);
void printConfig(FILE *fp);
void printInvFQstats(FILE *fp);


// *******************************
// Section : File Local Variables 
// *******************************
typedef struct cmdFun
{
  char cmd[CMD_MAX_LEN];
  int (*fun) (int, char **);
  char *helpstr;
} cmdFun_t;

cmdFun_t commandMap[] = 
{
  {"ps",   printStats,    "Prints client and trakers Statistics"},
  {"psk",  printSockState,"Prints socket state"},
  {"pfq",  printFQfun,    "Prints File Queue Statistics"},
  {"pc",   printCfgFun,   "Prints Config parameters"},
  {"v",    versionCmd,    "Prints version information"},
  {"q",    quitCmd,       "Close and quit the connection"},
  {"h",    printHelp,     "Prints this list"}
};

FILE *sockFp;
int  quitFlg;



// *******************************
// Section : Global Variables 
// *******************************
extern char *verDateStr;
extern char *verTimeStr;



// *******************************
// Section : Code 
// *******************************
int versionCmd(int argc, char *argv[])
{
  fprintf(sockFp,"\nSmartTrak Monitor Software V%d.%02d Dated %s %s\n",
         MAJOR_VER,MINOR_VER,verDateStr, verTimeStr);
}


int printFQfun(int argc, char *argv[])
{
  printInvFQstats(sockFp);
}

int printCfgFun(int argc, char *argv[])
{
  printConfig(sockFp);
}

int printSockState(int argc, char *argv[])
{
  fprintf(sockFp, "\n**************** TRACKER INFO ****************\n");
  if(smTrkCli.invsockState == SOCK_CONNECTED)
    fprintf(sockFp, "Socket in CONNECTED state\n");
  else if(smTrkCli.invsockState == SOCK_NOT_CONNECTED)
    fprintf(sockFp, "Socket in NOT_CONNECTED state\n");
  else
    fprintf(sockFp, "Socket in UNKNOWN state\n");

  if(smTrkCli.invsockTransState == SOCK_TRANS_IDLE)
    fprintf(sockFp, "Socket Transaction IDLE\n");
  else if(smTrkCli.invsockTransState == SOCK_TRANS_RESP_WAIT)
    fprintf(sockFp, "Socket Transaction WAIT_RESP\n");
  else
    fprintf(sockFp, "Socket Transaction UNKNOWN\n");
 
  fprintf(sockFp, "Connection Success : %u\n",smTrkCli.invsockConSuccessCnt);
  fprintf(sockFp, "Connection Fails   : %u\n",smTrkCli.invsockConFailCnt);
  fprintf(sockFp, "Connection Error   : %s\n",strerror(smTrkCli.invsockConErrno));
  fprintf(sockFp, "Close on ReaddFail : %u\n",smTrkCli.invsockRdFailCnt);
  fprintf(sockFp, "Close on Tmout     : %u\n",smTrkCli.invsockRespTmoutCnt);
}

int printStats(int argc, char *argv[])
{
  int ii;

  fprintf(sockFp, "No. of Times Tracker Polling   : %u\nNo. of Tracker Select Count    : %u\nNo. of Times while loop repeat : %u\n",smTrkCli.invpolling,stats.invserSelCnt,stats.invserHeartBeats);

  fprintf(sockFp, "Count of Requested Commands    : %u\nCount of Response for Commands : %u\n",           smTrkCli.cmdReqCnt, smTrkCli.cmdRespCnt);


  fprintf(sockFp, "\n**************** TRACKER INFO ****************\n");
  fprintf(sockFp, "Req Count   Resp Count  Tmout Cnt  \n");
  fprintf(sockFp, "----------  ----------  ---------- \n");
  for(ii=smTrkCli.invStart-1; ii<(smTrkCli.invCnt+smTrkCli.invStart-1); ii++)
  {
    fprintf(sockFp, "%10u  %10u  %10u\n",
          smTrkCli.stats[ii].invdataReqCnt, smTrkCli.stats[ii].invdataRespCnt,
          smTrkCli.stats[ii].invdataTmoutCnt);
  }

}

int quitCmd(int argc, char *argv[])
{
  quitFlg = 1;
}

int printHelp(int argc, char *argv[])
{
  int ii;
 
  fprintf(sockFp,"\nFollowing are the valid commands\n\n"); 
  for(ii=0; ii<(sizeof(commandMap)/sizeof(cmdFun_t)); ii++)
    fprintf(sockFp,"%s\t%s\n", commandMap[ii].cmd, commandMap[ii].helpstr);
 
  fprintf(sockFp,"\n"); 
  return 0;
}

void *docli(void *arg)
{
  char               *tokens[10];
  int                ii, tkncnt;
  unsigned char      cmd[CMD_MAX_LEN];
  int32_t            listenFd,conSockFd;
  struct sockaddr_in srvadr,cliadr;
  int32_t            stat;

  if((listenFd=socket(AF_INET,SOCK_STREAM,0))<0)
  {
    perror("can't open stream socket\n");
    exit(1);
  }

  memset(&srvadr,0,sizeof(srvadr));
  srvadr.sin_family      = AF_INET;
  srvadr.sin_addr.s_addr = htonl(INADDR_ANY);
  srvadr.sin_port        = htons(smTrkCli.localportnum);

  if(bind(listenFd,(struct sockaddr *)&srvadr,sizeof(srvadr))<0)
  {
    perror("can't bind local address\n");
    exit(2);
  }

  listen(listenFd,10);

  while(1)
  {
    conSockFd = accept(listenFd, (struct sockaddr *)0, NULL);
    if(conSockFd >= 0)
      sockFp = fdopen(conSockFd, "r+");
     
    if((conSockFd >= 0) && sockFp)
    {
      fprintf(sockFp, "\n\nSmartTrak Remote Monitor V1.01\n");
      quitFlg = 0;
      while(1)
      {
        fprintf(sockFp,"cli> ");

        if(!fgets(cmd, CMD_MAX_LEN, sockFp))
          break;
 
        tkncnt=0;
        tokens[tkncnt] = strtok(cmd, " \r\n"); 

        if(!tokens[tkncnt])
          continue;

        while(tokens[tkncnt])
          tokens[++tkncnt] = strtok(NULL, " \r\n"); 

        for(ii=0; ii<(sizeof(commandMap)/sizeof(cmdFun_t)); ii++)
        {
          if(0 == strcmp(commandMap[ii].cmd, tokens[0]))
            break;
        }
    
        if( ii == sizeof(commandMap)/sizeof(cmdFun_t) )
        {
          fprintf(sockFp,"Invalid command. Enter 'h' for valid commands\n");
        }
        else
          commandMap[ii].fun(tkncnt, tokens);
        if(quitFlg)
          break;
      }
      fclose(sockFp);
    }
  }
}

void PollingStatus(char *filename)
{
  FILE *fp;
  int ii;
  char file[1024];

  fp = fopen(filename, "w");
  if(!fp)
  {
    perror("opening text file");
    return;
  }

  sprintf(file,"\n\n**************** Clicfg.txt INFO ****************\n");
  fwrite(file,1,strlen(file),fp);
  printConfig(fp);
  sprintf(file,"*******************************************\n");
  fwrite(file,1,strlen(file),fp);
  
  sprintf(file,"\n\n**************** COMMANDS INFO ****************\n");
  fwrite(file,1,strlen(file),fp);
  sprintf(file,"Count of Requested Commands    : %u\n",smTrkCli.cmdReqCnt);
  fwrite(file,1,strlen(file),fp);
  sprintf(file,"Count of Response for Commands : %u\n",smTrkCli.cmdRespCnt);
  fwrite(file,1,strlen(file),fp);
  sprintf(file,"***********************************************\n");
  fwrite(file,1,strlen(file),fp);

  sprintf(file,"\n\n**************** INVERTER INFO ***************\n");
  fwrite(file,1,strlen(file),fp);
  sprintf(file,"No Of Times Inverter Polling   : %u\n",smTrkCli.invpolling);
  fwrite(file,1,strlen(file),fp);
  sprintf(file,"No Of Times While Loop Repeat : %u\n",stats.invserHeartBeats);
  fwrite(file,1,strlen(file),fp);
  sprintf(file,"No Of Times TRK QFile Truncate: %u\n",smTrkCli.invfile++);
  fwrite(file,1,strlen(file),fp);

  sprintf(file, "         Req Count  Resp Count   Tmout Cnt  \n");
  fwrite(file,1,strlen(file),fp);
  for(ii=smTrkCli.invStart-1; ii<(smTrkCli.invCnt+smTrkCli.invStart-1); ii++)
  {
    sprintf(file, "INV%02d",ii+1);
    fwrite(file,1,strlen(file),fp);
    sprintf(file, "   %10u  %10u  %10u\n",
          smTrkCli.stats[ii].invdataReqCnt, smTrkCli.stats[ii].invdataRespCnt,
          smTrkCli.stats[ii].invdataTmoutCnt);
    fwrite(file,1,strlen(file),fp);
  }
  sprintf(file,"*********************************************\n");
  fwrite(file,1,strlen(file),fp);
  fclose(fp);
}




