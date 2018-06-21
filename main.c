// **********************************************************************************************
//                           m a i n . c 
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : main.c
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
// Section : Definition 
// *******************************
#define CONFIG_FILE       "clicfg.txt"

// **********************************
// Section : Static & Local Variables
// **********************************
struct SmartTrakClient smTrkCli;
struct clientStats     stats;

// *******************************
// Section : Function Prototypes 
// *******************************
void *docli(void *arg);
void *scanThreadFun(void *arg);
void *INVThreadFun(void *arg);


// *******************************
// Section : Code 
// *******************************
int32_t readAndProcSockMsg(struct RxStream *pstrm)
{
  int32_t stat;

  stat = readIntoRxq(pstrm);
  if(stat == SUCCESS)
  {
    while(readIntoMsg(1,pstrm)==SUCCESS)
    {
      processSockMsg(pstrm->msg,pstrm->fdNo);
      pstrm->mix = 0;
    }
  }
  return stat;
}

int32_t main(int argc, char *argv[])
{
  fd_set             readfds;
  int32_t            stat;
  struct timeval     tmout;
  pthread_t          scanTh,cliTh,invTh,acdbTh,mfmTh;
  
  printf("\n\n\t********** SMARTTRAK CLIENT APPLICATION **********\n");

  // Reading The Client Configuration File
  if(readClientConfig(CONFIG_FILE) != SUCCESS)
    exit(1);

  // Reading Inverter,Sensor,MFM Q files
  if(smTrkCli.invCnt != 0)
    InvfileQInit();

  // Create Thread for Local User Support
  pthread_create(&cliTh, NULL, docli, NULL);

  // Create Thread for Scanning
  pthread_create(&scanTh, NULL, scanThreadFun, NULL);

  // Create Thread for Inverter
  if(smTrkCli.invCnt != 0)
        pthread_create(&invTh, NULL, INVThreadFun, NULL);

 
  // Stay Here  
  while(1);
}   

