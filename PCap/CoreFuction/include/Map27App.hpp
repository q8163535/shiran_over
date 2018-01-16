#ifndef MAP27APP_HPP
#define MAP27APP_HPP

#include "GlobalVar.hpp"
#include <stdio.h>

//#include  <stdlib.h>
/*******中断接收UART0的数据变量*********/
//
//
//int Uart0InputLen;
//BYTE Uart0InputData[255];//="";


#define PLANITEMLENGTH1 16
#define MAXITEMNUM1 50
#define PLANLABELLENGTH1 10

typedef struct WPI_OLDPLANITEM
{
    unsigned char nItemState;
    char lpbItem[PLANITEMLENGTH1];
} PLANITEM_OLD;

typedef struct WPI_OLDPLAN
{
    unsigned char  nSerialNo;
    unsigned char  nReserved[4];
    unsigned char  nDataType;
    char  lpbPlanLabel[PLANLABELLENGTH1];
    unsigned int  nDataLength;
    unsigned int  nLocomotiveNo;
    unsigned char  nItemNum;
    unsigned char  nFirstItemFlag;
    PLANITEM_OLD lpbPlanBuffer[MAXITEMNUM1];
} PLAN_OLD;


typedef struct WPI_NEWVERPLANITEM
{
    char lpbItem[PLANITEMLENGTH+1];
    UINT nOperStatus;
} PLANITEM_NEWVER;

typedef struct WPI_PLANITEMSTATE
{
    UINT nCompleteState;
    UINT nUpLoadState;
} PLANITEMSTATE;

#if 1
typedef struct WPI_NEWVERPLAN
    {
        char   nSerialNo;
        char   nReserved;
        char   nDataType;
        char lpbPlanLabel[PLANLABELLENGTH+1];
        char lpbVersion[PLANVERSIONLENGTH+1];
        int     nDataLength;
        int    nLocomotiveNo;//火车头号码
        char   nItemNum;
        char   nFirstItemFlag;
        PLANITEM_NEWVER lpbPlanBuffer[MAXITEMNUM1];
        PLANITEMSTATE   PlanItemState[MAXITEMNUM1];
        int    nSender;
        char lpbPlanTime[MAXPLANTIME+1];
        char lpbLocation[MAXLOCATIONLENGTH+1];
        //补加
        unsigned short  nVersion;       //计划版本号
        unsigned char   nLocation;      //区域ID
} PLAN_NEWVER;

#endif

#if 0
typedef struct WPI_NEWVERPLAN
{
    unsigned char  nSerialNo;
    unsigned short nVersion;  // 2bytes
    unsigned char  nReserved;
    unsigned char  nLocation;   //接受地点
    unsigned char  nDataType;
    char  lpbPlanLabel[PLANLABELLENGTH];
    unsigned int  nDataLength;
    unsigned int  nLocomotiveNo;
    unsigned char  nItemNum;
    unsigned char  nFirstItemFlag;
    PLANITEM_NEWVER lpbPlanBuffer[MAXITEMNUM];
    PLANITEMSTATE   PlanItemState[MAXITEMNUM];
    unsigned int  nSender; // 计划的发送者
} PLAN_NEWVER;


#endif
///


// BOOL bStartFrameSend;
// BOOL g_bTrunkMapStartFlags = FALSE;
// BOOL g_bOffnetMapStartFlags = FALSE;

// UINT nRecvTimes=0;

// BOOL	g_ExitTheApp = FALSE;



//CWPIMAP27NET wpimap27net[2];
//CWPIMAP27LINK wpimap27link[2];

// InfoPlat.CPP
/////////////
BOOL SetSysID(unsigned short nSysID);
BOOL SetComPortSet(BYTE nComStyle,BYTE nComNo,unsigned short nBaud);
BOOL GetSysID(unsigned short& nSysID);
BOOL GetComPortSet(BYTE nComStyle,BYTE& nComNo,unsigned short& nBaud);
////////////
// MainFrm.CPP
//CString strSelfID = _T("");
extern int nSendType;  //0 -- free; 1 -- binary; 2 -- clearinbox;
//CString strFileType = _T("");

