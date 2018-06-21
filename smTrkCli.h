// **********************************************************************************************
//                           s m T r k C l i . h
// **********************************************************************************************
// Project    : Smarttrak Linux Client Application for Monitoring and Controlling.
// Company    : Smarttrak Solar System pvt. ltd.
// File       : smTrkCli.h
// Description:
// Date       : 04-11-2015 
// Author     : FirmWare Team
// Compilers  : Gcc / ARM 
// **********************************************************************************************

// *******************************
// Section : Includes 
// *******************************

// *******************************
// Section : Definition 
// *******************************
#define INV_PARAM_CNT  2
#define ACBD_PARAM_CNT 4
#define MFM_PARAM_CNT  7

#define INV_MAX       10
#define ACDB_MAX      INV_MAX + 10    // 10 Parameters for ACDB
#define MFM_MAX       ACDB_MAX + 10   // 10 Parameters for MFM

#define MAJOR_VER                  3
#define MINOR_VER                  1

#define Q_SIZE                     300
#define MAX_MSG_SIZE               500  
#define MAX_SITE_NAME              100
#define MAX_DEVICES                500

#define SOCK_RESP_TMOUT_PERIOD     30
#define CONNECT_FAIL_SLEEP         30 
 
#define DEFAULT_INV_SRV_IP_ADRS    "183.82.115.149"
#define DEFAULT_SEN_SRV_IP_ADRS    "183.82.115.149"
#define DEFAULT_MFM_SRV_IP_ADRS    "183.82.115.149"
#define DEFAULT_INVS_START         1
#define DEFAULT_INVS_CNT           0
#define DEFAULT_ACDBS_START        1
#define DEFAULT_ACDBS_CNT          1
#define DEFAULT_MFMS_START         1
#define DEFAULT_MFMS_CNT           0
#define DEFAULT_INV_PORT_NUM       8111
#define DEFAULT_ACDB_PORT_NUM      8012
#define DEFAULT_MFM_PORT_NUM       8110
#define DEFAULT_SITE_NAME          "SMARTTRAK"
#define DEFAULT_SITE_ID            0001
#define DEFAULT_SER_SCAN_PERIOD    30  
#define DEFAULT_DEV_RESP_TMOUT     5
#define DEFAULT_SERIAL_DEVICE      "/dev/ttyUSB0"
#define DEFAULT_INV_BAUDRATE       57600
#define DEFAULT_ACDB_BAUDRATE      57600
#define DEFAULT_MFM_BAUDRATE       57600
#define DEFAULT_LOCAL_PORT_NUM     9000
#define DEFAULT_SRV_USER_IP_ADRS   "183.82.115.149"
#define DEFAULT_SRV_USER_PORT_NUM  8006

enum
{
  SUCCESS,
  SOCK_READ_ERR,
  SOCK_CLOSED_ERR,
  PARTIAL_MSG_RCVD,
  CLIENTS_ARRAY_OVF,
  EMPTY_CMD,
  INVALID_CMD,
};

enum
{
  SER_SCAN_INIT,
  SER_SCAN_ACTIVE,
  SER_SCAN_DONE
};

enum
{
  SOCK_NOT_CONNECTED,
  SOCK_CONNECTED
};

enum
{
  SOCK_TRANS_IDLE,
  SOCK_TRANS_RESP_WAIT
};

enum
{
  SOCK_MSG_NONE,
  SOCK_DATA_SENT,
  SOCK_PING_SENT
};



enum
{
  DEV_MSG_NONE,
  DEV_DATAREQ_SENT,
  DEV_CMD_SENT,
};

// *******************************
// Section : Global Data 
// *******************************
struct rxQue
{
  char     qBuf[Q_SIZE]; 
  int32_t  rix;          
  int32_t  wix;          
  int32_t  cnt;          
};

struct RxStream
{
  int32_t           fdNo;  
  struct rxQue      rxq;   
  char              msg[MAX_MSG_SIZE]; 
  uint32_t          mix;
  uint32_t          msgCnt;     
  uint32_t          msgLenOvf;  
  uint32_t          invMsgs;    
};

struct DevStats
{
  uint32_t  invdataReqCnt;
  uint32_t  acdbdataReqCnt;
  uint32_t  mfmdataReqCnt;

  uint32_t  invdataRespCnt;
  uint32_t  acdbdataRespCnt;
  uint32_t  mfmdataRespCnt;

  uint32_t  invdataTmoutCnt;
  uint32_t  acdbdataTmoutCnt;
  uint32_t  mfmdataTmoutCnt;
};

struct RemoteCmd
{
  int32_t  flg;
  char     msg[32];
};

struct SmartTrakClient
{
  int32_t          siteId;        
  char             siteName[64]; 
 
  char             invIpAdrs[32]; 
  char             acdbIpAdrs[32]; 
  char             mfmIpAdrs[32]; 
  int32_t	   invportnum;    
  int32_t	   acdbportnum;    
  int32_t	   mfmportnum; 
  int32_t          invCnt;   
  int32_t          invStart;  
  int32_t          acdbCnt; 
  int32_t          acdbStart;      
  int32_t          mfmCnt;       
  int32_t          mfmStart; 
 
  int32_t          invsockFd;
  int32_t          invsockState;  
  int32_t          acdbsockFd;
  int32_t          acdbsockState;
  int32_t          mfmsockFd;
  int32_t          mfmsockState;

  uint32_t         invsockConSuccessCnt; 
  uint32_t         invsockConFailCnt;    
  int32_t          invsockConErrno;
  uint32_t         invsockRdFailCnt;     
  uint32_t         invsockRespTmoutCnt; 

