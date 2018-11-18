/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :XX.c
* Author :shiran
* Date :2015-12-08
* Description :
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2015-12-08: 版本：V 0.1 作者: shiran
* Modification :
*------------------------------------------------------------------------------
******************************************************************************/


/************************************************************************
*                   头文件
*************************************************************************/

#include "trans_fifo_commu.h"
#include "debugSwitch.h"
#include "protocol_operate.h"
/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/
#define DEV_TYPE 2827
#define MAX_DEV_ADDR 3


#define SERIAL_PORT_READ_BUF_LENGTH 256
#define PIPE_READ_BUF_LENGTH        512


/************************************************************************
*                   全局变量
*************************************************************************/


/************************************************************************
*                   本地变量（即静态全局变量）
*************************************************************************/
static uint8_t g_serialPortReadBuf[SERIAL_PORT_READ_BUF_LENGTH] = {0};

static uint8_t g_pipeReadBuf[PIPE_READ_BUF_LENGTH] = {0};

static YX_PACKET_SEND_BY_PIPE_T g_yxPacketSendByPipe ;

static YC_PACKET_SEND_BY_PIPE_T g_ycPacketSendByPipe ;




static YK_PACKET_SEND_BY_PIPE_T g_ykPacketSendByPipe ;

static YT_PACKET_SEND_BY_PIPE_T g_ytPacketSendByPipe ;

#define DATA_TOTEL 512

dev_data_t fifo_data[DATA_TOTEL] ;

/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/


/************************************************************************
*                   全局函数
*************************************************************************/



/************************************************************************
*                   局部函数
*************************************************************************/
int32_t data_yx_rev_n  = 0;
int32_t data_yk_rev_n  = 0;
int32_t data_yt_rev_n  = 0;
int32_t data_yc_rev_n  = 0;


int32_t  tran_fifo_parse_yc_data ( int32_t to_tran_addr,uint16_t data_type, FifoValue data_value ,int16_t data_yx_rev_id )
{
    int32_t data_id  =  0;
    YC_FIFO_DATA_T * dataPointer = & ( g_ycPacketSendByPipe.data[data_yx_rev_id] );
    dataPointer[data_id].dataID       = to_tran_addr;//

    if ( data_type == TYPE_UINT32 )
    {
        dataPointer[data_id].val.value_32  =data_value.value_32;
    }
    else
    {
        dataPointer[data_id].val.value_f  =data_value.value_f;
    }
    dataPointer[data_id].val_type     = data_type;
    data_yx_rev_id += 1 ;

    return data_yx_rev_id;
}



int32_t  tran_fifo_parse_yx_data ( int32_t to_tran_addr,uint16_t data_type, uint16_t data_value ,int16_t data_yx_rev_id )
{
    int32_t data_id  =  0;
    YX_FIFO_DATA_T * dataPointer = & ( g_yxPacketSendByPipe.data[data_yx_rev_id] ) ;

    dataPointer[data_id].dataID       = to_tran_addr;//

    dataPointer[data_id].val  =data_value;

    data_yx_rev_id += 1 ;

    return data_yx_rev_id;
}



/*61850*/
int32_t parse_yc_data_and_send_to_pipe ( int32_t tran_fifo_w_handle,uint32_t dev_index,uint32_t dev_code,int32_t data_num )
{


    int totalSize = 0;
    int ret = 0;

    FifoHeader*     headPointer = &g_ycPacketSendByPipe.header;
    headPointer->dataType[0] = 'y';
    headPointer->dataType[1] = 'c';
    headPointer->dataType[2] = '\0';
    headPointer->devType = dev_code;
    headPointer->devNo = dev_index;
    headPointer->resv = 0;



    headPointer->dataNum = data_num;
    headPointer->bodySize= headPointer->dataNum * sizeof ( YC_FIFO_DATA_T ); //data bytes
    totalSize = headPointer->bodySize + sizeof ( FifoHeader );


    if ( totalSize > sizeof ( FifoHeader ) )
    {
        ret = write ( tran_fifo_w_handle, &g_ycPacketSendByPipe, totalSize );

        if ( totalSize != ret )
        {
            DEBUG_FAULT ( "ERROR: Failed to write %d bytes yc to pipe  \n",  totalSize );
        }
    }

    if ( debug_module.debug_set > 0 )
    {
        DEBUG_FIFO_SEND_INFO ( "\n------- parse_yc_data_and_send_to_pipe--------\n" );

        int32_t  send_id = 0;
        for ( send_id  = 0  ; send_id < g_ycPacketSendByPipe.header.dataNum ; send_id ++ )
        {

            if ( g_ycPacketSendByPipe.data[ send_id ].val_type == TYPE_FLOAT32 )
            {
                DEBUG_FIFO_SEND_INFO ( "data_addr:%d  value_f: %f  \n",g_ycPacketSendByPipe.data[ send_id ].dataID,g_ycPacketSendByPipe.data[ send_id ].val.value_f );
            }
            else
            {
                DEBUG_FIFO_SEND_INFO ( "data_addr:%d  value_u: 0x%0x  \n",g_ycPacketSendByPipe.data[ send_id ].dataID,g_ycPacketSendByPipe.data[ send_id ].val.value_32 );
            }

        }
        DEBUG_FIFO_SEND_INFO ( "\n" );
    }



    return 0;

}



