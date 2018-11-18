/*************************************************
FileName:     net.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         net.c
History:
**************************************************/

#ifndef _NET_H
#define _NET_H



/************************************************************************
*                  头文件
*************************************************************************/
#include "protocol_operate.h"

/************************************************************************
*                  全局函数声明
*************************************************************************/

int32_t make_socket_non_blocking ( int32_t sfd );

int32_t create_and_bind ( net_port_attr  port_link ,    char  netN );

void epoll_add_event ( int32_t epoll_fd,int32_t fd,uint32_t  EventType );

int32_t shutdown_net_client_conn ( int32_t client_fd );



int32_t  epoll_insert_net_server ( CommConnObj  *  insertComm_reg ,int32_t comm_id );

void fault_state_check ( int ret_s,  char * errorShow );

int32_t clear_net_epoll ( void );

void  set_keep_alive_sockopt ( int32_t  socketFd );


/************************************************************************
*                结构体声明
*************************************************************************/

/************************************************************************
*                  全局变量外部引用
*************************************************************************/


#endif