  uint32_t         acdbsockConSuccessCnt; 
  uint32_t         acdbsockConFailCnt;    
  int32_t          acdbsockConErrno;
  uint32_t         acdbsockRdFailCnt;     
  uint32_t         acdbsockRespTmoutCnt;

  uint32_t         mfmsockConSuccessCnt; 
  uint32_t         mfmsockConFailCnt;    
  int32_t          mfmsockConErrno;
  uint32_t         mfmsockRdFailCnt;     
  uint32_t         mfmsockRespTmoutCnt; 

  struct RxStream  invsockStrm; 
  struct RxStream  acdbsockStrm; 
  struct RxStream  mfmsockStrm;   
  struct RxStream  serStrm;    

  int32_t          invsockTransState; 
  time_t           invsockMsgSentTime;
  int32_t          acdbsockTransState; 
  time_t           acdbsockMsgSentTime;
  int32_t          mfmsockTransState; 
  time_t           mfmsockMsgSentTime;

  struct RemoteCmd rmtCmds; 

  int32_t          scanState;     
  time_t           lastScanTime;  
  int32_t          scanPeriod;  
  int32_t          tmoutPeriod;  
  int32_t          curDevIx;      
  int32_t          devState;     
  time_t           devSentTime;  

  int32_t          invFlag;
  int32_t          acdbFlag;
  int32_t          mfmFlag;

  int32_t          invfile;
  int32_t          acdbfile;
  int32_t          mfmfile;

  uint32_t         invpolling;
  uint32_t         acdbpolling;
  uint32_t         mfmpolling;
  
  uint32_t         cmdReqCnt;
  uint32_t         cmdRespCnt;
   
  struct DevStats  stats[MAX_DEVICES];

  char             ser_dev[50];
  uint32_t         inv_baud;
  uint32_t         acdb_baud;
  uint32_t         mfm_baud;
  uint32_t         localportnum;
  char             srvuserIpAdrs[32];
  uint32_t         srvuserportnum;
  char		   clifile[100];
  char		   srvfile[100];

  char             datestr[30];
  char             pidatetime[128];
  char             serverdatetime[128];
  char             dt[128];
  uint32_t         piFlag;
  uint32_t         nid;
  uint32_t         stow;       

  unsigned short int	  RegCount;
  unsigned short int      Function;
  unsigned short int      NReg;
  unsigned short int      sid;
  int32_t                 paramState;
  int32_t                 respcount; 
};

struct clientStats
{
  uint32_t invsockHeartBeats;
  uint32_t acdbsockHeartBeats;
  uint32_t mfmsockHeartBeats;

  uint32_t invserHeartBeats;
  uint32_t acdbserHeartBeats;
  uint32_t mfmserHeartBeats;

  uint32_t invsockSelCnt;
  uint32_t acdbsockSelCnt;
  uint32_t mfmsockSelCnt;

  uint32_t invserSelCnt;
  uint32_t acdbserSelCnt;
  uint32_t mfmserSelCnt;
};

extern struct SmartTrakClient smTrkCli;
extern struct clientStats     stats;
// *******************************
// Section : Modbus Polling  
// *******************************
int ProcessParam0(int respCount,char *queue);
int ProcessParam1(int respCount,char *queue);
int ProcessParam2(int respCount,char *queue);
int ProcessParam3(int respCount,char *queue);
int ProcessParam4(int respCount,char *queue);
int ProcessParam5(int respCount,char *queue);
int ProcessParam6(int respCount,char *queue);
int ProcessParam7(int respCount,char *queue);
int ProcessParam8(int respCount,char *queue);
int ProcessParam9(int respCount,char *queue);
int ProcessParam10(int respCount,char *queue);
int ProcessParam11(int respCount,char *queue);
int ProcessParam12(int respCount,char *queue);
int ProcessParam13(int respCount,char *queue);
int ProcessParam14(int respCount,char *queue);
int ProcessParam15(int respCount,char *queue);
int ProcessParam16(int respCount,char *queue);
int ProcessParam17(int respCount,char *queue);
int ProcessParam18(int respCount,char *queue);
int ProcessParam19(int respCount,char *queue);
int ProcessParam20(int respCount,char *queue);
int ProcessParam21(int respCount,char *queue);
int ProcessParam22(int respCount,char *queue);
int ProcessParam23(int respCount,char *queue);
int ProcessParam24(int respCount,char *queue);
int ProcessParam25(int respCount,char *queue);
int ProcessParam26(int respCount,char *queue);
int ProcessParam27(int respCount,char *queue);
int ProcessParam28(int respCount,char *queue);
int ProcessParam29(int respCount,char *queue);

typedef struct ReqFun
{
  char cmd[5];
  char funcode;    
  unsigned short int address;
  unsigned short int length;
  char parameter[15];
  int32_t (*Reqfun) (int ,char *);
} ReqFun_t;


// *******************************
// Section : Function Prototypes 
// *******************************
int32_t processSerMsg(char *msg);
int32_t processSockMsg(char *msg,int32_t sockfd);
int32_t serialInit(char *devname, int baudrate);
int32_t readIntoMsg(int id,struct RxStream *prs);
int32_t readIntoRxq(struct RxStream *prs);
int32_t InvfileQInit();
int32_t InvfileQCnt();
int32_t InvfileQRead(uint8_t *appdata);
int32_t InvfileQWrite(uint8_t *appdata);
int     readClientConfig(char *cfgfile);
int32_t readAndProcSockMsg(struct RxStream *pstrm);







