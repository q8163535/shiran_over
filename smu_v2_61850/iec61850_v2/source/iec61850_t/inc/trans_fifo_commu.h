/*************************************************
FileName:     XX.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         XX.c
History:
**************************************************/
#ifndef TRANS_FIFO_COMMU_H
#define TRANS_FIFO_COMMU_H

/************************************************************************
*                  头文件
*************************************************************************/

#include <time.h>
#include "io.h"


/************************************************************************
*                  全局函数声明
*************************************************************************/

/************************************************************************
*                结构体声明
*************************************************************************/
typedef union
{
    float32_t    value_f;     
    uint32_t     value_32;  
    uint8_t         value_bool; 
}FifoValue; /* 4 bytes */
    
typedef enum
{
    TYPE_FLOAT32,     
    TYPE_UINT32,       
    TYPE_BOOL,      
    TYPE_NUM,
}FifoValueType;

typedef struct{
    time_t  opTime;
    uint8_t opType;//on:1; off:0.
    uint8_t gotReturn;//yes:1; not yet:0.
    uint8_t returnVal;//ok:1; fail:0.
    uint8_t needCheckReturn;//yes:1, no:0.
}YK_RETURN_STATE_READ_T;

    /* ?*/
typedef struct
{
    char     dataType[4];    /* yc/yx/yk/yt */
    uint32_t  devType;       /*devType*/
    uint32_t  devNo;        /*dev Addr*/
    uint32_t  dataNum;      /*data members*/
    uint32_t  bodySize;      /* total data bytes */
    uint32_t  resv;          /* reserved */
}FifoHeader;


typedef struct
{
    uint16_t    dataID;   
    uint16_t    val;      
}YX_FIFO_DATA_T;

typedef struct
{
    uint16_t        dataID;   /* modbusAddr */      
    FifoValueType   val_type:16;  /* uint32 or float */
    FifoValue       val;      /* value */
}YC_FIFO_DATA_T;

typedef struct
{
    uint16_t    dataID;   
    uint16_t    val;      
}YK_FIFO_DATA_T;

typedef struct
{
    uint16_t        dataID;   /* modbusAddr */      
    FifoValueType   val_type:16;  /* uint32 or float */
    FifoValue       val;      /* value */
}YT_FIFO_DATA_T;


#define YC_REGISTER_QUANTITY 150

typedef struct YC_PACKET_SEND_BY_PIPE_T
{
    FifoHeader      header;
    YC_FIFO_DATA_T  data[YC_REGISTER_QUANTITY];//
}YC_PACKET_SEND_BY_PIPE_T;

#define YX_REGISTER_QUANTITY 200

typedef struct YX_PACKET_SEND_BY_PIPE_T
{
    FifoHeader      header;
    YX_FIFO_DATA_T  data[YX_REGISTER_QUANTITY];
}YX_PACKET_SEND_BY_PIPE_T;


#define YK_REGISTER_QUANTITY 5

typedef struct YK_PACKET_SEND_BY_PIPE_T
{
    FifoHeader      header;
    YK_FIFO_DATA_T  data[YK_REGISTER_QUANTITY];//
}YK_PACKET_SEND_BY_PIPE_T;


#define YT_REGISTER_QUANTITY 30

typedef struct YT_PACKET_SEND_BY_PIPE_T
{
    FifoHeader      header;
    YT_FIFO_DATA_T  data[YT_REGISTER_QUANTITY];
}YT_PACKET_SEND_BY_PIPE_T;
/************************************************************************
*                  全局变量外部引用
*************************************************************************/

 int32_t parse_yc_data_and_send_to_pipe ( int32_t tran_fifo_w_handle,uint32_t dev_index,uint32_t dev_code,int32_t data_num );
 int32_t parse_yx_data_and_send_to_pipe ( int32_t tran_fifo_w_handle,uint32_t dev_index,uint32_t dev_code,int32_t data_num );
 int32_t parse_yt_data_and_send_to_pipe ( int32_t fifo_w_handle, int32_t fifo_ptr );
 int32_t parse_yk_data_and_send_to_pipe ( int32_t fifo_w_handle, int32_t fifo_ptr );
#endif
