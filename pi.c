// **********************************************************************************************
//                           p i . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : pi.c
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

// *******************************
// Section : Global Variables 
// *******************************

// *******************************
// Section : Code 
// *******************************
void *piDateTimeThreadFun(void *arg)
{
  char command[128],pi[128],msg[64],ms[128];
  unsigned long int dt_val;
  
  printf("Server Date & Time : %s\n",smTrkCli.serverdatetime);
  system("date");
  sprintf(command,"sudo date -s '%s'",smTrkCli.serverdatetime);
  system(command);
  
  time_t timer = time(NULL);
  struct tm *t = localtime(&timer);
  sprintf(pi,"%04d%02d%02d %02d%02d",(t->tm_year+1900),t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min);
 /* if(strcmp(pi,smTrkCli.serverdatetime) == 0)
  {
    printf("\t   Pi Date and Time Settings are OK\n");
    smTrkCli.piFlag = 1;
    while(1)
    {
      time_t timer = time(NULL);
      struct tm *t = localtime(&timer);
      sprintf(smTrkCli.datestr,"%02d:%02d:%02d:%02d:%02d:%04d",t->tm_hour,t->tm_min,t->tm_sec,
      t->tm_mday,t->tm_mon+1,t->tm_year+1900);
      sprintf(smTrkCli.dt,"%02d%02d%02d%02d%02d",t->tm_year+1900-2000,t->tm_mon+1,t->tm_mday,
      t->tm_hour,t->tm_min);
      sleep(30);
    }
  }
  else
  {
    printf("\t   Resend the Date Request to Server\n");
    sleep(5);
    smTrkCli.piFlag = 0;
    sprintf(msg,"date req\n");
    write(smTrkCli.invsockFd,msg,strlen(msg));
  }*/  
}




