/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :Modbus_s.c.c
* Author :maxm
* Date : 2016-02-03
* Description : 函数主结构
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2016-02-03: 版本：V 0.1 作者: shiran
* Modification :
* 2016-03-10: 版本：V 1.1.2 作者: shiran
*------------------------------------------------------------------------------
******************************************************************************/



/************************************************************************
*                  头文件
*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/time.h>
// #include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>



#include "net.h"
#include "protocol_operate.h"
#include "protocol_cfg.h"
#include "init.h"


#include "xml_csv.h"
#include "debugSwitch.h"

#include "file_operate.h"



#include "data_process.h"

#include "socket_comm.h"


/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/
#define READFDMAXLEN (int32_t)4104
#define SENDFDMAXLEN (int32_t)4104

/************************************************************************
*                   全局变量
*************************************************************************/

int32_t efd; //epoll socket fd

int32_t net_ip_reset_flag;  //网络重置标志



/************************************************************************
*                   本地变量（即静态全局变量）
*************************************************************************/


/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/


/************************************************************************
*                   全局函数
*************************************************************************/


/************************************************************************
*                   局部函数
*************************************************************************/


/********************************************************************************
 * Function       : comport_infomation_init
 * Author          : shiran
 * Date              : 2016.02.01
 * Description  : 初始化全局变量参数 ，创建建立链接链表，将端口实例注册进入列表，端口句柄加入到epoll中。  初始化发送信号至server
 * Calls              :  insert_comm    epoll_insert_net_server
 * Input             :
 * Output          :
 * Return           : void
 *********************************************************************************/
void comport_infomation_init ( void )
{

    int32_t id = 0; //端口链接id

    uint32_t  eventType =  EPOLLIN | EPOLLET;

    int32_t ret  = 0;
    ret =  create_comm( );//建立链接链表

    printf ( " create_comm ret = %d \n",ret );

    //除了参数size被忽略外,此函数和epoll_create完全相同
    efd = epoll_create1 ( 0 );


    fault_state_check ( efd,  "epoll_create" );

    DEBUG_MESSAGE_PRINT_INFO ( "epoll_create global %d \n",efd );
    //epoll框架,管道,网络,串口,全部注册进去,阻塞等待

    for ( id = 0; id < FIFONUM ; id++ )
    {
        //查询 串口设备和读管道

        if ( p_g_com_conn[id].cmm_type ==  FIFOCLIENT )  // 管道注册
        {


            insert_comm ( p_g_com_conn[id].comm_obj_fd, id ); //注册一个协议处理的链表实例

            epoll_add_event ( efd,p_g_com_conn[id].comm_obj_fd,eventType );  //管道客户端句柄加入到efd 中

        }
        else if ( p_g_com_conn[id].cmm_type ==  FIFOSERVER )    // 管道初始信号发送至 server
        {
            printf ( "FIFOSERVER  no  deal with  \n" );

        }
        else      if ( p_g_com_conn[id].cmm_type ==  FIFO_TRAN_CLIENT )  // 管道注册
        {


            insert_comm ( p_g_com_conn[id].comm_obj_fd, id ); //注册一个协议处理的链表实例

            epoll_add_event ( efd,p_g_com_conn[id].comm_obj_fd,eventType );  //管道客户端句柄加入到efd 中

        }
        else if ( p_g_com_conn[id].cmm_type ==  FIFO_TRAN_SERVER )    // 管道初始信号发送至 server
        {
            printf ( "FIFO_TARN_SERVER  no  deal with  \n" );

        }

    }


}



/*******************************************************************************
 * Function       : fifo_data_process
 * Author          : shiran
 * Date              : 2016.02.01
 * Description  : 管道 网络链接非阻塞 针对设备句柄操作及处理
 * Calls              :
 * Input          : struct epoll_event *p_event_case
 * Output       :
 * Return        :1:写失败, 0:非写失败
 ********************************************************************************/
