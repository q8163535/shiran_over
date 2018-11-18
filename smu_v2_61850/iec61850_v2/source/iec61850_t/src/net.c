/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :net.c
* Author :shiran
* Date :2016-02-03
* Description : 网络处理
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2016-02-03: 版本：V 0.1 作者: shiran
* Modification :
*------------------------------------------------------------------------------
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <arpa/inet.h>


// #include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

/************************************************************************
*                   头文件
*************************************************************************/
#include "protocol_operate.h"
#include "net.h"
#include "protocol_operate.h"
#include "protocol_cfg.h"
#include "init.h"


#include "xml_csv.h"
#include "debugSwitch.h"

#include "file_operate.h"

/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/

/************************************************************************
*                   全局变量
*************************************************************************/



/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/



/************************************************************************
*                   全局函数
*************************************************************************/


/*******************************************************************************
 * Function       : make_socket_non_blocking
 * Author          : NULL
 * Date              : 2016.05.23
 * Description  :设置网络客户端链接为非阻塞模式
 * Calls           :
 * Input          :  int sfd
 * Output       : NULL
 * Return        :返回 0
 ********************************************************************************/
int make_socket_non_blocking ( int sfd )
{
    int flags, s;

    //得到文件状态标志
    flags = fcntl ( sfd, F_GETFL, 0 );
    if ( flags == -1 )
    {
        perror ( "get fcntl  error" );
        abort ();
        return -1;
    }

    //设置文件状态标志
    flags |= O_NONBLOCK;
    s = fcntl ( sfd, F_SETFL, flags );
    if ( s == -1 )
    {
        perror ( "fcntl error make_socket_non_blocking" );
	DEBUG_FAULT ( "fcntl error make_socket_non_blocking" );
        abort ();
        return -1;
    }

    return 0;
}



/************************************************************************
*                   局部函数
*************************************************************************/







/*******************************************************************************
 * Function       : epoll_add_event
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  :设置event方式加入到epoll句柄中
 * Calls           : create_and_bind  make_socket_non_blocking  listen
 * Input          :  int32_t epoll_fd  句柄集,int32_t  fd 网络设备句柄,uint32_t  EventType 读入边缘方式（预留）
 * Output       :
 * Return        :
 ********************************************************************************/
void epoll_add_event ( int32_t epoll_fd,int32_t fd,uint32_t  EventType )
{

    int32_t s;
    struct epoll_event event;

    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;   //读入,边缘触发方式
    s = epoll_ctl ( epoll_fd, EPOLL_CTL_ADD, fd, &event );
    if ( s == -1 )
    {
        perror ( "epoll_add_event epoll_ctl err" );
        DEBUG_FAULT ( "epoll_add_event:epoll_ctl err" );
        abort ();
    }
}


/*******************************************************************************
 * Function       : EpollDelEvent
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  :设置event方式加入到epoll句柄中
 * Calls           : create_and_bind  make_socket_non_blocking  listen
 * Input          :  int32_t epoll_fd  句柄集,int32_t  fd 网络设备句柄,uint32_t  EventType 读入边缘方式（预留）
 * Output       :
 * Return        :
 ********************************************************************************/
void EpollDelEvent ( int32_t epoll_fd,int32_t fd,uint32_t  EventType )
{


    struct epoll_event event;

    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;   //读入,边缘触发方式
//     s = epoll_ctl ( epoll_fd, EPOLL_CTL_ADD,  );

    if ( epoll_ctl ( epoll_fd, EPOLL_CTL_DEL, fd, &event ) )
    {
        perror ( "epoll_ctl(DEL)" );
        DEBUG_FAULT ( "EpollDelEvent:epoll_ctl err" );
        return ;
    }

}





/*******************************************************************************
 * Function       : program_state_check();
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : 错误处理
 * Calls           : perror  abort
 * Input          :  int32_t ret_s,  char * errorShow
 * Output       :
 * Return        :
 ********************************************************************************/
void fault_state_check ( int ret_s,  char * errorShow )
{

//     printf ( "s = %d\n", ret_s );
    if ( ret_s == -1 )
    {
        perror ( errorShow );
        abort ();
    }


}






/*******************************************************************************
 * Function       : set_keep_alive_sockopt
 * Author          : shiran
 * Date              : 2016.05.17
 * Description  : 设置客户端链接属性
 * Calls              :
 * Input             :  int32_t  socketFd
 * Output          :
 * Return           :NULL
 ********************************************************************************/
void  set_keep_alive_sockopt ( int32_t  socketFd )
{
    int keepalive = 1; // 开启keepalive属性
    int keepidle = 10; // 如该连接在60秒内没有任何数据往来,则进行探测
    int keepinterval = 5; // 探测时发包的时间间隔为5 秒
    int keepcount = 3; // 探测尝试的次数。如果第1次探测包就收到响应了,则后2次的不再发。
    setsockopt ( socketFd, SOL_SOCKET, SO_KEEPALIVE, ( void * ) &keepalive , sizeof ( keepalive ) );
    setsockopt ( socketFd, SOL_TCP, TCP_KEEPIDLE, ( void* ) &keepidle , sizeof ( keepidle ) );
    setsockopt ( socketFd, SOL_TCP, TCP_KEEPINTVL, ( void * ) &keepinterval , sizeof ( keepinterval ) );
    setsockopt ( socketFd, SOL_TCP, TCP_KEEPCNT, ( void * ) &keepcount , sizeof ( keepcount ) );

}
