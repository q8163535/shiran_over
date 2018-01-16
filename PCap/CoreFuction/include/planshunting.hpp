#ifndef PLAN_SHUNTING_H
#define PLAN_SHUNTING_H 


#define CTRLPERD 35 

//add shiran
#define    IO0CLR 0
#define    IO0SET 1
#define    IO0PIN  2

//�����ƿ����ź� �͵�ƽ���� P0�˿�
//#define     LAMPPORTSET     IO0SET
//#define     LAMPPORTCLR     IO0CLR
//#define     LAMPSTATUSREG   IO0PIN
#define     GREENLAMP       10     //�̵�    DGRY_IN1
#define     YELLOWLAMP      11     //�Ƶ�    DGRY_IN2
#define     REDLAMP1        15     //���1   DGRY_IN3
#define     REDLAMP2        16     //���2   DGRY_IN4


typedef union
{
    unsigned char PlaneShuntingModeCode;        //ƽ������ź�ģʽ��
    struct
    {
        unsigned char PlaneShuntingCmd  :5;     //����������
        unsigned char CmdSender         :2;     //��������ķ�����
        unsigned char NewCmd            :1;     //������ģʽ��ʶ
    }bits;
}UN_PlaneShuntingModeCode;


//�ƶ����ṹ�� 
//L1_CtrlPerd,��������
//���ڳ���״̬���趨L1_OnTime��1��L1_OffTime��#L1_CtrlPerd��1
//���ڳ���״̬���趨L1_OnTime��0��L1_OffTime��1
//һ������£�L1_OnTime��ȡֵ��Χ��1��#L1_CtrlPerd��1 ��L1_OffTime��ȡֵ��Χ��2��L1_CtrlPerd
typedef struct
{
    unsigned char CycNumb;      //�ƿ�ѭ������ 0xff��ʾ���޴�ѭ��
    unsigned char CtrlTimer;    //�ƿ�ʱ���ʱ��
    unsigned char OnTime;       //������ʱ��
    unsigned char OffTimer;     //������ʱ��
}StLampCtrl; 



#define     greenlamp  		0
#define     yellowlamp  	1
#define     redlamp1    	2
#define     redlamp2    	3
#define     LAMPSTATUSBUFLEN    100 //���Ի���100ms*100=10s������
extern unsigned char LampStatusBuf[LAMPSTATUSBUFLEN];
extern unsigned short LampStatusTail;


extern const unsigned char st_tab[32][32];
extern const unsigned char es_tab[32][32];
extern const unsigned char vt_tab[32][32];
extern const unsigned char ft_tab[32][32];
extern const unsigned char ef_tab[32][32];
extern const unsigned char rp_tab[32][32];
extern unsigned char PlaneShuntingCmdProcess(unsigned char PlaneShuntingModeCode);
extern unsigned char PlayVoiceNum;
extern void StaOutProc(void);
extern void GetLampStatus(void);
extern unsigned char VoicePlayFlag;
extern unsigned char Sta_Old;


//�������Լ����������ݽṹ����������������
extern "C" void  Task2(void *pdata);

//���Զ���ִ��(��ʱִ��)
extern "C" void  Task6(void *pdata);


extern  "C" int enqueue( char data);


typedef struct 
{
    unsigned char PSModeCodeValid;  //TRUE ��ʾ��¼������ͣ������ FALSE ��ʾû�м�¼������ͣ������
    unsigned char PSModeCode;       //D0~D4 ����ͣ������,D5~D6 �������(D5��D6��Ϊ1��ʾ��������Ա�����˽���ͣ������)
    unsigned long FlashWriteTime;
}sPSModeCode,*psPSModeCode;

extern sPSModeCode sPSModeCodeR;
extern psPSModeCode psPSModeCodeF;

extern void PSModeCodeRec(unsigned char PSModeCode);

#endif