// unsigned short nDispatcherID;  //调度中心控制台调度员Dispatcher;



// unsigned short nPlanSender; // 计划发送者; //add 1-12-2004
//
extern  unsigned char cRecvPfix ;
extern unsigned char cRecvExPfix ;//= 0x80;
extern unsigned short nSYSID;


#if 1
const int MAXPORTNUM = 4;
typedef struct
{
    unsigned char nComStyle;
    unsigned char nComPort;
    unsigned short nComBaud;
    unsigned short nSysID;
} SYSCONFIGTABLE;

const int COMSTYLE_TRUNK =1;
const int COMSTYLE_OFFNET =2;
const int COMSTYLE_RS =3;

//
const int MAXRSRECVBUFFLEN = 100;

// Lamp Display State Define
const int GREEN_LAMP =1;
const int  RED_LAMP1 =2;
const int RED1FLASH_LAMP =3;
const int RED_LAMP2 =4;
const int GREENFLASH_LAMP =5;
const int YELLOW_LAMP =6;
const int YELLOWFLASH_LAMP =7;
//const YELLOWFLASH_LAMP1 =12;//Added Code 040519
const int RED1GREENFLASH_STOPRED1 =8;
const int RED1GREENFLASH_STOPYELLOW =9;
const int YELLOWGREENFLASH_STOPYELLOW =10;
const int BLANK_LAMP = 11;// Initinial Lamp Display
/////////////////////////////////////////////////////////////////////////////
//
const int  TABLE_SIGNALCONF = 0x01;
const  int  TABLE_SPEEDSWITCH = 0x02;
const  int  TABLE_ACHECK = 0x03;
const  int  TABLE_RECVID = 0x04;
const  int  TABLE_SYSCONFIG = 0x05;
const int   TABLE_USERCONFIG = 0x06;
//
const int   MAXWCOMPORTNUM = 2;
/////////////////////////////////////////////////////////////////////////////

const  int  RS_DEBUG_VIEW = 1;
const  int  TRUNK_DEBUG_VIEW = 2;
const int   PLAN_DEBUG_VIEW = 3;
const  int  OFFNET_DEBUG_VIEW = 4;
const int   TRUNK_MAP27 = 1;
const  int  OFFNET_MAP27 = 2;

//////
///////////////////
#if 0
const  int  COMSTATE_NOLINK = 0;
const  int  COMSTATE_OK = 1;
const  int  COMSTATE_RECVING = 2;
const int COMSTATE_SENDING = 3;
const  int  COMSTATE_OVERTIME = 3;
#endif
//-----WPITrunkPlanComm.CPP
const  int  ACKMAXTIMEOUTCOUNTER= 60; // unit 0.5s
const int   RETRYMAXTIMES = 3;
const  int  READYSETUP = 1;
const int   SETUPEND =2 ;
const  int  SETUPSUCCESS =3;
const int   SETUPFAIL =4;

//-----WPIOffnetPlanComm.CPP

const  int  ALL_LINKER = 0;  // must be zero;
// Check Ack Result
const BYTE CHKACK_OK = 1;
const BYTE CHKACK_ERROR = 0;
const BYTE CHKACK_TIMEOUT = 0xff;
const BYTE CHKACK_INIT = 0xfe;
/////////////////////////////////////////////////////////////////////////////


//NoLinkingStopState
const  int  NoLinkingStopState = 0x12;//Added Code 040426

// Test Offnet
//const  int  TEST_OFFNET = 30;

// User powerty
const int USER_SYSMANAGER = 1;
const int USER_GENERAL = 2;


// wireless communication state view style;
const int CSV_STYLE_PLAN = 0x01;
const int CSV_STYLE_CLRCROSS = 0x02;
const int CSV_RESULT_OK =0x00;
const int CSV_RESULT_TIMEOUT = 0x01;
const int CSV_RESULT_ERR = 0x02;
const int CSV_VIEW_NOEFFECT = 0xff;
const int CSV_CONFIRM = 0x00;
const int CSV_NOCONFIRM = 0x01;

