// **********************************************************************************************
//                           M o d b u s . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : Modbus.c
// Description:
// Date       : 04-11-2015 
//              04-03-2016 -> Modifying as per karvy
// Author     : FirmWare Team
// Compilers  : Gcc / ARM 
// **********************************************************************************************

// *******************************
// Section : Includes 
// *******************************
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "smTrkCli.h"

// *******************************
// Section : Definitions 
// *******************************
int ProcessQueueMsg(char *Resp);
void set_Date_mysql();
// **********************************
// Section : Static & Local Variables 
// **********************************
char Parameter[100][500];

char  Parameter0[300];
char  Parameter1[300];
char  Parameter2[300];
char  Parameter3[300];
char  Parameter4[300];
char  Parameter5[300];
char  Parameter6[300];
char  Parameter7[300];
char  Parameter8[300];
char  Parameter9[300];
char  Parameter10[300];
char  Parameter11[300];
char  Parameter12[300];
char  Parameter13[300];
char  Parameter14[300];
char  Parameter15[300];
char  Parameter16[300];
char  Parameter17[300];
char  Parameter18[300];
char  Parameter19[300];
char  Parameter20[300];
char  Parameter21[300];
char  Parameter22[300];
char  Parameter23[300];
char  Parameter24[300];
char  Parameter25[300];
char  Parameter26[300];
char  Parameter27[300];
char  Parameter28[300];
char  Parameter29[300];

float InstP[10];
static unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

  
static char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;


// *******************************
// Section : Function Prototypes 
// *******************************
int32_t sendNextScanReq(int serFd);
uint8_t ReadYearRegister();
uint8_t ReadMonthRegister();
uint8_t ReadDateRegister();
uint8_t ReadHourRegister();
uint8_t ReadMinuteRegister();


// *******************************
// Section : Code 
// *******************************
unsigned short int CRC16 (unsigned char *puchMsg, unsigned short usDataLen )
{
  unsigned char uchCRCHi = 0xff;
  unsigned char uchCRCLo = 0xff;
  unsigned int  uIndex;

  while(usDataLen--)
  {
   uIndex = uchCRCLo ^ *puchMsg++;
   uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
   uchCRCHi = auchCRCLo[uIndex];
  }
  return (uchCRCHi << 8 | uchCRCLo);
}

void Frame_Format(unsigned char Sid,char Function,unsigned short int Start_Add,unsigned short int NReg,char *MBFrame)
{
    unsigned short int CRC;

    //-------------------------------------------------------------------//
    // S.ID | Function | Addess | No.Of.Regs | CRC                       //
    //-------------------------------------------------------------------//
    MBFrame[0] = Sid;
    MBFrame[1] = Function;

    MBFrame[3] = Start_Add & 0x00ff;
    MBFrame[2] = Start_Add >> 8;

    MBFrame[5] = NReg & 0x00ff;
    MBFrame[4] = NReg >> 8;
   
    CRC = CRC16 ( MBFrame, 6 );

    MBFrame[6] = CRC & 0x00ff;		
    MBFrame[7] = CRC >> 8;		

    smTrkCli.sid = Sid;
    smTrkCli.RegCount = NReg;
    smTrkCli.NReg   = ((2*smTrkCli.RegCount)+5);
    smTrkCli.Function = Function;   
}

unsigned int get_value(char LB, char UB)
{
    unsigned int n1,n2;
    n1 = LB;
    n2 =  0x000000FF & UB;
    n1 = n1<<8 ;
    n2  = n1 + n2;
    return n2;
}

unsigned int get32int(char c1, char c2,char c3,char c4)
{
    unsigned int n1;

    n1 = ((0xFF & c1) << 24) | ((0xFF & c2) << 16) | ((0xFF & c3) << 8) | (0xFF   & c4);
    return n1;
}

float get32float(char c3, char c4,char c1,char c2)
{
    unsigned int n32;
    float mantf,f32;
    int sign , expo, manti;
    int mantd = 0x800000;
    int mantn = 0x400000;

    n32 = get32int(c1,c2,c3,c4);

    if(n32 & 0x80000000)
        sign = -1;
    else
        sign = 1;

    expo = (n32 & 0x7f800000) >> 23;

    if(expo > 0)
    {
        expo -= 127;
        manti = (n32 & 0x007FFFFF);
        mantf = (float)manti/mantd + 1;
    }
    else
    {   
        expo -= 127;
        manti = (n32 & 0x007FFFFF);
        mantf = (float)manti/mantn;
    }
    f32 = sign * mantf * pow(2.0,expo);
    return f32;
}

