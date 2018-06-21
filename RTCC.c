// **********************************************************************************************
//                           R T C C . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : RTCC.c
// Description:
// Date       : 04-11-2015 
// Author     : FirmWare Team
// Compilers  : Gcc / ARM 
// **********************************************************************************************

// *******************************
// Section : Includes 
// *******************************
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include "smTrkCli.h"

// *******************************
// Section : Definitions 
// *******************************
#define I2C_ADDR 0x68

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
uint8_t BYTEtoBCD( uint8_t bByte )
{
  uint8_t msd = bByte / 10;
  uint8_t lsd = bByte - (msd * 10);

  return((msd * 16) + lsd);
}

uint8_t BCDtoBYTE(uint8_t bcd )
{
    uint8_t msn = (bcd >> 4);					

    return ((msn * 10) + (bcd & 0x0F));		
}

void WriteYearRegister(uint8_t value)
{
  int i,desc;
  //printf("yyyy : %d\n",value);
  unsigned char reg[5] = {0x06,0x05,0x04,0x02,0x01},buf[3]; 

  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }

  buf[0] = 0x06;
  buf[1] = BYTEtoBCD(value);
  write(desc,buf,2);
  close(desc);
}

void WriteMonthRegister(uint8_t value)
{
  int i,desc;
  unsigned char buf[3]; 
  
  //printf("mm : %d\n",value);
  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }

  buf[0] = 0x05;
  buf[1] = BYTEtoBCD(value);
  write(desc,buf,2);
  close(desc);
}

void WriteDateRegister(uint8_t value)
{
  int i,desc;
  unsigned char buf[3]; 
 
 // printf("dd : %d\n",value);
  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }

  buf[0] = 0x04;
  buf[1] = BYTEtoBCD(value);
  write(desc,buf,2);
  close(desc);
}

void WriteDayRegister(uint8_t value)
{
  int i,desc;
  unsigned char buf[3]; 
 
 // printf("dd : %d\n",value);
  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }

  buf[0] = 0x03;
  buf[1] = BYTEtoBCD(value);
  write(desc,buf,2);
  close(desc);
}
void WriteHourRegister(uint8_t value)
{
  int i,desc;
  unsigned char buf[3]; 
 
  // printf("hh : %d\n",value);
  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }

  buf[0] = 0x02;
  buf[1] = BYTEtoBCD(value);
  write(desc,buf,2);
  close(desc);
}

void WriteMinuteRegister(uint8_t value)
{
  int i,desc;
  unsigned char buf[3]; 
 
  // printf("mmi : %d\n",value);
  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }

  buf[0] = 0x01;
  buf[1] = BYTEtoBCD(value);
  write(desc,buf,2);
  close(desc);
}

void WriteSecondRegister(uint8_t value)
{
  int i,desc;
  unsigned char buf[3]; 

  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }

  buf[0] = 0x00;
  buf[1] = BYTEtoBCD(value);
  write(desc,buf,2);
  close(desc);
}


uint8_t ReadYearRegister()
{
  
  int i,desc;
  unsigned char buf[3],ch; 

  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }
 
  ch = 0x06;
  write(desc,&ch,1);
  if (read(desc,buf,2) != 2) 
  {
    printf("Failed to read from the i2c bus.\n");
    return -1;
  }
  else 
  {
     return BCDtoBYTE(buf[0]);
  }
  close(desc);
}

uint8_t ReadMonthRegister()
{
  int i,desc;
  unsigned char buf[3],ch; 

  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }
 
  ch = 0x05;
  write(desc,&ch,1);
  if (read(desc,buf,2) != 2) 
  {
    printf("Failed to read from the i2c bus.\n");
    return -1;
  }
  else 
  {
     return BCDtoBYTE(buf[0]);
  }
  close(desc);
}

uint8_t ReadDateRegister()
{
  int i,desc;
  unsigned char buf[3],ch; 

  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }
 
  ch = 0x04;
  write(desc,&ch,1);
  if (read(desc,buf,2) != 2) 
  {
    printf("Failed to read from the i2c bus.\n");
    return -1;
  }
  else 
  {
     return BCDtoBYTE(buf[0]);
  }
  close(desc);
}

uint8_t ReadDayRegister()
{  
  int i,desc;
  unsigned char buf[3],ch; 

  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }
 
  ch = 0x03;
  write(desc,&ch,1);
  if (read(desc,buf,2) != 2) 
  {
    printf("Failed to read from the i2c bus.\n");
    return -1;
  }
  else 
  {
     return BCDtoBYTE(buf[0]);
  }
  close(desc);
}

uint8_t ReadHourRegister()
{  
  int i,desc;
  unsigned char buf[3],ch; 

  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }
 
  ch = 0x02;
  write(desc,&ch,1);
  if (read(desc,buf,2) != 2) 
  {
    printf("Failed to read from the i2c bus.\n");
    return -1;
  }
  else 
  {
     return BCDtoBYTE(buf[0]);
  }
  close(desc);
}

uint8_t ReadMinuteRegister()
{
  int i,desc;
  unsigned char buf[3],ch; 

  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }
 
  ch = 0x01;
  write(desc,&ch,1);
  if (read(desc,buf,2) != 2) 
  {
    printf("Failed to read from the i2c bus.\n");
    return -1;
  }
  else 
  {
     return BCDtoBYTE(buf[0]);
  }
  close(desc);
}

uint8_t ReadSecondRegister()
{
  int i,desc;
  unsigned char buf[3],ch; 

  if ((desc = open("/dev/i2c-1",O_RDWR)) < 0) 
  {
    printf("Failed to open the bus.");
    exit(1);
  }

  if (ioctl(desc,I2C_SLAVE,I2C_ADDR) < 0) 
  {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }
 
  ch = 0x00;
  write(desc,&ch,1);
  if (read(desc,buf,2) != 2) 
  {
    printf("Failed to read from the i2c bus.\n");
    return -1;
  }
  else 
  {
     return BCDtoBYTE(buf[0]);
  }
  close(desc);
}







