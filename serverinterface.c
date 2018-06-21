// **********************************************************************************************
//                           P r o c S o c k M s g . c
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : serverinterface.c
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
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
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

// **********************************
// Section : Static & Local Variables 
// **********************************

// *******************************
// Section : Code 
// *******************************
int ReceiveFile(char *filename)
{
  int sock,nbytes,total;
  char message[1024], server_reply[2000];
  struct sockaddr_in server;
  FILE   *fp;

  fp = fopen(filename, "w");
  if(!fp)
  {
    perror("opening text file");
    return 1;
  }

  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
  {
    printf("Could not create socket");
    return 1;
  }
   
  server.sin_addr.s_addr = inet_addr(smTrkCli.srvuserIpAdrs);
  server.sin_family = AF_INET;
  server.sin_port = htons( smTrkCli.srvuserportnum );
 
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    perror("connect failed. Error");
    return 1;
  }   
  printf("Receiving '%s' file from server\n",smTrkCli.srvfile);
  puts("Connected to server\n");
            
  sprintf(message,"put:%s:%s\n",smTrkCli.srvfile,smTrkCli.clifile);
  if( send(sock , message , strlen(message) , 0) < 0)
  {
    puts("Send failed");
    return 1;
  }
  total = 0; 
  while( (nbytes=recv(sock , server_reply , 1024 , 0)) > 0)
  {
    total += nbytes;
    fwrite(server_reply,1,nbytes,fp);
    printf("%s",server_reply);
    memset(server_reply,'\0',2000);
  }
  fclose(fp);
  printf("Total Bytes Received : %d\n",total);
  total = 0;
  close(sock);
  return 0;
}

int SendFile(char *filename,int stat)
{
  int   sock,fd,rc,addrlen;  
  off_t offset = 0;          
  struct sockaddr_in server; 
  struct stat stat_buf;     
  char message[1024]; 

  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
  {
     printf("Could not create socket");
     exit(1);
  }     
  server.sin_addr.s_addr = inet_addr(smTrkCli.srvuserIpAdrs);
  server.sin_family = AF_INET;
  server.sin_port = htons(smTrkCli.srvuserportnum);
 
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
     perror("connect failed. Error");
     return 1;
  }
  printf("Sending '%s' file to server\n",filename);
  puts("Connected to server\n");
  
  fd = open(filename, O_RDONLY);
  if (fd == -1) 
  {
    fprintf(stderr, "unable to open '%s': %s\n", filename, strerror(errno));
    return 1;
  }

  if(stat == 1)
    sprintf(message,"command:%s:%s\n",smTrkCli.srvfile,smTrkCli.clifile);
  else if(stat == 2)
    sprintf(message,"get:%s:%s\n",smTrkCli.srvfile,smTrkCli.clifile);
  if( send(sock , message , strlen(message) , 0) < 0)
  {
    puts("Send failed");
    return 1;
  }

  fstat(fd, &stat_buf);
  /* copy file using sendfile */
  offset = 0;
  printf("File Length : %d\n", (int)stat_buf.st_size);
  rc = sendfile (sock, fd, &offset, stat_buf.st_size);
  if (rc == -1)
  {
    fprintf(stderr, "error from sendfile: %s\n", strerror(errno));
    return 1;
  }
  if (rc != stat_buf.st_size) 
  {
    fprintf(stderr, "incomplete transfer from sendfile: %d of %d bytes\n", rc, (int)stat_buf.st_size);
    return 1;
  }
  printf("Total Bytes Send : %d\n", (int)stat_buf.st_size);
  close(fd);
  close(sock);
  return 0;
}