// *************************
// Inverter Functions
// *************************
int32_t Frame_Response(int count, char *msg)
{
  int ii;
  char cur_date[30],cur_time[30],command[128];
//  set_Date_mysql();  
  if(smTrkCli.invFlag == 1)
  {
    if(count != INV_PARAM_CNT)
    {
      sendNextScanReq(smTrkCli.serStrm.fdNo);    
      return PARTIAL_MSG_RCVD;
    }
    set_Date_mysql();
    sprintf(msg,"MFM%02d",msg[0]);
    PiRTCC2:
//    if(smTrkCli.piFlag == 1)
    {
        time_t timer = time(NULL);
        struct tm *t = localtime(&timer);
        sprintf(cur_date,"%02d:%02d:%04d",t->tm_mday,t->tm_mon+1,t->tm_year+1900);
        sprintf(cur_time,"%02d:%02d:%02d",t->tm_hour,t->tm_min,t->tm_sec);
    }
  /*  else
    {
        sprintf(command,"sudo date -s '%04d%02d%02d %02d%02d'",ReadYearRegister()+2000,ReadMonthRegister(),ReadDateRegister(),ReadHourRegister(),ReadMinuteRegister());
        system(command);
        smTrkCli.piFlag = 1;
        goto PiRTCC2;
    }*/
    strcat(msg,":");
    strcat(msg,cur_time);
    strcat(msg,":");
    strcat(msg,cur_date);

    for(ii=0;ii<INV_PARAM_CNT;ii++)
    {
      strcat(msg,":");
      strcat(msg,Parameter[ii]);
    }
    strcat(msg,"\n");
  }

  printf("msg:%s",msg);

  ProcessQueueMsg(msg);

  return SUCCESS;
}


union 
{
	char data_char[4];
	int data_int;	
	float data_float;	
}CONVERT32;


float get_flt(char c1,char c2,char c3,char c4)
{
 CONVERT32.data_char[0] = c4;
 CONVERT32.data_char[1] = c3;
 CONVERT32.data_char[2] = c1;
 CONVERT32.data_char[3] = c2;
 printf("\n%02x,%02x,%02x,%02x,%f\n",c1,c2,c3,c4,CONVERT32.data_float);
 return CONVERT32.data_float;
}



int ProcessParam0(int count,char *msg)
{
  unsigned short int P1,P2,P3,P4,P5,P6,P7,P8,P9,P10;
  unsigned short int P11,P12,P13,P14,P15,P16,P17,P18,P19,P20;
  unsigned short int P21,P22,P23,P24,P25,P26,P27,P28,P29,P30, P31,P32,P33,P34,P35,P36,P37,P38,P39,d1,d2;
 float R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16,R17,R18,R19,R20;   

  unsigned int Q1;
  R1 = get32float(msg[5],msg[6],msg[3],msg[4]);
  R2 = get32float(msg[9],msg[10],msg[7],msg[8]);
  R3 = get32float(msg[13],msg[14],msg[11],msg[12]);
  R4 = get32float(msg[25],msg[26],msg[23],msg[24]);

  R5 =  get32float(msg[45],msg[46],msg[43],msg[44]);
  R6 = get32float(msg[49],msg[50],msg[47],msg[48]);
  R7 =  get32float(msg[53],msg[54],msg[51],msg[52]);
  R8 = get32float(msg[57],msg[58],msg[55],msg[56]);

  R9 = get32float(msg[61],msg[62],msg[59],msg[60]);
  R10 = get32float(msg[65],msg[66],msg[63],msg[64]);
  R11 = get32float(msg[69],msg[70],msg[67],msg[68]);
  R12 = get32float(msg[77],msg[78],msg[75],msg[76]);
 
  R13 = get32float(msg[125],msg[126],msg[123],msg[124]);
  R14 = get32float(msg[141],msg[142],msg[139],msg[140]);
  R15 = get32float(msg[157],msg[158],msg[155],msg[156]);


  R16 = get32float(msg[161],msg[162],msg[159],msg[160]);
  R17 = get32float(msg[165],msg[166],msg[163],msg[164]);
  R18 = get32float(msg[169],msg[170],msg[167],msg[168]);
  R19 = get32float(msg[173],msg[174],msg[171],msg[172]);
  R20 = get32float(msg[225],msg[226],msg[223],msg[224]);


//  printf("\n%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15);
 /// printf("\n%.2f,%.2f,%.2f,%.2f,%.2f",R16,R17,R18,R19,R20);
  
  sprintf(Parameter[0],"%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f:%.2f",R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16,R17,R18,R19,R20);
 printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[0]);
  return Frame_Response(count,msg);
}

