/*************************************************
FileName:     XX.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         XX.c
History:
**************************************************/
#ifndef QUEUE_H
#define QUEUE_H

/************************************************************************
*                  头文件
*************************************************************************/
#include "../../io.h"

/************************************************************************
*                  全局函数声明
*************************************************************************/
void InitClearRxBufData ( int RxNum );


void PutRxBufData ( uint8_t *buf,int32_t len,int32_t RxNum );

void GetRxBufData ( uint8_t *buf,int32_t RxNum );

int32_t GetRxBufDataLen ( int32_t RxNum );
/************************************************************************
*                结构体声明
*************************************************************************/
#define COMRXBUFMAXN (int32_t)10       ////串口接收缓冲区个数
#define COMRXBUFMAXLEN (int32_t)1024+1024     //串口接收队列缓冲区大小

typedef struct COMRXBUFFER
{

    uint32_t RxBufHead;
    uint32_t RxBufTail;
    uint8_t RxBuf[COMRXBUFMAXLEN] ;
} comRxBuffer;
/************************************************************************
*                  全局变量外部引用
*************************************************************************/

extern comRxBuffer comRxBuf[ ];
extern int32_t comRxBufNum;

#endif
