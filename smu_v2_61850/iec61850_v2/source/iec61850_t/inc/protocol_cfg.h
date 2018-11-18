/*************************************************
FileName:     protocol_cfg.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         protocol_cfg.c
History:
**************************************************/
#ifndef PROTOCOL_CFG_H_
#define PROTOCOL_CFG_H_

/************************************************************************
*                  头文件
*************************************************************************/

#include "../../io.h"
#include "protocol_operate.h"


#define MAXGDATANUM (int)30

/************************************************************************
*                  全局函数声明
*************************************************************************/

/************************************************************************
*                结构体声明
*************************************************************************/


typedef enum
{
    com_pc = 1,   //接入pc串口
    com_lcd,        //接入lcd串口
    net_pc1,         //设备网口1
    net_pc2,		//设备网口2
    server_fifo,   //写管道
    client_fifo,    //读管道
} comm_type_e;

//通信连接类型
typedef enum
{
    FIFO_TRAN_SERVER = 0,     //管道转发写设备
    FIFO_TRAN_CLIENT,           //管道转发读设备
    FIFOSERVER,	                      //管道写设备
    FIFOCLIENT,	                     //管道读设备
    FIFONUM                           //管道数量

} comm_type_show;



// 转发配置通信类型
typedef enum
{
    COM_PC = 0,   //接入pc串口
    COM_LCD,      //接入lcd串口
    COM_BMS,	//接入 BMS设备
    COM_EMS,	//接入 EMS设备
    COM_NEW_DEV_TYPE_1,	//接入 UNKNOW_TYPE 1 设备
    COM_NEW_DEV_TYPE_2,	//接入 UNKNOW_TYPE 2 设备
    COM_NEW_DEV_TYPE_3,	//接入 UNKNOW_TYPE 3 设备
    COM_MAX_DEV_TYPE_NUM
} TRANS_COMM_TYPE;



/************************************************************************
*                  全局变量外部引用
*************************************************************************/

extern char comm_type_name[][100] ;
extern protocol_s protocol[];
extern comm_reg_s comm_reg;



#endif /* PROTOCOL_CFG_H_ */
