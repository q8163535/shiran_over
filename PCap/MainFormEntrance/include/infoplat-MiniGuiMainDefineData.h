
/*************************************************
FileName:     infoplat-MiniGuiMainDefineData.h
Copyright:    GoCom
Link:         infoplat-MiniGuiMainDefineData.c
History:
**************************************************/
#ifndef _INFOPLATMINIGUIMAINDEFINEDATA_H
#define _INFOPLATMINIGUIMAINDEFINEDATA_H

#include <minigui/common.h>

#include "cppHeadInclude.h"

extern HWND hMainWnd ;        //�����ھ��
extern int currentUser[];    //��ǰ����˾��

extern HWND   swd_butn_umountSD;//SDж��


extern HWND swd_status_firstLamp;   //�׼�״̬
extern HWND swd_status_secondLamp;  //�μ�״̬
extern HWND swd_firstLampName; //�׼�����
extern HWND swd_secondLampName; //�׼�����
extern HWND swd_distance;      //����
extern HWND swd_maxSpeed;      //����
extern HWND swd_station;       //վ��
extern HWND swd_date;          //����
extern HWND swd_time;          //ʱ��


extern HWND swd_taskTable;     //���������
extern HWND swd_pipePress;     //�ƶ���ѹ
extern HWND swd_potPress;      //�ƶ���ѹ
extern HWND swd_oilPress;      //����ѹ
extern HWND swd_oilTemp;       //������
extern HWND swd_waterTemp;     //��ȴˮ��
extern HWND swd_rotSpeed;      //ת��
extern HWND swd_handle;        //�ֱ�
extern HWND swd_whistle;       //����
extern HWND swd_pushLength;    //���ͳ���
extern HWND swd_calLength;     //���㳵��
extern HWND swd_wuLength;      //��������

extern HWND swd_status_communicatGPS;  //ͨ��״̬

extern HWND swd_status_communicatGPS1;  //ͨ��״̬

extern HWND swd_status_communicatGPS2;  //ͨ��״̬

extern HWND swd_status_communicatGPS3;  //ͨ��״̬

extern HWND swd_status_communicatGPS4;  //ͨ��״̬


extern HWND swd_status_communicatDD;  //ͨ��״̬

extern HWND swd_status_communicatDD1;  //ͨ��״̬
extern HWND swd_status_communicatDD2;  //ͨ��״̬
extern HWND swd_status_communicatDD3;  //ͨ��״̬

extern HWND swd_status_communicatDD4;  //ͨ��״̬

extern GHANDLE item_planItem[];  //�ƻ�����

extern HWND swd_dateTime;          //����ʱ��
extern HWND swd_trainNo;           //��������
extern HWND swd_classNo;           //���
extern HWND swd_mainDriv;          //��˾��
extern HWND swd_secdDriv;          //��˾��
extern HWND swd_speed;             //ʵ���ٶ�
extern HWND swd_limitSpeed;        //�����ٶ�

extern HWND swd_jiQunStatus;       //��Ⱥͨ��
extern HWND swd_changGuiStatus;    //����ͨ��
extern HWND swd_jianCeStatus;      //���ͨ��


extern HWND swd_jiQunStatustwo;       //��Ⱥͨ���ı���
extern HWND swd_changGuiStatustwo;    //����ͨ���ı���


extern HWND swd_planNo;            //�ƻ����
extern HWND swd_versionNo;         //�汾��
extern HWND swd_hookNum;           //����

extern HWND swd_lamp1;             //������ʾ��1
extern HWND swd_lamp2;             //������ʾ��2
extern HWND swd_lamp3;             //������ʾ��3
extern HWND swd_lamp4;             //������ʾ��4

extern HWND swd_radioCololy ;      //��Ⱥ¼����
extern HWND swd_radioCommon ;      //����¼����
extern HWND swd_radioMainDr ;      //��˾��¼����
extern HWND swd_radioSecdDr ;      //��˾��¼����


extern HWND swd_butn_trainInfo;        		//��������
extern HWND swd_butn_mobileStatus;    		 //�ֻ�״̬
extern HWND swd_butn_inputLimitSpeed; 		 //��������
extern HWND swd_butn_getPlan;         		 //����ƻ�
extern HWND swd_butn_sendPlan;        		 //�´��ƻ�
extern HWND swd_butn_printPlan;       		 //��ӡ�ƻ�

extern HWND swd_butn_changeUser;      		 //�л��û�

extern HWND swd_butn_parameterConfiguration;     //��������
extern HWND swd_butn_systemConfiguration;     	 //ϵͳ����

extern HWND swd_butn_driverEnteringInfomation;     //˾��¼����Ϣ

extern HWND swd_butn_wifiUpdataToServer;  //wifi �����ϴ�


extern HWND   swd_butn_sysExit;			//ϵͳ�˳�
extern HWND   swd_butn_SdAndRecord;		//SD����¼��
extern HWND   swd_butn_down;			//�·�ҳ
extern HWND   swd_butn_up;			//�·�ҳ

extern HWND swd_butn_alterNum;    //�޸ĳ���


extern int  swd_radioCololyProgress ;      //��Ⱥ¼��������
extern int  swd_radioCommonProgress ;      //����¼��������
extern int  swd_radioMainDrProgress ;      //��˾��¼��������
extern int  swd_radioSecdDrProgress ;      //��˾��¼��������



//add for nangang
extern HWND swd_infoWindow;  //��Ϣ����
extern HWND swd_groupNo; //��������

extern  HWND swd_confirm_plan;

extern HWND swd_butn_finishWork;  //��ҵ���


extern  HWND swd_alarmWindow;     //��ʾ���ﴰ��

 extern  HWND swd_butn_sysExit;     //ϵͳ�˳�
 extern  HWND swd_butn_finishWork;  //��ҵ���
 extern  HWND swd_butn_getPlan;     //����ƻ�
 extern  HWND swd_butn_printPlan;   //�ƻ���ӡ
 extern  HWND swd_butn_confirmPlan;   //ȷ���¼ƻ�
 extern  HWND swd_butn_setTime;      //����ʱ��
 
 extern HWND swd_listNo;          //����
 
 extern  HWND swd_status_communicat;  //ͨ��״̬

#endif
