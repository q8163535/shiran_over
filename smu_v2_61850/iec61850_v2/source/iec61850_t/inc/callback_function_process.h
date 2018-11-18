/*************************************************
FileName:     callback_function_process.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         callback_function_process.c
History:
**************************************************/
#ifndef CALLBACK_FUNCTION_PROCESS_H
#define CALLBACK_FUNCTION_PROCESS_H

/************************************************************************
*                  头文件
*************************************************************************/
#include "protocol_operate.h"

/************************************************************************
*                  全局函数声明
*************************************************************************/
uint8_t insert_function_id ( char *fuction_name );
/************************************************************************
*                结构体声明
*************************************************************************/

#define MB_FUNC_MAX_NUM  (int32_t)100

int32_t ( *comm_func[MB_FUNC_MAX_NUM] ) ( uint16_t name,data_u old_data,pro_data_s *,int32_t indexid,uint16_t search_id );
/************************************************************************
*                  全局变量外部引用
*************************************************************************/
#define DATA_TOTEL  512


#define   CB_REC     0
#define   CB_SEND  1
#define   CB_FIFO    2

#define    MAX_CB_NUM   3





extern int32_t   IP1;
extern int32_t   IP2;
extern int32_t   IP3;
extern int32_t   IP4;

extern int32_t   SUBMASK1;
extern int32_t   SUBMASK2;
extern int32_t   SUBMASK3;
extern int32_t   SUBMASK4;

extern int32_t   DNS11;
extern int32_t   DNS12;
extern int32_t   DNS13;
extern int32_t   DNS14;

extern int32_t   DNS21;
extern int32_t   DNS22;
extern int32_t   DNS23;
extern int32_t   DNS24;

extern int32_t   GATEWAY1;
extern int32_t   GATEWAY2;
extern int32_t   GATEWAY3;
extern int32_t   GATEWAY4;




extern int32_t   IP21;
extern int32_t   IP22;
extern int32_t   IP23;
extern int32_t   IP24;

extern int32_t   SUBMASK21;
extern int32_t   SUBMASK22;
extern int32_t   SUBMASK23;
extern int32_t   SUBMASK24;

extern int32_t   DNS211;
extern int32_t   DNS212;
extern int32_t   DNS213;
extern int32_t   DNS214;

extern int32_t   DNS221;
extern int32_t   DNS222;
extern int32_t   DNS223;
extern int32_t   DNS224;

extern int32_t   GATEWAY21;
extern int32_t   GATEWAY22;
extern int32_t   GATEWAY23;
extern int32_t   GATEWAY24;
//route add default gw 172.18.24.1 eth1


#if 0
ADDRESS,  触摸屏访问逆变器的modbus地址修改
SECOND_LCD	 触摸屏 时间,
             MIN_LCD,		触摸屏 时间,
             HOUR_LCD,		触摸屏 时间,
             DATE_LCD,		触摸屏 时间,
             MONTH_LCD,	触摸屏 时间,
             YEAR_LCD,		触摸屏 时间,

             SECOND,		逆变器 时间,
             MIN,			逆变器 时间,
             HOUR	,		逆变器 时间,
             DATE,			逆变器 时间,
             MONTH,		逆变器 时间,
             YEAR,			逆变器 时间,

             FIFO_TIME_FINISH, 	管道对时完成标记,   特殊标记id 提前定义约定管道完成标记

             PC_TIMER,			触摸屏 时间,

             COM_PC_TOT,		03发送pc后台信号强度数据,

             CNT_FLAG_TOT,		03 变位 触摸屏获取时间标记,

             INV_UNIT_FAULT_DISPLAY_CONTROL,  	逆变器单元故障序号 ,

             ERR1_LCD,			触摸屏 故障信息1,
             ERR1,				逆变器 故障信息1,
             ERR2_LCD,			触摸屏 故障信息2,
             ERR2,				逆变器 故障信息2,
             ERR3_LCD,			触摸屏 故障信息3,
             ERR3,				逆变器 故障信息3,
             ALARM_INFO_LCD,	触摸屏 告警信息,
             ALARM_INFO,		逆变器告警信息

             MODBUS_S_RESTART	modbusslave启动发送至数据处理进程的信号 ,
#endif
/*
typedef enum
{

//     SECOND_LCD	,
//     MIN_LCD,
//     HOUR_LCD,
//     DATE_LCD,
//     MONTH_LCD,
//     YEAR_LCD,
//     SECOND,
//     MIN,
//     HOUR	,
//     DATE,
//     MONTH,
//     YEAR,



  //  FIFO_TIME_FINISH,

  //  PC_TIMER,

  //  COM_PC_TOT,
//      DATA_TOTEL,
  //  CNT_FLAG_TOT,

  //  INV_UNIT_FAULT_DISPLAY_CONTROL,



  //  ERR1_LCD,
  //  ERR1,
   // ERR2_LCD,
  //  ERR2,
  //  ERR3_LCD,
  //  ERR3,
  //  ALARM_INFO_LCD,
  //  ALARM_INFO,


//      IP1,
//      IP2,
//      IP3,
//      IP4,
//      SUBMASK1,
//      SUBMASK2,
//      SUBMASK3,
//      SUBMASK4,
//      DNS11,
//      DNS12,
//      DNS13,
//      DNS14,
//
//
//      DNS21,
//      DNS22,
//      DNS23,
//      DNS24,
//
//      GATEWAY1,
//      GATEWAY2,
//      GATEWAY3,
//      GATEWAY4,

    MODBUS_S_RESTART
} fuction_data_name;



extern char *func_names[];*/

uint8_t get_index_from_func_names ( char *element );

void network_infomation_init ( void );

int32_t fifo_request_signal_send ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );

#endif

