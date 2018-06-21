// **********************************************************************************************
//                           r e a d c f g . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : readcfg.C
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
#include "smTrkCli.h"

// *******************************
// Section : Definition 
// *******************************
#define MAX_CFG_NAMES  15        // No Of Parameters in clicfg.txt file
#define LINE_SIZE      80        // Max Length Of Each Parameters(Each Line)

enum
{
  SMB_SRV_IP_ADRS,
  SMB_SRV_PORT_NO,
  START_NO_SMB,
  NO_OF_SMBS,
  SITE_NAME,
  SITE_ID,
  SCAN_PERIOD,
  SCAN_TMOUT,
  SERIAL_DEVICE,
  INV_BAUDRATE,
  LOCAL_PORT_NO,
  SRV_USER_IP_ADRS,
  SRV_USER_PORT_NO
};

// *******************************
// Section : File Local Variables 
// *******************************
char *cfgParNames[MAX_CFG_NAMES] =
{
  "SMB_SRV_IP_ADRS",
  "SMB_SRV_PORT_NO",
  "START_NO_SMB",
  "NO_OF_SMBS",
  "SITE_NAME",
  "SITE_ID",
  "SCAN_PERIOD",
  "SCAN_TMOUT",
  "SERIAL_DEVICE",
  "INV_BAUDRATE",
  "LOCAL_PORT_NO",
  "SRV_USER_IP_ADRS",
  "SRV_USER_PORT_NO"
};


// *******************************
// Section : Code 
// *******************************
int getCfgParId(char *parName)
{
  int ii;

  for(ii=0; ii<MAX_CFG_NAMES; ii++)   
  {
    if(strcmp(cfgParNames[ii],parName)==0)
      return ii;
  }
  return -1;
}

