
/*************************************************
FileName:     init.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         init.c
History:
**************************************************/
#ifndef _INIT_H
#define _INIT_H

/************************************************************************
*                  头文件
*************************************************************************/
#include "../../io.h"

/************************************************************************
*                  全局函数声明
*************************************************************************/

void sys_device_info_init ( void );

/************************************************************************
*                结构体声明
*************************************************************************/

/************************************************************************
*                  全局变量外部引用
*************************************************************************/
extern int32_t server_fifo_write_fd;         //写管道
extern int32_t client_fifo_read_fd;  //读管道

extern int32_t server_fifo_tran_write_fd;         //写管道
extern int32_t client_fifo_tran_read_fd;  //读管道

extern int32_t efd;                           //epoll全局句柄变量


#define soft_ver "1.0.0.0" //2018-11-16

#define MAXCOMMOBJN (int32_t)10
#define   CONFIGFILEPATH        (char *)"./Config/ModbusSlaveConfig.xml"


#define eth0    (int32_t)0
#define eth1    (int32_t)1

#endif



