/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :queue.c
* Author :shiran
* Date :2015-12-08
* Description :
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2016-04-18: 版本：V 0.1 作者: shiran
* Modification :
*------------------------------------------------------------------------------
******************************************************************************/





/************************************************************************
*                   头文件
*************************************************************************/
#include "queue.h"

/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/
////////////// 接收队列处理 ///////////////////////////////////

/************************************************************************
*                   全局变量
*************************************************************************/


/************************************************************************
*                   本地变量（即静态全局变量）
*************************************************************************/


/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/


/************************************************************************
*                   全局函数
*************************************************************************/


// #define COMRXBUFMAXN (int32_t)10
// #define COMRXBUFMAXLEN (int32_t)1024+1024

#define MAXCOMRXLEN (int32_t)1024

int32_t comRxBufNum;
comRxBuffer comRxGloabl[COMRXBUFMAXN];//= {0};

/*******************************************************************************
 * Function       : PutRxBufData
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  :上传数据至全局数据缓冲队列
 * Calls           :
 * Input          :uint8_t *buf,int32_t len,int32_t RxNum
 * Output       :
 * Return        :
 ********************************************************************************/
void PutRxBufData ( uint8_t *buf,int32_t len,int32_t RxNum )
{
    int32_t i;
    //  uint32_t tRxBufHead =  comRxGloabl[RxNum].RxBufHead;
    uint32_t tRxBufTail    =  comRxGloabl[RxNum].RxBufTail;

    for ( i=0; i<len; i++ )
    {
        comRxGloabl[RxNum].RxBuf[tRxBufTail]=buf[i];
        tRxBufTail++;

        if ( tRxBufTail > MAXCOMRXLEN )
        {
            InitClearRxBufData ( RxNum );
            return;
        }
        comRxGloabl[RxNum].RxBufTail =   tRxBufTail;
        //   RxBufTail%=MAXRXBUFLEN;
    }
}



/*******************************************************************************
 * Function       : GetRxBufData
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  :从全局数据缓冲队列 获取数据
 * Calls           :
 * Input          :uint8_t *buf,int32_t RxNum 队列序号
 * Output       :
 * Return        :
 ********************************************************************************/
void GetRxBufData ( uint8_t *buf,int32_t RxNum )
{
    int32_t i = 0;
//     int32_t len;
    uint32_t tRxBufHead  =   comRxGloabl[RxNum].RxBufHead;
    uint32_t tRxBufTail     =   comRxGloabl[RxNum].RxBufTail;

//     len =  tRxBufTail - tRxBufHead;
    for ( i=tRxBufHead; i<tRxBufTail; i++ )
    {
        buf[i] = comRxGloabl[RxNum].RxBuf[i];
        //  tRxBufHead++;
    }
}



/*******************************************************************************
 * Function       : GetRxBufDataLen
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  :获取全局数据缓冲队列 长度
 * Calls           :
 * Input          :,int32_t RxNum 队列序号
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t GetRxBufDataLen ( int32_t RxNum )
{

    int32_t len = 0;
    uint32_t tRxBufHead  =   comRxGloabl[RxNum].RxBufHead;
    uint32_t tRxBufTail     =   comRxGloabl[RxNum].RxBufTail;

    len =  tRxBufTail - tRxBufHead;
    return len;
}


/*******************************************************************************
 * Function       : InitClearRxBufData
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  :清空全局数据缓冲队列长度
 * Calls           :
 * Input          :,int32_t RxNum 队列序号
 * Output       :
 * Return        :
 ********************************************************************************/
void InitClearRxBufData ( int32_t RxNum )
{
    comRxGloabl[RxNum].RxBufHead = 0;
    comRxGloabl[RxNum].RxBufTail = 0;

}
// ///////////////////////////////////////////

/************************************************************************
*                   局部函数
*************************************************************************/