// Error and  tints for Users
const int ET_UPLOAD_CLRCROSS = 1;
const int ET_DOWNLOAD_CLRCROSS =2;
const int ET_RECV_CLRCROSS = 3;
const int ET_RECV_PLANCONFIRM = 4;
const int ET_RECV_NEWPLAN = 5;


// Mobiler communication Data Style
const unsigned char M_STYLE_CONFIRM = 0x80;
const unsigned char M_STYLE_CLRCROSS = 0x81;

// the max time before we Send Down data to radio unit;
// 发送数据给车台之前的最大延时时间
const UINT TIME_MAXWAITING = 600 ; // (unit ms);
const  int   TIMER_INTERVAL_OFFNET= 60; // 60ms

// 点名状态表被初始化的方式
const int CHECKINIT_ALL = 0;
const int CHECKINIT_PART = 1;

// Communication State to general users
const  int  TRUNK_USER_VIEW = 1;
const  int  OFFNET_USER_VIEW = 2;

//
const int MAXRESENDTIMES = 3;

//
const int MAN_LTSPEED = 100;
#endif

const int MAXCHECKTYPENUM = 2;
const int CHECK_PLAN = 0;  // must be 0
const int CHECK_CLRCROSS = 1; // must be 1

typedef struct SendCheck1
{
    unsigned char nOrder;
    unsigned short nRecvID;
    unsigned char nCheckAckStatus[MAXCHECKTYPENUM];  // only used by Check (点名)
    unsigned char nCheckAckTimes[MAXCHECKTYPENUM];  // only used by Check (点名)
    char lpbOperOrder[10];
    unsigned short nMoveKate; //动车键值//Added Code 040422
    unsigned short nUnLock;//解锁键值//Added Code 040422
    int nCheckStartTimeCount;
    char strName[40];
}IDCONFIGTABLE;

const int MAXMEMBERNUM = 6;

class CMap27App
{
public:
    CMap27App();

    // SendPlan(unsigned char *pPlanDataBuffer,UINT nLength,unsigned short SendID);
    BOOL ReadCurPlanTable();

    int m_nInputDataNum;
    BYTE m_bInputData[400];//255];//,recvbuf[MAX_COMM_SIZE];
    //void OnSendPlan();
    void ShowData(int flag);
    //void SendDownPlan(unsigned char *pPlanDataBuffer,UINT nLength,unsigned short nRecvID);
    //unsigned char m_lpbSendDataBuffer[1000];

    //void CloseCommunication();
    //HANDLE InitCommunication(int nCommNo, UINT nBaudRate);




    BOOL STOP_EMERGENCY_ConcelMoveKate(unsigned int nRecvID,unsigned char nStatus);
    BOOL Stop_ConcelMoveKate(unsigned int nRecvID,unsigned char nStatus);

    //Added Code 040422
    //BOOL STOP_EMERGENCY_ConcelMoveKate(unsigned int nRecvID,unsigned char nStatus);
    BOOL SetMoveKate(unsigned int nRecvID,BYTE nStatus);
    //END