int ProcessParam1(int count,char *msg)
{
  unsigned short int P1,P2;

float R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16,R17,R18,R19,R20;


  R1 = get32float(msg[5],msg[6],msg[3],msg[4]);
  R2 = get32float(msg[9],msg[10],msg[7],msg[8]);
 
 R3 = get32float(msg[21],msg[22],msg[19],msg[20]);
  R4 = get32float(msg[25],msg[26],msg[23],msg[24]);

  R5 =  get32float(msg[37],msg[38],msg[35],msg[36]);
  R6 = get32float(msg[41],msg[42],msg[39],msg[40]);

 //printf("\n%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",R1,R2,R3,R4,R5,R6);

  sprintf(Parameter[1],"%.2f:%.2f:%.2f:%.2f:%.2f:%.2f",R1,R2,R3,R4,R5,R6);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[1]);
  return Frame_Response(count,msg);

}

int ProcessParam2(int count,char *msg)
{
  unsigned short int P1,P2,P3;
  unsigned int Q1;
  //printf("In Parameter2\n");

  P1 = get_value(msg[3],msg[4]);
  printf("Total Instant Current : %d\n",P1);
  P2 = get_value(msg[5],msg[6]);
  P3 = get_value(msg[7],msg[8]);
  printf("%d %d\n",P2,P3);
  Q1 = get32int(msg[5],msg[6],msg[7],msg[8]);  
  sprintf(Parameter[2],"%d:%d",P1,Q1);
  printf("P%d -> SMB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[2]);
  return Frame_Response(count,msg);
}

int ProcessParam3(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter3\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[3],"%d",P1);
  printf("P%d -> SMB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[3]);
  return Frame_Response(count,msg);
}

int ProcessParam4(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter4\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[4],"%d",P1);
  printf("P%d -> SMB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[4]);
  return Frame_Response(count,msg);
}

int ProcessParam5(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter5\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[5],"%d",P1);
  printf("P%d -> SMB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[5]);
  return Frame_Response(count,msg);
}
int ProcessParam6(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter6\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[6],"%d",P1);
  printf("P%d -> SMB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[6]);
  return Frame_Response(count,msg);
}

int ProcessParam7(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter7\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[7],"%d",P1);
  printf("P%d -> SMB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[7]);
  return Frame_Response(count,msg);
}

int ProcessParam8(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter8\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[8],"%d",P1);
  printf("P%d -> SMB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[8]);
  return Frame_Response(count,msg);
}

int ProcessParam9(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter9\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[9],"%d",P1);
  printf("P%d -> SMB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[9]);
  return Frame_Response(count,msg);
}

int ProcessParam10(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter10\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[10],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[10]);
  return Frame_Response(count,msg);
}

int ProcessParam11(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter11\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[11],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[11]);
  return Frame_Response(count,msg);
}

int ProcessParam12(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter12\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[12],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[12]);
  return Frame_Response(count,msg);
}

int ProcessParam13(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter13\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[13],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[13]);
  return Frame_Response(count,msg);
}

int ProcessParam14(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter14\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[14],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[14]);
  return Frame_Response(count,msg);
}

int ProcessParam15(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter15\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[15],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[15]);
  return Frame_Response(count,msg);
}

int ProcessParam16(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter16\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[16],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[16]);
  return Frame_Response(count,msg);
}

int ProcessParam17(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter17\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[17],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[17]);
  return Frame_Response(count,msg);
}

int ProcessParam18(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter18\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[18],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[18]);
  return Frame_Response(count,msg);
}

int ProcessParam19(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter19\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[19],"%d",P1);
  printf("P%d -> ACDB%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[19]);
  return Frame_Response(count,msg);
}

int ProcessParam20(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter20\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[20],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[20]);
  return Frame_Response(count,msg);
}
int ProcessParam21(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter21\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[21],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[21]);
  return Frame_Response(count,msg);
}

int ProcessParam22(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter22\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[22],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[22]);
  return Frame_Response(count,msg);
}

int ProcessParam23(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter23\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[23],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[23]);
  return Frame_Response(count,msg);
}

int ProcessParam24(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter24\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[24],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[24]);
  return Frame_Response(count,msg);
}

int ProcessParam25(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter25\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[25],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[25]);
  return Frame_Response(count,msg);
}

int ProcessParam26(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter26\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[26],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[26]);
  return Frame_Response(count,msg);
}

int ProcessParam27(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter27\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[27],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[27]);
  return Frame_Response(count,msg);
}

int ProcessParam28(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter28\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[28],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[28]);
  return Frame_Response(count,msg);
}

int ProcessParam29(int count,char *msg)
{
  unsigned short int P1,P2;
  //printf("In Parameter29\n");

  P1 = get_value(msg[3],msg[4]);
  sprintf(Parameter[29],"%d",P1);
  printf("P%d -> MFM%02d:%s\n",smTrkCli.paramState-1,msg[0],Parameter[29]);
  return Frame_Response(count,msg);
}


