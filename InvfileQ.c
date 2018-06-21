// **********************************************************************************************
//                           I n v f i l e Q . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : InvfileQ.c
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
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "smTrkCli.h"

// *******************************
// Section : Definition 
// *******************************
#define FQ_FILE_NAME   "Qfile1.txt"
#define FQ_RECORD_LEN  200
#define MAXRECORS1     1000      

// *******************************
// Section : Local Variables 
// *******************************
struct FileQueue
{
   long int fdNo;
   long int wix;
   long int rix;
   long int cnt;
};
struct FileQueue fq1;


// *******************************
// Section : Code 
// *******************************
int32_t getFirstNewRecIx_Inv(int32_t fdNo)
{
  char rec[FQ_RECORD_LEN];
  int ii=0;

  lseek(fdNo,0, SEEK_SET);
  while(read(fdNo,rec,FQ_RECORD_LEN) == FQ_RECORD_LEN)
  {
    if(rec[0] == 'N')
      return ii; 
    ii++;
  }
  return ii;
}

int32_t InvfileQInit()
{
  long int  rcnt; 
  off_t   flen;
   
  printf("Q FILES STATUS() :\n------------------\n");
  fq1.fdNo = open(FQ_FILE_NAME, O_CREAT|O_RDWR,0666);
  if(fq1.fdNo < 0)
    exit(2);

  flen = lseek(fq1.fdNo,0, SEEK_END);
  if(flen % FQ_RECORD_LEN)
    printf("\tInvalid file length\n");

  rcnt = flen / FQ_RECORD_LEN;
  printf("\tTotal No. Of Records in QFile1.txt(SMB)  = %ld\n",rcnt);

  if(rcnt==0)
    fq1.wix = fq1.rix = fq1.cnt = rcnt;
  else if(rcnt>0)
  {
    fq1.rix = getFirstNewRecIx_Inv(fq1.fdNo);
    fq1.cnt = (rcnt-fq1.rix);
    fq1.wix = rcnt;
  }
  printf("\tTotal No. Of New Records to Send         = %ld\n\n",(rcnt-fq1.rix));
}

int32_t InvfileQWrite(uint8_t *appdata)
{
  int ii,len;
  char rec[FQ_RECORD_LEN];

  len = strlen(appdata); 
  if(len > (FQ_RECORD_LEN-4))
  {
    printf("FILE() : Record length is more than the 496 bytes\n");
    return 1;
  }
  sprintf(rec,"N %s#",appdata); 

  for(ii=len+3; ii<(FQ_RECORD_LEN-1); ii++)
    rec[ii] = ' ';
  rec[ii] = '\n';

  lseek(fq1.fdNo,fq1.wix * FQ_RECORD_LEN, SEEK_SET);
  write(fq1.fdNo,rec,FQ_RECORD_LEN);

  fq1.cnt++; 
  fq1.wix++;
}

int32_t InvfileQCnt()
{
  return fq1.cnt;
}

int32_t InvfileQRead(uint8_t *appdata)
{
  int ii,jj,len;
  char rec[FQ_RECORD_LEN];

  if(fq1.cnt==0)
    return 1;

  lseek(fq1.fdNo,fq1.rix * FQ_RECORD_LEN, SEEK_SET);
  read(fq1.fdNo,rec,FQ_RECORD_LEN);
  if(rec[0]!='N')
    printf("Error: read record is not new one\n");
  rec[0] = ' ';
  lseek(fq1.fdNo,(-1)*FQ_RECORD_LEN, SEEK_CUR);
  write(fq1.fdNo,rec,FQ_RECORD_LEN);

  ii=2;
  jj=0;
  while((ii<FQ_RECORD_LEN) && (rec[ii] != '#'))
  {
    appdata[jj++] = rec[ii++];
  }
  appdata[jj] = 0;

  fq1.cnt--; 
  fq1.rix++; 
  if((fq1.wix == fq1.rix) && (fq1.rix > MAXRECORS1))
  {
    smTrkCli.invfile++;
    printf("QFILE() : Qfile1.txt is truncated\n");
    fq1.fdNo = open(FQ_FILE_NAME, O_CREAT|O_RDWR|O_TRUNC,0666);
    fq1.rix = 0;
    fq1.wix = 0;
  }
  return 0;
}

void printInvFQstats(FILE *fp)
{
  fprintf(fp, "Qfile1.txt FD : %ld\n",fq1.fdNo);
  fprintf(fp, "Read Index   : %ld\nWrite Index   : %ld\n",fq1.rix,fq1.wix);
}



