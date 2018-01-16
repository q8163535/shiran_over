#ifndef MAP27LINK_Hpp
#define MAP27LINK_Hpp

#include "GlobalVar.hpp"
#define  TICKS	200

#define  T0		0
#define  T1		1
#define  T2		2
#define  T3		3
#define  T4		4

#define  OK			      0
#define  ERR			  -1	
//#define  MAX_COMM_SIZE    180   //ԭ��80 ,next 160 ,��Ӧ��255��
#define  MAX_WINDOW		  16   //����λ������ʱ1���복̨����ʱ16  //��ֵ��ʾ���ͼ��������ٽ���Ӧ��,����Ϊÿ�η��ͱ���Ӧ��
#define  MAX_DATA_SIZE    200  //����λ������ʱ15���복̨����ʱ��ǰΪ200
#define  CUR_VERSION	   1

#define  RESET_WAIT        0
#define  LINK_WAIT         1  
#define  DATA_READY        2 
#define  DATA_READDING     3
#define  DATA_READED       4
#define  DATA_READEDFAIL   5

#define  PLAN_DATA            0x00
#define  WULIU_DATA           0x01
#define  PLAN_TABLE_DATA      0x02

extern int WriteBuffcount,WriteBufffront,WriteBuffrear;
extern char WriteBuff[];
extern UINT WriteLen;
extern BOOL WriteFlag;
extern BOOL ReTransBeepFlag;

extern int WriteBuffcount1,WriteBufffront1,WriteBuffrear1;
extern char WriteBuff1[];




void  DelayNS(uint32  dly);
///////////////////////////
typedef struct _PLANITEM
{
	unsigned char nItemState;
	char lpbItem[PLANITEMLENGTH];
} PLANITEM;
/*
typedef struct _PLAN
{
	unsigned char  nSerialNo;     //1
	unsigned char  nReserved[4];  //4
	unsigned char  nDataType;     //1
	char  lpbPlanLabel[PLANLABELLENGTH];//PLANLABELLENGTH==10
	unsigned int  nDataLength;     //2
	unsigned int  nLocomotiveNo;   //2
	unsigned char  nItemNum;       //1
	unsigned char  nFirstItemFlag; //1
	PLANITEM lpbPlanBuffer[MAXITEMNUM];//MAXITEMNUM=30
} PLAN;*/     


// typedef struct WPI_NEWVERPLANITEM
// {
// 	char lpbItem[PLANITEMLENGTH+1];
// 	UINT nItemState;
// } PLANITEM_NEWVER;

// typedef struct WPI_PLANITEMSTATE
// {
// 	UINT nCompleteState;
// 	UINT nUpLoadState;
// } PLANITEMSTATE;
/*
typedef struct WPI_NEWVERPLAN
{
	unsigned char   nSerialNo;      //��ˮ��           1byte
	unsigned short  nVersion;       //�ƻ��汾��       2byte
	unsigned char   nReserved;
	unsigned char   nLocation;      //����ID
	unsigned char   nDataType;      //��������
	char            lpbPlanLabel[PLANLABELLENGTH+1];    //�ƻ����
	unsigned int    nDataLength;    //����data�ĳ���
	unsigned int    nLocomotiveNo;  //�������
	unsigned char   nItemNum;       //�ƻ�����
	unsigned char   nFirstItemFlag;
	PLANITEM_NEWVER lpbPlanBuffer[MAXITEMNUM];  //������
	PLANITEMSTATE   PlanItemState[MAXITEMNUM];
	unsigned int    nSender;        // �ƻ��ķ�����
} PLAN_NEWVER;*/
// extern "C" typedef struct WPI_NEWVERPLAN
// {
// 	uint8   nSerialNo;
// 	uint8   nReserved;
// 	uint8   nDataType;
// 	char lpbPlanLabel[PLANLABELLENGTH+1];
// 	char lpbVersion[PLANVERSIONLENGTH+1];
// 	uint32    nDataLength;
// 	uint32    nLocomotiveNo;
// 	uint8   nItemNum;
// 	uint8   nFirstItemFlag;
// 	PLANITEM_NEWVER lpbPlanBuffer[MAXITEMNUM];
// 	PLANITEMSTATE   PlanItemState[MAXITEMNUM];
// 	uint32    nSender;
// 	char lpbPlanTime[MAXPLANTIME+1];
// 	char lpbLocation[MAXLOCATIONLENGTH+1];
// 	//����
// 	unsigned short  nVersion;       //�ƻ��汾��
// 	unsigned char   nLocation;      //����ID
// } PLAN_NEWVER;


