#ifndef PLAN_SHUNTING_H
#define PLAN_SHUNTING_H 


#define CTRLPERD 35 

//add shiran
#define    IO0CLR 0
#define    IO0SET 1
#define    IO0PIN  2

//调车灯控制信号 低电平点亮 P0端口
//#define     LAMPPORTSET     IO0SET
//#define     LAMPPORTCLR     IO0CLR
//#define     LAMPSTATUSREG   IO0PIN
#define     GREENLAMP       10     //绿灯    DGRY_IN1
#define     YELLOWLAMP      11     //黄灯    DGRY_IN2
#define     REDLAMP1        15     //红灯1   DGRY_IN3
#define     REDLAMP2        16     //红灯2   DGRY_IN4


typedef union
{
    unsigned char PlaneShuntingModeCode;        //平面调车信号模式码
    struct
    {
        unsigned char PlaneShuntingCmd  :5;     //调车命令码
        unsigned char CmdSender         :2;     //调车命令的发送者
        unsigned char NewCmd            :1;     //新命令模式标识
    }bits;
}UN_PlaneShuntingModeCode;


//灯动作结构体 
//L1_CtrlPerd,控制周期
//对于常亮状态，设定L1_OnTime＝1，L1_OffTime＝#L1_CtrlPerd＋1
//对于常灭状态，设定L1_OnTime＝0，L1_OffTime＝1
//一般情况下，L1_OnTime的取值范围在1～#L1_CtrlPerd－1 ，L1_OffTime的取值范围在2～L1_CtrlPerd
typedef struct
{
    unsigned char CycNumb;      //灯控循环次数 0xff表示无限次循环
    unsigned char CtrlTimer;    //灯控时间计时器
    unsigned char OnTime;       //点灯起点时间
    unsigned char OffTimer;     //灭灯起点时间
}StLampCtrl; 



#define     greenlamp  		0
#define     yellowlamp  	1
#define     redlamp1    	2
#define     redlamp2    	3
#define     LAMPSTATUSBUFLEN    100 //可以缓冲100ms*100=10s的数据
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


//建立灯显及语音的数据结构，并进行语音播放
extern "C" void  Task2(void *pdata);

//灯显动作执行(定时执行)
extern "C" void  Task6(void *pdata);


extern  "C" int enqueue( char data);


typedef struct 
{
    unsigned char PSModeCodeValid;  //TRUE 表示记录过紧急停车命令 FALSE 表示没有记录过紧急停车命令
    unsigned char PSModeCode;       //D0~D4 紧急停车命令,D5~D6 命令发送者(D5、D6皆为1表示两个调车员都发了紧急停车命令)
    unsigned long FlashWriteTime;
}sPSModeCode,*psPSModeCode;

extern sPSModeCode sPSModeCodeR;
extern psPSModeCode psPSModeCodeF;

extern void PSModeCodeRec(unsigned char PSModeCode);

#endif