int readClientConfig(char *cfgfile)
{
  char    linebuf[LINE_SIZE + 1];
  FILE    *fp;
  char    *tkn;
  int     cfgId;

  strcpy(smTrkCli.invIpAdrs, DEFAULT_INV_SRV_IP_ADRS);
  strcpy(smTrkCli.acdbIpAdrs,DEFAULT_SEN_SRV_IP_ADRS);
  strcpy(smTrkCli.mfmIpAdrs, DEFAULT_MFM_SRV_IP_ADRS);
  smTrkCli.invportnum      = DEFAULT_INV_PORT_NUM; 
  smTrkCli.acdbportnum     = DEFAULT_ACDB_PORT_NUM; 
  smTrkCli.mfmportnum      = DEFAULT_MFM_PORT_NUM; 
  smTrkCli.invStart        = DEFAULT_INVS_START;
  smTrkCli.invCnt          = DEFAULT_INVS_CNT;
  smTrkCli.acdbStart       = DEFAULT_ACDBS_START;
  smTrkCli.acdbCnt         = DEFAULT_ACDBS_CNT;
  smTrkCli.mfmStart        = DEFAULT_MFMS_START;
  smTrkCli.mfmCnt          = DEFAULT_MFMS_CNT;
  strcpy(smTrkCli.siteName,  DEFAULT_SITE_NAME);
  smTrkCli.siteId          = DEFAULT_SITE_ID;  
  smTrkCli.scanPeriod      = DEFAULT_SER_SCAN_PERIOD; 
  smTrkCli.tmoutPeriod     = DEFAULT_DEV_RESP_TMOUT;
  strcpy(smTrkCli.ser_dev,   DEFAULT_SERIAL_DEVICE);
  smTrkCli.inv_baud        = DEFAULT_INV_BAUDRATE;
  smTrkCli.acdb_baud       = DEFAULT_ACDB_BAUDRATE;
  smTrkCli.mfm_baud        = DEFAULT_MFM_BAUDRATE;
  smTrkCli.localportnum    = DEFAULT_LOCAL_PORT_NUM;  
  strcpy(smTrkCli.srvuserIpAdrs, DEFAULT_SRV_USER_IP_ADRS);
  smTrkCli.srvuserportnum  = DEFAULT_SRV_USER_PORT_NUM;  

  fp = fopen(cfgfile, "r");
  if(!fp)
  {
    perror("opening clicfg.txt");
    return 1;
  }

  while(fgets(linebuf,LINE_SIZE,fp))
  {
    tkn = strtok(linebuf," =\r\n");
    if(!tkn)
      continue;
    cfgId = getCfgParId(tkn);
    tkn = strtok(NULL," =\r\n");
    if(!tkn)
      continue;

    switch(cfgId)
    {
      case SMB_SRV_IP_ADRS:
        strcpy(smTrkCli.invIpAdrs,tkn);
        break;

      case SMB_SRV_PORT_NO:
        sscanf(tkn, "%d", &smTrkCli.invportnum);
        break;

      case START_NO_SMB:
        sscanf(tkn, "%d", &smTrkCli.invStart);
        break;

      case NO_OF_SMBS:
        sscanf(tkn, "%d", &smTrkCli.invCnt);
        break;

      case SITE_NAME:
        strcpy(smTrkCli.siteName,tkn);
        break;

      case SITE_ID:
        sscanf(tkn, "%d", &smTrkCli.siteId);
        break;

      case SCAN_PERIOD:
        sscanf(tkn, "%d", &smTrkCli.scanPeriod);
        break;

      case SCAN_TMOUT:
        sscanf(tkn, "%d", &smTrkCli.tmoutPeriod);
        break;

      case SERIAL_DEVICE:
        strcpy(smTrkCli.ser_dev,tkn);
        break;

      case INV_BAUDRATE:
        sscanf(tkn, "%d", &smTrkCli.inv_baud);
        break;
 
      case LOCAL_PORT_NO:
        sscanf(tkn, "%d", &smTrkCli.localportnum);
        break;
  
      case SRV_USER_IP_ADRS:
        strcpy(smTrkCli.srvuserIpAdrs,tkn);
        break;

      case SRV_USER_PORT_NO:
        sscanf(tkn, "%d", &smTrkCli.srvuserportnum);
        break;

      default:
        break;
    }
  }
  fclose(fp);

  printf("SMB's Server IP address      : %s\n",smTrkCli.invIpAdrs);
  printf("SMB's Server PORT number     : %d\n",smTrkCli.invportnum);
  printf("Starting No of SMBs          : %d\n",smTrkCli.invStart);
  printf("No of SMBs                   : %d\n",smTrkCli.invCnt);
  printf("Site name                    : %s\n",smTrkCli.siteName);
  printf("Site Id                      : %d\n",smTrkCli.siteId);
  printf("Scan Period                  : %d\n",smTrkCli.scanPeriod);
  printf("Scan Timeout                 : %d\n",smTrkCli.tmoutPeriod);
  printf("Serial Device                : %s\n",smTrkCli.ser_dev);
  printf("SMB's Baud Rate              : %d\n",smTrkCli.inv_baud);
  printf("Local Interface Port Number  : %d\n",smTrkCli.localportnum);
  printf("Server Interface IP address  : %s\n",smTrkCli.srvuserIpAdrs);
  printf("Server Interface Port Number : %d\n",smTrkCli.srvuserportnum);
  printf("\n\n");  
  return SUCCESS;
} 

void printConfig(FILE *fp)
{
  fprintf(fp,"SMB's Server IP address      : %s\n",smTrkCli.invIpAdrs);
  fprintf(fp,"SMB's Server PORT number     : %d\n",smTrkCli.invportnum);
  fprintf(fp,"Starting No of SMB           : %d\n",smTrkCli.invStart);
  fprintf(fp,"No of SMBs                   : %d\n",smTrkCli.invCnt);
  fprintf(fp,"Site name                    : %s\n",smTrkCli.siteName);
  fprintf(fp,"Site Id                      : %d\n",smTrkCli.siteId);
  fprintf(fp,"Scan Period                  : %d\n",smTrkCli.scanPeriod);
  fprintf(fp,"Scan Timeout                 : %d\n",smTrkCli.tmoutPeriod);
  fprintf(fp,"Serial Device                : %s\n",smTrkCli.ser_dev);
  fprintf(fp,"SMB's Baud Rate              : %d\n",smTrkCli.inv_baud);
  fprintf(fp,"Local Interface Port Number  : %d\n",smTrkCli.localportnum);
  fprintf(fp,"Server Interface IP address  : %s\n",smTrkCli.srvuserIpAdrs);
  fprintf(fp,"Server Interface Port Number : %d\n",smTrkCli.srvuserportnum);
}