typedef struct WPI_WULIU
{
	unsigned char  nSerialNo; //1 ��ˮ��
	unsigned char  nReserved[4];//4
	unsigned char  nDataType; //1 ��������
	unsigned char  nFieldID[2];  //2 �����
	unsigned char  nRoadID[8];//4 �ɵ���
	unsigned int  nDataLength;//2bytes
	unsigned int  nItemNum;//2 add by jhl
	unsigned char  lpbWuliuBuffer[WULIUMAXITEMNUM][WULIUITEMLENGTH];   
} WULIU;
////////////////////////////////////


class CMap27Link  
{
public:
/////////////////////////////    
 
    BOOL  m_bPlanCommBeginFlag,m_bPlanCommEndFlag,m_bReqReSendPlan,m_bReqNewPlan;
	BOOL  m_bRecvPlanConfirm;//,m_bReqReSendPlan,m_bReqNewPlan;
/////////////////////////////
	int SendRadioParaQuest(BYTE reason);	
	void DecAppTimer();
	UINT GetAppTimer();
	BOOL GetAppTimerEnable();
	UINT GetTimerVal(BYTE type);
	void DecTimerVal(BYTE type);
	BOOL GetTimerEnable(BYTE type);
	void InitLink(int flag);
	BYTE GetStatus();
	void SendTextData();
	Map27_CallAttribute GetCallAttri();
	void RecvMessProc(BYTE *buf, int len);
	int RadioHintTone(BYTE type, BYTE way);
	void RecvSSTProc(BYTE *buf1, BYTE len, BYTE nlb, unsigned short nCallingID);
	//void RecvStatusProc(BYTE nStatus, unsigned short nSendID);
	void SetMAPStatus(BYTE st);
	BYTE GetMAPStatus();
	void SetStatus(BYTE st);
	int SendCheck(Map27_MobileID id, BYTE *buf, UINT len, BYTE nChkType);
	int DisconnectCall();
	int SendModemData(BYTE *buf, UINT len, BYTE nextPack, BYTE firstPack);
	int OutVoiceModem(Map27_MobileID id, Map27_CallAttribute details, BYTE calltype);
	void SendLINKMsg(BYTE *sbuf, UINT l);
	int SendSST(Map27_MobileID id, BYTE *buf, UINT len, BYTE code, BYTE nlb);
	int SendStatus(Map27_MobileID id, BYTE status);
	unsigned short RevertWORD(unsigned short ww);
	UINT GetMPCommNo();
	void SetMPCommNo(UINT no);
	void SetMPSRk(BYTE srk);
	void SetMPSNR(BYTE snr);
	UINT GetT2TimerVal(BYTE len,UINT baud);
	void StoreReceiveCredit();
	void UpdateReceiveCredit();
	void DecRecvCredit();
	void IncRecvVariable();
	BOOL PacketOutofSequence(BYTE ns);
	BOOL PacketOutsideWindow(BYTE ns);
	void MaxLinkPara();
	BYTE GetMPC1();
	void DecRetransCount();
	BOOL RepeatedLinkACK(BYTE nr);
	void StoreACKed(BYTE nr);
	void SetRetransCounter();
	//void WriteComm(BYTE *buf,int len);
	void WriteComm(int flag);
	BYTE GetMPSNR();
	BOOL ACKOutsideWindow(BYTE nr);
	void SetMAXNum(UINT num);
	BYTE GetMPSk();
	void SetMPSk(BYTE sk);
	void RecordSendCredit(BYTE nk);
	void InitVariable();
	BOOL ReceivedParaAcceptable(BYTE k,BYTE n1,BYTE cv);
	void AdjustLinkPara(BYTE k,BYTE n1,BYTE cv);
	void StopTimer(BYTE type);
	BYTE GetMPRk();
	BYTE GetMPVR();
	BYTE GetMPCV();
	BYTE GetMPN1();
	BYTE GetMPk();
	void RecvLink(BYTE *buf,int len);
	CMap27Link();
	CMap27Link(int comN);
	void StartTimer(BYTE type);
	UINT GetMPBaud();
	void SetMPTimer(BYTE type,UINT tm);
	UINT GetT1TimerVal(BYTE len,UINT baud);
	void IncSendVariable();
	BYTE GetMPVS();
	int CheckSumIsOK(BYTE *buf,int len,unsigned short fcs);
	UINT BuildLAMessage(BYTE *buf,BYTE nr,BYTE nk);
	void OutputLA(BYTE nr,BYTE nk);
	UINT BuildLRMessage(BYTE *buf,BYTE k,BYTE n1,BYTE cv);
	void OutputLR(BYTE k,BYTE n1,BYTE cv);
	unsigned short CheckSumCalc(BYTE *buff,UINT len);
	UINT BuildeDLE(BYTE *inbuf,BYTE *outbuf,int len);
	UINT BuildLTMessage(BYTE *buf,int len,BYTE ns,BYTE ar);
	void OutputLT(BYTE *buf,int len,BYTE ns,BYTE ar);
	void StatusProc(StructStatusToAPP* statusMsg);
	int ReportToApp(BYTE type,BYTE para,BYTE pa1,BYTE pa2);
	void RecvNETMsg(BYTE *rbuf, UINT rLen);
	void ResendDataTimer();
	
	
	CMap27Link(BYTE n2,BYTE n3,BYTE st,UINT baud);	
	virtual ~CMap27Link();
    int gOffnetCommState;
    
/////////////////////////////////�Լ����///////////////
   void RecvModemDataProc(BYTE *buf, BYTE len);
  // Map27_CallAttribute GetCallAttri();
   void SetCallAttri(Map27_CallAttribute attri);
   BYTE GetCallType();
   void SetCallType(BYTE calltype);

