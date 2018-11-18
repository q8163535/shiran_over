/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :init.c
* Author :shiran
* Date :2016-02-03
* Description : 设备初始化函数
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2016-02-03: 版本：V 0.1 作者: shiran
* Modification :
*------------------------------------------------------------------------------
******************************************************************************/




/************************************************************************
*                   头文件
*************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
// #include <sys/types.h>
#include <sys/stat.h>


#include "init.h"

#include "protocol_operate.h"
#include "protocol_cfg.h"

#include "xml_csv.h"
#include "debugSwitch.h"

#include "io.h"

#include "callback_function_process.h"

#include "debug_config.h"

/************************************************************************
*                    宏定义-常量定义
*************************************************************************/

/************************************************************************
*                   全局变量
*************************************************************************/
int32_t server_fifo_write_fd;  //写管道
int32_t client_fifo_read_fd; //读管道
int32_t server_fifo_tran_write_fd;  //写管道
int32_t client_fifo_tran_read_fd; //读管道

/************************************************************************
*                   本地变量（即静态全局变量）
*************************************************************************/


/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/



/************************************************************************
*                   全局函数实现
*************************************************************************/




/*******************************************************************************
 * Function       : creat_fifo
 * Author          : NULL
 * Date              : 2016.05.17
 * Description  : 创建管道
 * Calls              :
 * Input             :  const char *path,int mode
 * Output          :
 * Return           :成功返回管道fd  失败    exit  进程
 ********************************************************************************/
int creat_fifo ( const char *path,int mode )
{
    int fd = 0;
    if ( access ( path, F_OK ) == -1 )
    {
        mkfifo ( path, 0777 );
    }

    fd = open ( path, mode );
    if ( fd == -1 )
    {

        fprintf ( stderr, path );
        fprintf ( stderr, "\n" );
        exit ( EXIT_FAILURE );
    }

    return fd;

}





/*******************************************************************************
 * Function       : fifo_init
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : 管道初始化
 * Calls              :creat_fifo 创建fifo管道
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/

#define IEC104_TRAN_R_FIFO  "/tmp/fifo_ctrl"   //fifo_ctrl ykYtPipe
#define IEC104_TRAN_W_FIFO   "/tmp/fifo_data"  //fifo_data yxYcPipe


void fifo_init ( void ) //管道初始化
{

    int32_t   conn_id = 0;
    printf ( "--------------fifo_init ------\n" );

    server_fifo_write_fd = creat_fifo ( SERVER_FIFO_NAME,O_WRONLY );

    client_fifo_read_fd = creat_fifo ( IEC61850_W_FIFO,O_RDONLY | O_NONBLOCK );

    server_fifo_tran_write_fd = creat_fifo ( IEC104_TRAN_W_FIFO,O_WRONLY |O_NONBLOCK| O_APPEND );

    client_fifo_tran_read_fd = creat_fifo ( IEC104_TRAN_R_FIFO,O_RDONLY | O_NONBLOCK );
    
   

    for ( conn_id = 0 ;  conn_id < FIFONUM; conn_id ++ )
    {

        if ( p_g_com_conn[conn_id ].cmm_type== FIFOSERVER )
        {
            DEBUG_MESSAGE_PRINT_INFO ( " FIFOSERVER fd %d \n",server_fifo_write_fd );

            p_g_com_conn[conn_id].comm_obj_fd =  server_fifo_write_fd;

        }
        else    if ( p_g_com_conn[conn_id ].cmm_type== FIFOCLIENT )
        {
            DEBUG_MESSAGE_PRINT_INFO ( " FIFOCLIENT fd %d \n",client_fifo_read_fd );
            p_g_com_conn[conn_id].comm_obj_fd =  client_fifo_read_fd;

        }
        if ( p_g_com_conn[conn_id ].cmm_type== FIFO_TRAN_SERVER )
        {
            DEBUG_MESSAGE_PRINT_INFO ( " FIFO_TRAN_SERVER fd %d \n",server_fifo_tran_write_fd );

            p_g_com_conn[conn_id].comm_obj_fd =  server_fifo_tran_write_fd;

        }
        else    if ( p_g_com_conn[conn_id ].cmm_type== FIFO_TRAN_CLIENT )
        {
            DEBUG_MESSAGE_PRINT_INFO ( " FIFO_TRAN_CLIENT fd %d \n",client_fifo_tran_read_fd );
            p_g_com_conn[conn_id].comm_obj_fd =  client_fifo_tran_read_fd;

        }


    }


}



/*******************************************************************************
 * Function       : sys_device_info_init
 * Author          : shiran
 * Date              : 2016.02.01
 * Description  : 串口初始化,管道初始化，协议初始化,定时器初始化
 * Calls              :      uart_init  fifo_init  protocol_init  timerSignalStateDetection
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/

void sys_device_info_init ( void )
{
  DEBUG_MESSAGE_PRINT_INFO("INIT-sys_device_info_init ");
  
    fifo_init();  //管道初始化
    protocol_init(); //协议初始化

}





/************************************************************************
*                   局部函数实现
*************************************************************************/