    int m_nUndoItemNum;
    int GetOffnetPlanBuff(unsigned char *pPlan,UINT &pLen);
    void NewPlanProcess( BOOL bSendDownFlag,BOOL bSysRestore);
    void TestOffnet(void);
    //BOOL GetPlanCompleteState(void) {  return ( m_bIsCompleted ); };
    //void SetPlanCompleteState(BOOL bFlag)
    //{ m_bIsCompleted = bFlag;	};
//	UINT GetDownLoadInterval(void) { return m_nDownLoadInterval; };
//	void SetDownLoadInterval(UINT nInterval) { m_nDownLoadInterval = nInterval; } ;
    BOOL GetClearCrossFlag() {
        return( m_bClearCrossStartFlag );
    };
    void Exit();
    void EmptyMobileCommState(void);
    void OnInitialUpdate(int id);
    BOOL GetCheckMess(BYTE* lpbChkMess, int& nLen);
    void SetPlanVersionNo(unsigned short nVersionNo);
    unsigned short GetPlanVersionNo(void);
    void InitChkAckTable(BYTE nCheckType,int nMode);
    void SendClrCrossProcess(void);
    void SendOffnetPlanProcess(void);
    void CheckProcess(void);
    void CheckResultView(void);
    void SetRecvClrCrossID(unsigned short nRecvID);
    unsigned short GetRecvClrCrossID(void);
    BYTE GetCheckType(void);
    void SetCheckType(BYTE nCheckType);
    BOOL SumCheckResult(unsigned short* nRecvID,int* nErrNum,BOOL* bStopChkFlag);
    void InitLinkerIDTable(void);
    void AddComState(unsigned short nMobileID,BYTE nStyle,char* pOperOrder,BYTE nState,BYTE nPlanManAckState);
    void ViewMobileCommState();
    unsigned short GetRecvOffnetPlanID(void) {
        return(m_nRecvOffnetPlanID);
    };
    void SetRecvOffnetPlanID(unsigned short nRecvID) {
        m_nRecvOffnetPlanID = nRecvID;
    };
    BOOL GetCheckFlag()
    {
        return( m_bStartCheckFlag );
    };
    void SetCheckFlag(BOOL bFlag) {
        m_bStartCheckFlag = bFlag;
    };
    void SetOffnetDownLoadToken(BOOL bFlag)
    {
        m_bOffnetDownLoadToken = bFlag;
    };
    BOOL GetOffnetDownLoadToken()
    {
        return(m_bOffnetDownLoadToken);
    };


    BOOL GetPlanCompleteState(void) {
        return ( m_bIsCompleted );
    };
    void SetPlanCompleteState(BOOL bFlag)
    {
        m_bIsCompleted = bFlag;
    };


  //  void SaveRailShuntOper(unsigned short nSendID,BYTE nStatus);
    
    char* GetOperOrder(void) {
        return(m_lpbOperOrder);
    };
    void SetOperOrder(char *lpbOperOrder);
    void SetClearCrossFlag(BOOL bFlag) {
        m_bClearCrossStartFlag = bFlag;
    };
    BOOL SearchPlanItem(UINT* nItem,UINT nOperOrder);
    //void FormatPlanHead(CStringArray *pArray);
    //void FormatPlanItem(CStringArray* pArray,UINT nItem,UINT nCompleteState,UINT nUpLoadState);
    void TempSaveCurPlan(void);
    void AddCheckID(unsigned short nRecvID);
    void SetSendOffnetPlanFlag(BOOL bFlag) {
        m_bSendOffnetPlanFlag = bFlag;
    };
    BOOL GetSendOffnetPlanFlag(void)
    {
        return(m_bSendOffnetPlanFlag);
    };

    UINT GetDownLoadInterval(void) {
        return m_nDownLoadInterval;
    };
    void SetDownLoadInterval(UINT nInterval) {
        m_nDownLoadInterval = nInterval;
    } ;

    unsigned char SearchLinker(unsigned short nID);
    unsigned short GetFirstLinkerID(void);
    unsigned short GetNextLinkerID(void);
    unsigned short GetDeployerID(void);
    void SendClearCross(unsigned short nRecvID,char *pOperOrder);
    void SendCheck(unsigned short nRecvID,BYTE nChkType,BYTE* pDataMess,int nLen);

    int init(int id);
    void SendDownOffnetPlan(unsigned char *pPlanDataBuffer,UINT nLength,unsigned short nRecvID);
    //void SetOffnetPlan(PLAN_NEWVER plan);
    BOOL ReadIDCfTable(void);
    BOOL SaveIDCfTable(void);
    //virtual ~CWPIOffnetPlanComm();
    // Generated message map functions
    void OnTimer(UINT nIDEvent);

