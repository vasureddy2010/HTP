// **********************************************************************************************
//                           s c a n n e r . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : scanner.c
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
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "smTrkCli.h"

// *******************************
// Section : Function Prototypes 
// *******************************
void Frame_Format(unsigned char Sid,char Function,unsigned short int Start_Add,unsigned short int NReg,char *MBFrame);

// *******************************
// Section : Definitions 
// *******************************
#define msleep    (100*1000)

ReqFun_t AddressMap[] = 
{
  {"SMB",   0x03, 2999,  120,  "Parameter0",  ProcessParam0},
  {"SMB",   0x03, 2699,  30,  "Parameter1",  ProcessParam1},
  {"SMB",   0x04, 00,  0,  "Parameter2",  ProcessParam2},
  {"SMB",   0x04, 00,  0,  "Parameter3",  ProcessParam3},
  {"SMB",   0x04, 00,  0,  "Parameter4",  ProcessParam4},
  {"SMB",   0x04, 00,  0,  "Parameter5",  ProcessParam5},
  {"SMB",   0x04, 00,  0,  "Parameter6",  ProcessParam6},
  {"SMB",   0x04, 00,  0,  "Parameter7",  ProcessParam7},
  {"SMB",   0x04, 00,  0,  "Parameter8",  ProcessParam8},
  {"SMB",   0x04, 00,  0,  "Parameter9",  ProcessParam9},
};

// *******************************
// Section : Code 
// *******************************
int32_t readAndProcSerMsg(struct RxStream *pstrm)
{
  int32_t stat;

  stat = readIntoRxq(pstrm);
  if(stat == SUCCESS)
  {
    while(readIntoMsg(2,pstrm)==SUCCESS)
    {
      processSerMsg(pstrm->msg);
      pstrm->mix = 0;
    }
  }
  return stat;
}

int32_t SMBScan()
{
  int32_t            serFd,stat;
  fd_set             readfds;
  struct timeval     tmout;
  unsigned char      msg[500];

  serFd = smTrkCli.serStrm.fdNo;
  smTrkCli.paramState = 0;
  while(1)
  {
    FD_ZERO(&readfds);
    FD_SET(serFd, &readfds);

    tmout.tv_sec  = 1;
    tmout.tv_usec = 0;
    stat = select(serFd+1, &readfds, NULL, NULL, &tmout);
    stats.invserHeartBeats++;
    if(stat > 0)
    {
      if(FD_ISSET(serFd, &readfds))
      { 
        stats.invserSelCnt++;
        stat = readAndProcSerMsg(&smTrkCli.serStrm);
      }
    }
    else if(stat <0)
    {
      perror("select error\n");
    }

    if(smTrkCli.rmtCmds.flg)
    {
       write(serFd, smTrkCli.rmtCmds.msg,strlen(smTrkCli.rmtCmds.msg));
       smTrkCli.rmtCmds.flg = 0;
       smTrkCli.cmdReqCnt++;          
       usleep(msleep);
    }
    if(smTrkCli.scanState == SER_SCAN_INIT)
    {
      smTrkCli.scanState = SER_SCAN_ACTIVE;
      smTrkCli.curDevIx  = smTrkCli.invStart - 1;
      smTrkCli.respcount = 0;      
      Frame_Format(smTrkCli.curDevIx+1,AddressMap[smTrkCli.paramState].funcode,AddressMap[smTrkCli.paramState].address,AddressMap[smTrkCli.paramState].length,msg);   
      printf("P%d: %02x %02x %02x %02x %02x %02x %02x %02x\n",smTrkCli.paramState,msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6],msg[7]);
      write(serFd, msg,8);
      smTrkCli.paramState++;
      smTrkCli.stats[smTrkCli.curDevIx].invdataReqCnt++;
      smTrkCli.devState    = DEV_DATAREQ_SENT;
      smTrkCli.devSentTime = time(NULL);
      usleep(msleep); 
    }
    else if(smTrkCli.scanState == SER_SCAN_ACTIVE)
    {
      if((time(NULL) - smTrkCli.devSentTime) > smTrkCli.tmoutPeriod)
      {
        smTrkCli.stats[smTrkCli.curDevIx].invdataTmoutCnt++;
        if(smTrkCli.curDevIx == (smTrkCli.invCnt+smTrkCli.invStart-1))
        {
          smTrkCli.curDevIx  = 0;
          smTrkCli.scanState = SER_SCAN_DONE;
          smTrkCli.devState  = DEV_MSG_NONE;
        }        
	else 
        {
         if(smTrkCli.paramState < INV_PARAM_CNT)
         {
          Frame_Format(smTrkCli.curDevIx+1,AddressMap[smTrkCli.paramState].funcode,AddressMap[smTrkCli.paramState].address,AddressMap[smTrkCli.paramState].length,msg);   
          printf("P%d: %02x %02x %02x %02x %02x %02x %02x %02x\n",smTrkCli.paramState,msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6],msg[7]);
          write(serFd, msg,8);
          smTrkCli.paramState++;
          smTrkCli.stats[smTrkCli.curDevIx].invdataReqCnt++;
          smTrkCli.devState    = DEV_DATAREQ_SENT;
          smTrkCli.devSentTime = time(NULL);
          usleep(msleep); 
         }
         else
         {
           smTrkCli.respcount = 0;
           smTrkCli.curDevIx++;
           smTrkCli.paramState = 0;
         }
        }
      }
    }
    else if(smTrkCli.scanState == SER_SCAN_DONE)
    {
 /*     if((smTrkCli.mfmCnt == 0) && (smTrkCli.acdbCnt == 0))
      {
        printf("done\n");
        smTrkCli.invFlag  = 0;
        smTrkCli.lastScanTime = time(NULL);
        return 0;
      }
      else
      {  
        printf("else sdg\n"); 
        smTrkCli.lastScanTime = time(NULL);
        smTrkCli.scanState = SER_SCAN_INIT;
        smTrkCli.invFlag  = 0;
        return 0;
      }
  */
      smTrkCli.invFlag  = 0;
      smTrkCli.lastScanTime = time(NULL);
      //printf("%d %d %d\n",time(NULL),smTrkCli.lastScanTime,smTrkCli.scanPeriod);
      return 0;
    }
  } 
}