//----------------------------------------------------------------------------------------------------------------------------
int32_t parse_yx_data_and_send_to_pipe ( int32_t tran_fifo_w_handle,uint32_t dev_index,uint32_t dev_code,int32_t data_num )
{
//     YX_FIFO_DATA_T* dataPointer = g_yxPacketSendByPipe.data;


    int32_t ret  = 0;

    g_yxPacketSendByPipe.header.dataType[0] = 'y';
    g_yxPacketSendByPipe.header.dataType[1] = 'x';
    g_yxPacketSendByPipe.header.dataType[2] = '\0';
    g_yxPacketSendByPipe.header.devType = dev_code; //500K
    g_yxPacketSendByPipe.header.devNo = dev_index;
    g_yxPacketSendByPipe.header.resv = 0;


    g_yxPacketSendByPipe.header.dataNum  = data_num;
    g_yxPacketSendByPipe.header.bodySize = g_yxPacketSendByPipe.header.dataNum * sizeof ( YX_FIFO_DATA_T );

    int totalSize = g_yxPacketSendByPipe.header.bodySize + sizeof ( FifoHeader );

    if ( totalSize > sizeof ( FifoHeader ) )
    {
        ret = write ( tran_fifo_w_handle, &g_yxPacketSendByPipe, totalSize );

        if ( totalSize != ret )
        {
            DEBUG_FAULT ( "ERROR: Failed to write %d bytes yx to pipe\ n",  totalSize );
        }
    }

    if ( debug_module.debug_set > 0 )
    {
        DEBUG_FIFO_SEND_INFO ( "\n------- parse_yx_data_and_send_to_pipe--------\n" );
        int32_t  send_id = 0;
        for ( send_id  = 0  ; send_id < g_yxPacketSendByPipe.header.dataNum ; send_id ++ )
        {
            DEBUG_FIFO_SEND_INFO ( "data_addr:%d  value_u: 0x%0x  \n",g_yxPacketSendByPipe.data[ send_id ].dataID,g_yxPacketSendByPipe.data[ send_id ].val );

        }
        DEBUG_FIFO_SEND_INFO ( "\n" );
    }


    return 0;
}






#include "protocol_operate.h"

int32_t parse_yt_data_and_send_to_pipe ( int32_t fifo_w_handle, int32_t fifo_ptr )
{


    if ( fifo_ptr )
    {
        uint16_t count = 0;

        count = write ( fifo_w_handle, ( uint8_t * ) &fifo_data, sizeof ( dev_data_t ) *fifo_ptr );
        if ( count == -1 )
        {
            fprintf ( stderr," write ---server_fifo_write_fd ERROR \n" );
            DEBUG_FAULT ( "\n  rev_cmd_pro 06 10 --- write ---server_fifo_write_fd ERROR  \n" );
        }
    }

//--------------------------------------------------------------------------2018-10-12 管道发送数据打印-----------------------------------------------------------------------------------------------
    if ( debug_module.debug_set  > 0 )
    {
        int32_t tmp_id = 0 ;
        DEBUG_FIFO_SEND_INFO ( "\n 管道发送数据: \n" );
        for ( tmp_id = 0 ; tmp_id < fifo_ptr ; tmp_id ++ )
        {
            DEBUG_FIFO_SEND_INFO ( "\n 0-F 1-S 2-U type %d data.f32 %f  u32:0x%0x  ",fifo_data[tmp_id].data_type,  fifo_data[tmp_id].data.f32, fifo_data[tmp_id].data.u32 );

            DEBUG_FIFO_SEND_INFO ( "data_id %d   dev_code  %d  \n",fifo_data[tmp_id].data_id ,fifo_data[tmp_id].dev_code );
        }

    }
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    return 0;
}


int32_t parse_yk_data_and_send_to_pipe ( int32_t fifo_w_handle, int32_t fifo_ptr )
{


    if ( fifo_ptr )
    {
        uint16_t count = 0;

        count = write ( fifo_w_handle, ( uint8_t * ) &fifo_data, sizeof ( dev_data_t ) *fifo_ptr );
        if ( count == -1 )
        {
            fprintf ( stderr," write ---server_fifo_write_fd ERROR \n" );
            DEBUG_FAULT ( "\n  rev_cmd_pro 06 10 --- write ---server_fifo_write_fd ERROR  \n" );
        }
    }

//--------------------------------------------------------------------------2018-10-12 管道发送数据打印-----------------------------------------------------------------------------------------------
        if ( debug_module.debug_set  > 0 )
        {
            int32_t tmp_id = 0 ;
            DEBUG_FIFO_SEND_INFO ( "\n 管道发送数据: \n" );
            for ( tmp_id = 0 ; tmp_id < fifo_ptr ; tmp_id ++ )
            {
                DEBUG_FIFO_SEND_INFO ( "\n 0-F 1-S 2-U type %d data.f32 %f  u32:0x%0x  ",fifo_data[tmp_id].data_type,  fifo_data[tmp_id].data.f32, fifo_data[tmp_id].data.u32 );

                DEBUG_FIFO_SEND_INFO ( "data_id %d   dev_code  %d  \n",fifo_data[tmp_id].data_id ,fifo_data[tmp_id].dev_code );
            }

        }
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


        return 0;
    }
