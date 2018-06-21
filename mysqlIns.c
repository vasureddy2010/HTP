// **********************************************************************************************
//                           m y s q l I n s . c
// **********************************************************************************************
// Project    : Smarttrak Linux Server Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : mysqlIns.c
// Description:
// Date       : 12-02-2016 
// Author     : FirmWare Team
// Compilers  : Gcc / ARM 
// **********************************************************************************************

// *******************************
// Section : Includes 
// *******************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include "smTrkCli.h"

// *******************************
// Section : Global Variables 
// *******************************

// *******************************
// Section : Code 
// *******************************
void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "ErrorMsg()  : %s\n", mysql_error(con));
  mysql_close(con);
}

void set_Date_mysql()
{
  char 	qry[50],date[50],*tkns[10];
  int tkncnt;
  MYSQL *con;
  con = mysql_init(NULL);
  if(con == NULL)
  {
    fprintf(stderr, "%s\n", mysql_error(con));
    return 1;
  }
  if(mysql_real_connect(con, "192.168.0.102", "smart", "smart123", "5028_SMT", 0, NULL, 0) == NULL)
  {
    finish_with_error(con);
    return 2;
  }

  sprintf(qry,"SELECT NOW()");
  if (mysql_query(con, qry))
    {
      printf("Mysql Error : Seleceting date\n");
      finish_with_error(con);
      return 3;
    }
  MYSQL_RES *result = mysql_store_result(con);
  MYSQL_ROW row = mysql_fetch_row(result);
  strcpy(qry,row[0]);
  printf("%s",qry);
  
  sprintf(date,"sudo date -s \"%s\"",qry);
  printf("%s",date);
  system(date);
  mysql_close(con);
  return 0;
}