    IDCONFIGTABLE m_IDConfigTable[MAXMEMBERNUM];
protected:
    int m_nLineWaitingTime;
    unsigned short m_nVersionNo;
    unsigned int m_nRecvClrCrossID;
    BYTE m_nCheckType;
    unsigned short m_nRecvOffnetPlanID;
    BOOL m_bOffnetDownLoadToken;
    BOOL m_bSendOffnetPlanFlag;
    //CCriticalSection m_CriOffnetPlan;
    //CIpCriticalSection m_CriOffnetPlan;

    char m_lpbOperOrder[10];

public:
    unsigned int CurrnRecvID;
    unsigned short m_nSumMoveKate;//动车键值和//Added Code 040422
    unsigned short m_nSumLinkerMoveKate;//连接员动车键值和
    unsigned short m_nCurrenStop_FaultState;//当前故障停车状态
    unsigned short m_nCommandState;//当前命令状态
    BOOL m_NoLinking_OutVoice;//车台断联后，提示报警
    unsigned short m_nNoLinkingStopState;//记录因车台断联因起的故障停车状态
    //PLAN_NEWVER  m_OffnetPlan;
    unsigned char m_lpbSendDataBuffer[1000];
    //UINT  m_nPlanDataSizeCounter;
//	CListCtrl	m_ListCtrl;
    //IDCONFIGTABLE m_IDConfigTable[MAXMEMBERNUM];
    int m_nMemberNum;
    int m_nMemberOrder;
    int m_nCheckOrder;

    unsigned short m_lpnLinkerCheckTable[100];
    unsigned short m_nCheckTimes;
    //
    // 显示脱网通讯时手持机接受计划、清勾命令的状态。//040422 修改可显示是否动车的状态
    //OFFNETCOMSTATETABLE m_ComStateTable[MAXSTATENUM];
    unsigned short m_nStateNum;
    //
    int m_nSendOffPlanTimes;
    int m_nSendClrCrossTimes;
    BOOL m_bClearCrossStartFlag;
    BOOL m_bCheckInProcessing;
    BOOL m_bStartCheckFlag;
    unsigned char m_nSerialNo;
    unsigned short m_nRecvPlanID;
    BOOL m_bNewPlanFlag;

    // 测机使用
    BOOL m_bBeginOffnetTest;
    int m_nCircleTimes;
    int m_nCircleTimes_NoLinking;//车台断联后，计数以延时提示音报警
    int m_nTestTimes;


    UINT m_nDownLoadInterval;
    long int m_nStartTime;
    BOOL m_bIsCompleted;
    //
protected:
    BOOL m_bInputHead;
};

int GetNewDiretorCount();
// extern BOOL g_bPlanFinished;
// extern BOOL g_bPlanFinishedBeep;




extern "C" void sendPlan(void);

extern "C" void LampDisplay(void);

extern "C" void setLampColor(int lampNum, DWORD color);

extern "C" int ReadIDCfCONFIG( void );


/********from C code************/
extern "C" int ComSend(int fd, const char *wbuf,int r);
extern "C" void mobileForm_insertItem();
extern "C" void mobileForm_setItem(int row, int column, char *pdata);
extern "C" void mobileForm_removeAll();
extern "C" void  setvoiceflag(int flag);
extern "C" void waveplay(void);
extern "C" int getvoiceflag(void);
extern "C" HWND hMainWnd ;
extern "C" void RestoreLampState();
extern "C" void saveMoveKateDataFile(unsigned int nRecvID,unsigned int MoveKate);
extern "C" void writeMoveKateT( unsigned int nRecvID,unsigned int MoveKate );
extern "C" void SaveRailShuntState(int SaveType,int LampState,int SenderID,int Cmd);


extern "C" int  FillPlanTable(char *lpbPlanRecvBuffer,unsigned short m_nPlanDataSizeCounter);

extern "C" void testdownplan();

extern "C" void  readtestmobile( int id );



extern "C"  void ReadMoveKateConfig(void );;



extern "C" BOOL SendPlanFlag;



#endif
