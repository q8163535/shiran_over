
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

extern HWND hMainWnd ;        //主窗口句柄
extern int currentUser[];    //当前主副司机

extern HWND   swd_butn_umountSD;//SD卸载


extern HWND swd_status_firstLamp;   //首架状态
extern HWND swd_status_secondLamp;  //次架状态
extern HWND swd_firstLampName; //首架名称
extern HWND swd_secondLampName; //首架名称
extern HWND swd_distance;      //距离
extern HWND swd_maxSpeed;      //限速
extern HWND swd_station;       //站场
extern HWND swd_date;          //日期
extern HWND swd_time;          //时间


extern HWND swd_taskTable;     //调度任务表
extern HWND swd_pipePress;     //制动管压
extern HWND swd_potPress;      //制动缸压
extern HWND swd_oilPress;      //润滑油压
extern HWND swd_oilTemp;       //润滑油温
extern HWND swd_waterTemp;     //冷却水温
extern HWND swd_rotSpeed;      //转速
extern HWND swd_handle;        //手柄
extern HWND swd_whistle;       //鸣笛
extern HWND swd_pushLength;    //顶送车长
extern HWND swd_calLength;     //验算车长
extern HWND swd_wuLength;      //物流车长

extern HWND swd_status_communicatGPS;  //通信状态

extern HWND swd_status_communicatGPS1;  //通信状态

extern HWND swd_status_communicatGPS2;  //通信状态

extern HWND swd_status_communicatGPS3;  //通信状态

extern HWND swd_status_communicatGPS4;  //通信状态


extern HWND swd_status_communicatDD;  //通信状态

extern HWND swd_status_communicatDD1;  //通信状态
extern HWND swd_status_communicatDD2;  //通信状态
extern HWND swd_status_communicatDD3;  //通信状态

extern HWND swd_status_communicatDD4;  //通信状态

extern GHANDLE item_planItem[];  //计划内容

extern HWND swd_dateTime;          //日期时间
extern HWND swd_trainNo;           //机车车号
extern HWND swd_classNo;           //班次
extern HWND swd_mainDriv;          //主司机
extern HWND swd_secdDriv;          //副司机
extern HWND swd_speed;             //实际速度
extern HWND swd_limitSpeed;        //限制速度

extern HWND swd_jiQunStatus;       //集群通信
extern HWND swd_changGuiStatus;    //常规通信
extern HWND swd_jianCeStatus;      //检测通信


extern HWND swd_jiQunStatustwo;       //集群通信文本框
extern HWND swd_changGuiStatustwo;    //常规通信文本框


extern HWND swd_planNo;            //计划编号
extern HWND swd_versionNo;         //版本号
extern HWND swd_hookNum;           //勾数

extern HWND swd_lamp1;             //调度提示灯1
extern HWND swd_lamp2;             //调度提示灯2
extern HWND swd_lamp3;             //调度提示灯3
extern HWND swd_lamp4;             //调度提示灯4

extern HWND swd_radioCololy ;      //集群录音条
extern HWND swd_radioCommon ;      //常规录音条
extern HWND swd_radioMainDr ;      //主司机录音条
extern HWND swd_radioSecdDr ;      //副司机录音条


extern HWND swd_butn_trainInfo;        		//机车工况
extern HWND swd_butn_mobileStatus;    		 //手机状态
extern HWND swd_butn_inputLimitSpeed; 		 //输入限速
extern HWND swd_butn_getPlan;         		 //请求计划
extern HWND swd_butn_sendPlan;        		 //下传计划
extern HWND swd_butn_printPlan;       		 //打印计划

extern HWND swd_butn_changeUser;      		 //切换用户

extern HWND swd_butn_parameterConfiguration;     //参数配置
extern HWND swd_butn_systemConfiguration;     	 //系统配置

extern HWND swd_butn_driverEnteringInfomation;     //司机录入信息

extern HWND swd_butn_wifiUpdataToServer;  //wifi 无线上传


extern HWND   swd_butn_sysExit;			//系统退出
extern HWND   swd_butn_SdAndRecord;		//SD卡及录音
extern HWND   swd_butn_down;			//下翻页
extern HWND   swd_butn_up;			//下翻页

extern HWND swd_butn_alterNum;    //修改车号


extern int  swd_radioCololyProgress ;      //集群录音条进度
extern int  swd_radioCommonProgress ;      //常规录音条进度
extern int  swd_radioMainDrProgress ;      //主司机录音条进度
extern int  swd_radioSecdDrProgress ;      //副司机录音条进度



//add for nangang
extern HWND swd_infoWindow;  //信息窗口
extern HWND swd_groupNo; //早中晚班次

extern  HWND swd_confirm_plan;

extern HWND swd_butn_finishWork;  //作业完成


extern  HWND swd_alarmWindow;     //警示标语窗口

 extern  HWND swd_butn_sysExit;     //系统退出
 extern  HWND swd_butn_finishWork;  //作业完成
 extern  HWND swd_butn_getPlan;     //请求计划
 extern  HWND swd_butn_printPlan;   //计划打印
 extern  HWND swd_butn_confirmPlan;   //确认新计划
 extern  HWND swd_butn_setTime;      //设置时间
 
 extern HWND swd_listNo;          //勾数
 
 extern  HWND swd_status_communicat;  //通信状态

#endif
