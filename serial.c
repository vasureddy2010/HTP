// **********************************************************************************************
//                           s e r i a l . c 
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : serial.c
// Description:
// Date       : 04-11-2015 
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


// *******************************
// Section : Code 
// *******************************
int32_t serialInit(char *devname, int baudrate)
{
  int32_t        sfd;
  struct termios tio;

  //return 1;
  sfd = open(devname, O_RDWR | O_NOCTTY);
  if(sfd<0)
  {
     sfd = open("/dev/ttyUSB1",O_RDWR | O_NOCTTY);
     if(sfd<0)
     {
       sfd = open("/dev/ttyUSB2",O_RDWR | O_NOCTTY);
       if(sfd<0)
       {
 	 sfd = open("/dev/ttyUSB3",O_RDWR | O_NOCTTY);
         if(sfd<0)
	    return sfd;
       }
     }
  }
  memset( &tio,0,sizeof(tio) );
  tcgetattr(sfd,&tio);

  switch(baudrate)
  {
    case 9600:
      tio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
      break;
    case 19200:
      tio.c_cflag = B19200 | CS8 | CLOCAL | CREAD;
      break;
    case 38400:
      tio.c_cflag = B38400 | CS8 | CLOCAL | CREAD;
      break;
    case 57600:
      tio.c_cflag = B57600 | CS8 | CLOCAL | CREAD;
      break;
    case 115200:
      tio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
      break;
    default:
      printf("Invalid baudrate %d, default baudrate 115200 used\n",baudrate);
      tio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  }
  tio.c_iflag = 0;
  tio.c_oflag = 0;
  tio.c_lflag = 0;
  tio.c_cc[VTIME] = 0;
  tio.c_cc[VMIN]  = 0;

  tcflush(sfd,TCIFLUSH);
  tcsetattr(sfd,TCSANOW,&tio);
  return sfd;
}


