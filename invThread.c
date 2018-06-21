// **********************************************************************************************
//                           i n v T h r e a d . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : invThread.c
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
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "smTrkCli.h"

// *******************************
// Section : Definitions 
// *******************************

// *******************************
// Section : Function Prototypes 
// *******************************

// ***********************************
// Section : Static &  Local Variables 
// ***********************************
static char    invData[300];
static int32_t invDataPending;

// *******************************
// Section : Global Variables 
// *******************************

// *******************************
// Section : Code 
// *******************************

void ackPendingInvData()
{
  invDataPending = 0;
}

void postInvData()
{
  char  msg[500];

  if(invDataPending)
  {
    sprintf(msg, "data %s\n", invData);
    write(smTrkCli.invsockFd,msg,strlen(msg));
    smTrkCli.invsockTransState   = SOCK_TRANS_RESP_WAIT;
    smTrkCli.invsockMsgSentTime  = time(NULL);
  }
  else if(InvfileQCnt() > 0)
  {
    InvfileQRead(invData);
    invDataPending = 1;
    sprintf(msg, "data %s\n", invData);
    write(smTrkCli.invsockFd,msg,strlen(msg));
    smTrkCli.invsockTransState   = SOCK_TRANS_RESP_WAIT;
    smTrkCli.invsockMsgSentTime  = time(NULL);
  }
}

int32_t connect2INVServer()
{
  int32_t            sockFd;
  char               msg[80];
  struct sockaddr_in srvadr,cliadr;

  sockFd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockFd >= 0)
  {
    memset(&srvadr,0,sizeof(srvadr));
    srvadr.sin_family      = AF_INET;
    srvadr.sin_addr.s_addr = inet_addr(smTrkCli.invIpAdrs);
    srvadr.sin_port        = htons(smTrkCli.invportnum);

    if(connect(sockFd,(struct sockaddr *)&srvadr, sizeof(srvadr)) < 0)
    {
      smTrkCli.invsockConErrno = errno;
      smTrkCli.invsockConFailCnt++;
      close(sockFd);
      return 1;
    }
    else
    {
      smTrkCli.invsockState     = SOCK_CONNECTED;
      sprintf(msg, "site %d %s %d\n",smTrkCli.siteId,smTrkCli.siteName,smTrkCli.invCnt);
      write(sockFd, msg,strlen(msg));
      smTrkCli.invsockTransState = SOCK_TRANS_RESP_WAIT;
      smTrkCli.invsockConSuccessCnt++;
      smTrkCli.invsockStrm.fdNo = sockFd;
      smTrkCli.invsockFd = sockFd;
      return SUCCESS;
    }
  }
  else
  {
    printf("Can't Open Stream Socket(Inverter Client)\n");
    return 1;
  }
}

void *INVThreadFun(void *arg)
{
  fd_set             readfds;
  int32_t            stat;
  struct timeval     tmout;

  FD_ZERO(&readfds);
  while(1)
  {
    stats.invsockHeartBeats++;
    switch(smTrkCli.invsockState)
    {
      case SOCK_NOT_CONNECTED:
           if(connect2INVServer() != SUCCESS)
            {
                 sleep(CONNECT_FAIL_SLEEP);
            }
            break;

      case SOCK_CONNECTED:
            FD_SET(smTrkCli.invsockFd, &readfds);
            tmout.tv_sec  = 1;
            tmout.tv_usec = 0;
            stat = select(smTrkCli.invsockFd+1, &readfds, NULL, NULL, &tmout);
            if(stat > 0)
            {
              if(FD_ISSET(smTrkCli.invsockFd, &readfds))
              {
               stats.invsockSelCnt++;
               stat = readAndProcSockMsg(&smTrkCli.invsockStrm);
               if(stat != SUCCESS)
               {
                  printf("Error on Socket(Inverter Client)\n");
                  smTrkCli.invsockRdFailCnt++;
                  close(smTrkCli.invsockFd);
                  smTrkCli.invsockState = SOCK_NOT_CONNECTED;
                  continue;
               }
              }
            }
            else if (stat <0)
            {
              printf("Select Error(Inverter Client)\n");
            }
            if(smTrkCli.invsockTransState == SOCK_TRANS_IDLE)
            {
              postInvData();
            } 
            else if(smTrkCli.invsockTransState == SOCK_TRANS_RESP_WAIT)
            {
             
             if((time(NULL) - smTrkCli.invsockMsgSentTime) > SOCK_RESP_TMOUT_PERIOD)
             {
               close(smTrkCli.invsockFd);
               smTrkCli.invsockRespTmoutCnt++;
               smTrkCli.invsockState = SOCK_NOT_CONNECTED;
             }
            }       
     }
  }
}