int32_t  fifo_data_process ( int32_t fifo_read_fd )
{



    ssize_t count = 0;

    uint8_t inbuf[READFDMAXLEN+READFDMAXLEN]= { 0 };

    uint8_t sendbuf[SENDFDMAXLEN+SENDFDMAXLEN]= { 0 };


    int32_t readptr = 0;
    int32_t sendnum = 0;

//     printf ( "p_event_case->data.fd %d \n",p_event_case->data.fd );
    while ( 1 )
    {

        count = read ( fifo_read_fd , &inbuf[readptr], READFDMAXLEN );
        if ( count <= 0 )
        {
            break;
        }
        readptr += count;


        if ( readptr >= READFDMAXLEN )
        {
            memset ( sendbuf,0,sizeof ( sendbuf ) );
            sendnum = RevProcess ( fifo_read_fd ,inbuf,readptr,sendbuf );
            readptr = 0;
        }


    }


    //接收数据处理,结果填入sendbuf,立即回复或者异步回复
    sendnum = RevProcess ( fifo_read_fd,inbuf,readptr,sendbuf );


    int32_t sendptr = 0;

    int32_t fifo_write_fd = 0;

    if ( fifo_read_fd != p_g_com_conn[FIFO_TRAN_CLIENT].comm_obj_fd )
    {
        fifo_write_fd = p_g_com_conn[FIFO_TRAN_SERVER].comm_obj_fd;
    }
    else  if ( fifo_read_fd != p_g_com_conn[FIFOCLIENT].comm_obj_fd )
    {
        fifo_write_fd = p_g_com_conn[FIFOSERVER].comm_obj_fd;
    }

    if ( fifo_write_fd > 0 )
    {
        for ( sendptr = 0; sendptr < sendnum; )
        {
            count = write ( fifo_write_fd, &sendbuf[sendptr], sendnum );

            if ( count == -1 )
            {
                /* If errno == EAGAIN, that means send buffer is full. So send later. */
                if ( errno != EAGAIN )
                {
                    fprintf ( stderr,"write socket error %d\n",errno );
                    break;
                }
            }
            else
            {
                sendptr += count;
            }
        }
    }



    if ( debug_module.debug_set > 0 )
    {
        DEBUG_FIFO_RECV_INFO ( "\n------------------------------fifo_data_process : fd %d -----------------------------\n",fifo_write_fd );

        int32_t  send_id = 0;

        for ( send_id = 0 ; send_id < sendnum; send_id ++ )
        {
            DEBUG_FIFO_RECV_INFO ( "0x%02x ",sendbuf[ send_id ] );
        }
        DEBUG_FIFO_RECV_INFO ( " \n " );

    }




    return sendnum;

}


void init_gloabl_data ( void )
{

    setlinebuf ( stdout ); //设置行缓冲输出

    fprintf ( stderr, soft_ver );   //写入程序版本号

    signal ( SIGPIPE, SIG_IGN ); //屏蔽管道信号异常信号

}


int main ( int argc, char *argv[] )
{



    int32_t    event_fd=  0 ;

    struct epoll_event *p_events= NULL;

    int32_t epoll_ret_n = 0;
    int32_t id = 0;


    DEBUG_TIME_INIT ( "\n-------init_gloabl_data --------------------\n" );

    init_gloabl_data( );

    DEBUG_INIT ( "\n--------socket_hand----------------------\n" );

    socket_hand();    //app握手模块

//debug.xml  初始化配置
    xml_debug_config_read ( DEBUG_PRINT_PATH );

    DEBUG_INIT ( "\n--------sysconfig_content_init init xml ---------------------------\n" );

    sysconfig_content_init ( CONFIGFILEPATH );

    DEBUG_INIT ( "\n--------distribute_dev_gdata----------------------\n" );


    distribute_dev_gdata ( );

    //  DEBUG_INIT ( "\n--------sys_device_info_init: uart_init- fifo_init- protocol_init-timerSignalStateDetection----------------------\n" );

    DEBUG_INIT ( "\n串口初始化,管道初始化，协议初始化,定时器初始化\n" );


    sys_device_info_init ( );// 串口初始化,管道初始化，协议初始化,定时器初始化



    DEBUG_INIT ( "\n----------------------端口信息初始化 ---------------------------\n" );


    comport_infomation_init();  //端口信息初始化


    p_events = ( struct epoll_event * ) calloc ( MAXEVENTS, sizeof ( struct epoll_event ) );
    if ( p_events == NULL )
    {
        fprintf ( stderr,"calloc p_events error exit\n" );
        return -1;
    }

    /* The event loop */
    while ( 1 )
    {

        epoll_ret_n = epoll_wait ( efd, p_events, MAXEVENTS, 500 );
        if ( epoll_ret_n > 0 )
        {
            for ( id = 0; id < epoll_ret_n; id++ )
            {
                event_fd  =  p_events[id].data.fd;


                if ( ( p_events[id].events & EPOLLERR ) ||
                        ( p_events[id].events & EPOLLHUP ) ||
                        ( ! ( p_events[id].events & EPOLLIN ) ) )
                {
                    /* An error has occured on this fd, or the socket is not
                    ready for reading (why were we notified then?) */
                    //printf("events[i].events = %d\n,%d,%d,%d\n",events[i].events,EPOLLERR,EPOLLHUP,EPOLLIN);

                    DEBUG_FAULT ( "error ---events[i].data.fd = %d\n",event_fd );

                }
                else if ( p_events[id].events & EPOLLIN ) //读事件触发
                {
                    /* We have data on the fd waiting to be read. Read and
                    display it. We must read whatever data is available
                    completely, as we are running in edge-triggered mode
                    and won't get a notification again for the same
                    data. */

                    fifo_data_process ( event_fd );


                }
            }
        }


    }


    free ( p_events );

    return EXIT_SUCCESS;


}