void *scanThreadFun(void *arg)
{
  int32_t            serFd,sockFd,clIx,maxFd;
  struct sockaddr_in srvadr,cliadr;
  fd_set             readfds;
  int32_t            stat;
  struct timeval     tmout;
  char               msg[80];

  sleep(1);
  while(1)
  {
    if((smTrkCli.invCnt != 0) && (smTrkCli.scanState == SER_SCAN_INIT))
    {
      printf("Inverter Scanning :\n------------------\n");
      serFd = serialInit(smTrkCli.ser_dev, smTrkCli.inv_baud);
      if(serFd < 0)
      {
        perror("serial open fail: ");
        exit(1);
      }
      smTrkCli.serStrm.fdNo = serFd;
      smTrkCli.invFlag = 1;
      smTrkCli.invpolling++;
      SMBScan();
      close(serFd);  
    }
    if((time(NULL) - smTrkCli.lastScanTime) > smTrkCli.scanPeriod)
    { 
       //printf("%d %d %d\n",time(NULL),smTrkCli.lastScanTime,smTrkCli.scanPeriod);
       smTrkCli.scanState = SER_SCAN_INIT;
    }       
  }
}   

int32_t sendNextScanReq(int serFd)
{
  char msg[500];

  if((smTrkCli.paramState == INV_PARAM_CNT) && (smTrkCli.invFlag == 1)) 
  {
    smTrkCli.paramState = 0;
    smTrkCli.respcount = 0;
    smTrkCli.curDevIx++;
  }

  if((smTrkCli.curDevIx == (smTrkCli.invCnt+smTrkCli.invStart-1)) && (smTrkCli.invFlag == 1))      
  { 
    smTrkCli.curDevIx  = 0;
    smTrkCli.scanState = SER_SCAN_DONE;
    smTrkCli.devState  = DEV_MSG_NONE;
  }
  else
  {
    if((smTrkCli.invFlag == 1) && (smTrkCli.invCnt != 0))
    {  
      Frame_Format(smTrkCli.curDevIx+1,AddressMap[smTrkCli.paramState].funcode,AddressMap[smTrkCli.paramState].address,AddressMap[smTrkCli.paramState].length,msg);   
      printf("P%d: %02x %02x %02x %02x %02x %02x %02x %02x\n",smTrkCli.paramState,msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6],msg[7]);
      write(serFd, msg,8);
      smTrkCli.paramState++;
      smTrkCli.devState    = DEV_DATAREQ_SENT;
      smTrkCli.devSentTime = time(NULL);
      smTrkCli.stats[smTrkCli.curDevIx].invdataReqCnt++;
      usleep(msleep);
    }
  }
}



