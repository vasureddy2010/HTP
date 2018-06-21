// **********************************************************************************************
//                          q u e u e P r o c . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : queueProc.c
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
#include <time.h>
#include "smTrkCli.h"

// *******************************
// Section : Definitions 
// *******************************

// *******************************
// Section : Function Prototypes 
// *******************************
int Getdate_From_Pi();
int Getdate_From_Socket();

// *******************************
// Section : File Local Variables 
// *******************************
char  Flag = 0;
extern ReqFun_t AddressMap[];

// *******************************
// Section : Code 
// *******************************
int32_t readIntoRxq(struct RxStream *prs)
{
  int32_t nRxbytes;
  int32_t availSpace;
  int32_t rearSpace;
  char    lostData[80];
 
  if(prs->rxq.cnt == Q_SIZE)
  {
    nRxbytes = read(prs->fdNo, lostData, 80);
    if(nRxbytes > 0)
      return SUCCESS;
    if(nRxbytes < 0)
      return SOCK_READ_ERR;
    if(nRxbytes == 0)
      return SOCK_CLOSED_ERR;
  }

  availSpace = (Q_SIZE - prs->rxq.cnt);
  if(availSpace <= (Q_SIZE - prs->rxq.wix))
  {
    nRxbytes = read(prs->fdNo, &prs->rxq.qBuf[prs->rxq.wix],availSpace);
    if(nRxbytes < 0)
      return SOCK_READ_ERR;
    if(nRxbytes == 0)
      return SOCK_CLOSED_ERR;

    prs->rxq.wix = (prs->rxq.wix + nRxbytes) % Q_SIZE;
    prs->rxq.cnt = prs->rxq.cnt + nRxbytes;
    return SUCCESS;
  }

  rearSpace = Q_SIZE - prs->rxq.wix;
  nRxbytes = read(prs->fdNo, &prs->rxq.qBuf[prs->rxq.wix], rearSpace);
  if(nRxbytes < 0)
    return SOCK_READ_ERR;
  if(nRxbytes == 0)
    return SOCK_CLOSED_ERR;
  prs->rxq.wix = (prs->rxq.wix + nRxbytes) % Q_SIZE;
  prs->rxq.cnt = prs->rxq.cnt + nRxbytes;
  return SUCCESS;
}

int32_t readIntoMsg(int in_id,struct RxStream *prs)
{
  int ii;

  while(prs->rxq.cnt)
  {
    prs->msg[prs->mix] = prs->rxq.qBuf[prs->rxq.rix];
    
    prs->rxq.rix = (prs->rxq.rix + 1) % Q_SIZE;
    prs->rxq.cnt--;

    if(in_id == 1)
    {
      if(prs->msg[prs->mix] == '\r')
        continue;
      else if(prs->msg[prs->mix] == '\n')
      {
        prs->msg[prs->mix++] = 0;
        return SUCCESS;
      }
    }
//    printf("%02x-[%d], ",prs->msg[prs->mix],prs->mix);
    if(prs->msg[prs->mix] == 2*smTrkCli.RegCount) 
    {  
      if(prs->msg[(prs->mix) - 1] == smTrkCli.Function)
      { 
        if(prs->msg[(prs->mix) - 2] == smTrkCli.sid)
        { 
	  prs->msg[0] = prs->msg[(prs->mix) - 2];
          prs->msg[1] = smTrkCli.Function;
          prs->msg[2] = 2*smTrkCli.RegCount;
          prs->mix = 2;
	  Flag = 1;
        }
      } 
    }        
    prs->mix++;
    if(in_id == 2)
    {
      if(prs->mix == smTrkCli.NReg)
      if(Flag == 1)
      {
        if(prs->msg[1] != smTrkCli.Function)
        {
	 if(prs->msg[2] != 2*smTrkCli.RegCount)
         {
	    prs->mix = 0;
	    return PARTIAL_MSG_RCVD;
	 }
        }
        Flag = 0;
        prs->msg[prs->mix] = 0;
        return AddressMap[smTrkCli.paramState-1].Reqfun(++smTrkCli.respcount,prs->msg);
      }
    }
    if(prs->mix == MAX_MSG_SIZE)
    {
      printf("msg Ovf err\n");
      prs->mix = 0;
      prs->msgLenOvf++;
    }
  }
  return PARTIAL_MSG_RCVD;
}

int Getdate_From_Socket()
{
  int32_t dtsockFd;
  char    datastr[300],msg[300];
  struct sockaddr_in dtsrvadr,dtcliadr;

  dtsockFd = socket(AF_INET, SOCK_STREAM, 0);
  if(dtsockFd >= 0)
  {
     memset(&dtsrvadr,0,sizeof(dtsrvadr));
     dtsrvadr.sin_family      = AF_INET;
     dtsrvadr.sin_addr.s_addr = htonl(INADDR_ANY);
     dtsrvadr.sin_port        = htons(9000);
     if(connect(dtsockFd,(struct sockaddr *)&dtsrvadr, sizeof(dtsrvadr)) < 0)
     {
        printf("Connect fail in connecting to local server for Date\n\n");
        close(dtsockFd);
        return 0;
     }
     else
     {
        memset(datastr,'\0',300);
        read(dtsockFd, datastr,80);
        memset(datastr,'\0',300);
        strcpy(datastr, "dt\n");
        write(dtsockFd, datastr,strlen(datastr));
        read(dtsockFd, msg,80);
        strtok(msg, "\n");
        printf("Date From Local Server: %s\n",msg);
        strcpy(smTrkCli.datestr,msg);
        memset(datastr,'\0',300);
        strcpy(datastr, "q\n");
        write(dtsockFd, datastr,strlen(datastr)); 
     }
     close(dtsockFd);
  }
  else
  {
     printf("Can't open socket in local connection for Date\n\n");
     close(dtsockFd);
     return 0;
  }
  return 1;
}

int Getdate_From_Pi()
{
  int ii;

  time_t timer = time(NULL);
  struct tm *t = localtime(&timer);
  sprintf(smTrkCli.datestr,"%02d:%02d:%02d:%02d:%02d:%04d",t->tm_hour,t->tm_min,t->tm_sec,
      t->tm_mday,t->tm_mon+1,t->tm_year+1900);
  return 0;
}