int ProcessQueueMsg(char *Resp)
{
  char queryMsg[500], *tkns[45], msg[500], senid[10];
  char timeStr[15],dateStr[15];
  int32_t ii, tkncnt;  
  int8_t  date,month,hour,min,sec;
  int16_t year;  
  float   Sen1,Sen2,Sen3,Sen4,Sen5,Sen6,Sen7,Sen8,Sen9,Sen10,Sen11,Sen12,Sen13,Sen14,Sen15,Sen16,Sen17,Sen18,Sen19,Sen20,Sen21,Sen22,Sen23,Sen24,Sen25,Sen26;  

  strcpy(msg,Resp);
  tkncnt=0;
  tkns[tkncnt] = strtok(msg, ":"); 

  while(tkns[tkncnt])
    tkns[++tkncnt] = strtok(NULL, ":");
 
  if(tkncnt == 33)
  { 
    sscanf(tkns[0], "%s", senid);
    if(strncmp(senid,"MFM",3) != 0)
       return -1;
    sscanf(tkns[1], "%hhd",&hour);
    sscanf(tkns[2], "%hhd",&min);
    sscanf(tkns[3], "%hhd",&sec);
    sscanf(tkns[4], "%hhd",&date);
    sscanf(tkns[5], "%hhd",&month);
    sscanf(tkns[6], "%hd", &year);
    sscanf(tkns[7], "%f",  &Sen1);  
    sscanf(tkns[8], "%f",  &Sen2);   
    sscanf(tkns[9], "%f",  &Sen3);   
    sscanf(tkns[10],"%f",  &Sen4);   
    sscanf(tkns[11],"%f",  &Sen5);   
    sscanf(tkns[12],"%f",  &Sen6);   
    sscanf(tkns[13],"%f",  &Sen7);   
    sscanf(tkns[14],"%f",  &Sen8); 
    sscanf(tkns[15],"%f",  &Sen9);   
    sscanf(tkns[16],"%f",  &Sen10);   
    sscanf(tkns[17],"%f",  &Sen11);
    sscanf(tkns[18], "%f",  &Sen12);
    sscanf(tkns[19], "%f",  &Sen13);
    sscanf(tkns[20],"%f",  &Sen14);
    sscanf(tkns[21],"%f",  &Sen15);
    sscanf(tkns[22],"%f",  &Sen16);
    sscanf(tkns[23],"%f",  &Sen17);
    sscanf(tkns[24],"%f",  &Sen18);
    sscanf(tkns[25],"%f",  &Sen19);
    sscanf(tkns[26],"%f",  &Sen20);
    sscanf(tkns[27],"%f",  &Sen21);
    sscanf(tkns[28], "%f",  &Sen22);
    sscanf(tkns[29], "%f",  &Sen23);
    sscanf(tkns[30],"%f",  &Sen24);
    sscanf(tkns[31],"%f",  &Sen25);
    sscanf(tkns[32],"%f",  &Sen26);
    /*sscanf(tkns[13],"%f",  &Sen7);
    sscanf(tkns[14],"%f",  &Sen8);
    sscanf(tkns[15],"%f",  &Sen9);
    sscanf(tkns[16],"%f",  &Sen10);
    sscanf(tkns[17],"%f",  &Sen11);*/ 
  }
  else 
  { 
    printf("INVALID NUMBER OF ARGUMENTS : %d\n",tkncnt);  
    return -1;
  } 

  MYSQL *con;
  con = mysql_init(NULL);
  if(con == NULL)
  {
    fprintf(stderr, "%s\n", mysql_error(con));
    return 1;
  }
  if(mysql_real_connect(con, "192.168.0.102", "smart", "smart123", "5028_SMT", 0, NULL, 0) == NULL)
  {
    finish_with_error(con);
    return 2;
  }
  int sid=5028;
  if(tkncnt == 33)     
  {
    sprintf(timeStr,"%02d:%02d:%02d",hour,min,sec);
    sprintf(dateStr,"%04d-%02d-%02d", year,month,date);
  
    sprintf(queryMsg, "INSERT INTO mfmdata(SITEID,MFMId,Date,Time,I1,I2,I3,AvgI,V12,V23,V31,AvgLL,V1N,V2N,V3N,AvgLN,Tot_KW,Tot_KVA,Tot_KVAR,PF1,PF2,PF3,AvgPF,Freq,FWD_KWH,REV_KWH,FWD_kVARh,REV_kVARh,FWD_kVAh,REV_kVAh,Sys_DT) VALUES(%d,'%s','%s','%s',%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,now())",sid,senid,dateStr,timeStr,Sen1,Sen2,Sen3,Sen4,Sen5,Sen6,Sen7,Sen8,Sen9,Sen10,Sen11,Sen12,Sen13,Sen14,Sen15,Sen16,Sen17,Sen18,Sen19,Sen20,Sen21,Sen22,Sen23,Sen24,Sen25,Sen26);
//    printf("%s\n",queryMsg);
     if (mysql_query(con, queryMsg))
     {
      printf("Mysql Error : In Inserting sensordata Table\n");
      finish_with_error(con);
      return 3;
     }

    sprintf(queryMsg, "UPDATE inst_mfmdata SET Date='%s',Time='%s',I1=%0.2f,I2=%0.2f,I3=%0.2f,AvgI=%0.2f,V12=%0.2f,V23=%0.2f,V31=%0.2f,AvgLL=%0.2f,V1N=%0.2f,V2N=%0.2f,V3N=%0.2f,AvgLN=%0.2f,Tot_KW=%0.2f,Tot_KVA=%0.2f,Tot_KVAR=%0.2f,PF1=%0.2f,PF2=%0.2f,PF3=%0.2f,AvgPF=%0.2f,Freq=%0.2f,FWD_KWH=%0.2f,REV_KWH=%0.2f,FWD_kVARh=%0.2f,REV_kVARh=%0.2f,FWD_kVAh=%0.2f,REV_kVAh=%0.2f,Sys_DT=now() WHERE MFMId='%s' and SITEID=%d",dateStr,timeStr,Sen1,Sen2,Sen3,Sen4,Sen5,Sen6,Sen7,Sen8,Sen9,Sen10,Sen11,Sen12,Sen13,Sen14,Sen15,Sen16,Sen17,Sen18,Sen19,Sen20,Sen21,Sen22,Sen23,Sen24,Sen25,Sen26,senid,sid);
    if (mysql_query(con, queryMsg))
    {
      printf("Mysql Error : In update inst sensordata Table\n");
      finish_with_error(con);
      return 3;
    }
    if(mysql_affected_rows(con) == 0)
    {
     sprintf(queryMsg, "INSERT INTO inst_mfmdata(SITEID,MFMId,Date,Time,I1,I2,I3,AvgI,V12,V23,V31,AvgLL,V1N,V2N,V3N,AvgLN,Tot_KW,Tot_KVA,Tot_KVAR,PF1,PF2,PF3,AvgPF,Freq,FWD_KWH,REV_KWH,FWD_kVARh,REV_kVARh,FWD_kVAh,REV_kVAh,Sys_DT) VALUES(%d,'%s','%s','%s',%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,now())",sid,senid,dateStr,timeStr,Sen1,Sen2,Sen3,Sen4,Sen5,Sen6,Sen7,Sen8,Sen9,Sen10,Sen11,Sen12,Sen13,Sen14,Sen15,Sen16,Sen17,Sen18,Sen19,Sen20,Sen21,Sen22,Sen23,Sen24,Sen25,Sen26);
     if (mysql_query(con, queryMsg))
     {
      printf("Mysql Error : In Inserting inst sensordata Table\n");
      finish_with_error(con);
      return 3;
     }
   }
  }
  
  mysql_close(con);
  return 0;
}