	void SetCalleeID(Map27_MobileID id);
	void SetCallerID(Map27_MobileID id);
	Map27_MobileID GetSelfID();
	void SetSelfID(Map27_MobileID id);
	//--------PlanComm.h-----------
	void ResetRecvPlanBuff(void);
	void PackToRecvPlanBuff(unsigned char* buff,UINT uLen);
	UINT GetLostPackNum(void);
	void SetLostPackNum(UINT uLostNum);
	UINT m_uLostPackNum;
	BOOL GetPlanCommEndFlag(void);
	BOOL GetPlanCommBeginFlag(void);
	void SetPlanCommEndFlag(BOOL bFlag);
	void SetPlanCommBeginFlag(BOOL bFlag);
	void SetupRequest();
	void ReqNewPlan(void);
	void ReqReSendPlan(char* lpbPlanLabel);
	int DeleteItem(void);
	void RefreshPlanItem();
	int InsertBlankItem(unsigned char nPos,unsigned char nOriginalItemNum);
	void RecvPlanConfirm();
////	bool FillPlanTable(void);
////	void ViewRecvData(void);
	void Transmit(void);
	
	void SetRecvPlanConfirmFlag(BOOL bFlag);
	BOOL GetRecvPlanConfirmFlag();
	//void SetReqReSendPlanFlag(BOOL bFlag);
		void SetReqReSendPlanFlag(BOOL bFlag) { 	m_bReqReSendPlan  = bFlag; };
	BOOL GetReqReSendPlanFlag();
	//void SetReqNewPlanFlag(BOOL bFlag);
		void SetReqNewPlanFlag(BOOL bFlag) { 	m_bReqNewPlan  = bFlag; };
	BOOL GetReqNewPlanFlag(){
	 return  m_bReqNewPlan;
	};
	BOOL IsRecvNewPlan();
////////////////////////////////////////////////////////////
// ��ʾ�ƻ�����Ӧ��״̬�֣�00--��ȷ��01--����02--��ǰ�ƻ�û����ɣ������ܣ�
	BYTE m_nPlanRecvState;
	unsigned char m_nSerialNo;
/////////////////////////////////////////////////////////

	BYTE gMP_C1;
	BYTE gMP_N2;
	BYTE gMP_N3;
	
	BYTE gMP_N1;		//Э�̺ú�Ĳ���
	BYTE gMP_k;			//Э�̺ú�Ĳ���
	BYTE gMP_CV;		//Э�̺ú�Ĳ���

	BYTE gMP_Rk;			//variable of nk
	BYTE gMP_Sk;

	BYTE gMP_SNR;
	BYTE gMP_SRk;

	BYTE gMP_VS;
	BYTE gMP_VR;

	BOOL gMP_TEnable[5];
	
	UINT gMP_TTimer[5];
	UINT gMP_T[5];
	
	UINT gMP_LinkState;

	UINT gMP_BaudRate;
	UINT gMP_CommNo;

	UINT max_num;   	//��Ϣ����������������ݰ���
};

extern BYTE CurDataType;
void itoa(short);

#endif
