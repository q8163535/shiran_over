#include <unistd.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/socket.h>      /* basic socket definitions */
#include <sys/select.h>       /* select function*/
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <fcntl.h>
#include "socket_comm.h"
#include "protocol_operate.h"
#include "data_process.h"

#include "callback_function_process.h"
#include "debugSwitch.h"


#include "xml_csv.h"



#define GET_U32(data) (data[0] << 8) | data[1] | (data[2] << 24) | (data[3] << 16)


int sever_sock_communicate_fd;  //数据处理 socket 通讯句柄

int32_t msg_load ( uint8_t *inbuf,uint32_t length,sg_msg_s *msg );
int32_t msg_context_handle ( sg_msg_s *msg );

int32_t socket_hand ( void );

int32_t create_send_msg ( uint8_t *outbuf ,uint32_t cmd,uint32_t para,char *data_msg );
void modbus_client_register_process ( int32_t connfd );

/**
 * [CheckCRC16 crc 校验]
 * @param  ptr [校验数组指针]
 * @param  len [校验数组长度]
 * @return     [0:成功,-1:失败]
 */
int CheckCRC16 ( volatile uint8_t *ptr, uint16_t len )
{
    int ret = -1;
    uint8_t i;
    uint16_t crc=0xFFFF;
    if ( len==0 )
    {
        len=1;
    }
    while ( len-- )
    {
        crc ^= *ptr;
        for ( i=0; i<8; i++ )
        {
            if ( crc&1 )
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        ptr++;
    }
    if ( crc == 0 )
    {
        ret = 0;
    }
    return ret;
}

/**
 * [GetCRC16 计算crc16值]
 * @param  ptr [待计算数组指针]
 * @param  len [待机算数组长度]
 * @return     [crc计算结果]
 */
uint16_t GetCRC16 ( volatile uint8_t *ptr, uint16_t len )
{
    uint8_t i;
    uint16_t crc=0xFFFF;
    if ( len==0 )
    {
        len=1;
    }
    while ( len-- )
    {
        crc ^= *ptr;
        for ( i=0; i<8; i++ )
        {
            if ( crc&1 )
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        ptr++;
    }
    return ( crc );
}



/******************************************************************************
* Function       :msg_load
* Author         :
* Date           :2017-2-21
* Description    :socket信息校验
* Calls          :none
* Table Accessed :none
* Table Updated  :none
* Input          :
* Output         :none
* Return         :none
******************************************************************************/
int32_t msg_load ( uint8_t *inbuf,uint32_t length,sg_msg_s *p_msg )
{
    int32_t ret = 0;

  //  printf ( "%d length\n",length );
    
    uint32_t crc_len = 0;
    
    
    if ( ( length >=  14 ) && ( length < 1040 ) )
    {
        crc_len = ( * ( uint32_t * ) &inbuf[8] )  + 14 ;
    }

    if ( ( length < 14 ) || ( length > 1040 ) )
    {
        ret = -1;//格式错误
    }
    else if ( 0 == CheckCRC16 ( inbuf,crc_len ) )
    {
        p_msg->len = ( * ( uint32_t * ) &inbuf[8] );
        memset ( p_msg->data,0,sizeof ( p_msg->data ) );
        memcpy ( p_msg->data,&inbuf[12],p_msg->len );
        p_msg->cmd = ( * ( uint32_t * ) inbuf );
        p_msg->para = ( * ( uint32_t * ) &inbuf[4] );

        ret =  p_msg->len + 14;
    }
    else
    {
        int32_t len_id = 0;

        DEBUG_NET_RECV_TIME_INFO ( "msg_load receive_buf_\n " );

        for ( len_id = 0; len_id< length; len_id ++ )
        {
            DEBUG_NET_RECV_INFO ( " 0x%0x ",inbuf[ len_id ] );
        }
        DEBUG_NET_RECV_INFO ( " \n" );

        ret = -1;//crc错误
    }
    return ret;
}




/******************************************************************************
* Function       :create_send_msg
* Author         :
* Date           :2017-3-24
* Description    :创建网络socket 通讯 内容
* Calls          :none
* Table Accessed :none
* Table Updated  :none
* Input          :
* Output         :none
* Return         :none
******************************************************************************/
int32_t create_send_msg ( uint8_t *outbuf ,uint32_t cmd,uint32_t para,char *data_msg )
{
    //创建命令
    //  msg->len = strlen ( "ModbusSlave" );
    sg_msg_s remsg = {0};

    remsg.cmd = cmd;
    remsg.para = para;
    memset ( remsg.data,0,sizeof ( remsg.data ) );

    remsg.len = ( uint32_t ) strlen ( data_msg );
    memcpy ( remsg.data, data_msg,remsg.len ) ;



    memcpy ( outbuf,& ( remsg.cmd ),sizeof ( uint32_t ) );
    memcpy ( outbuf+4,& ( remsg.para ),sizeof ( uint32_t ) );
    memcpy ( outbuf+8,& ( remsg.len ),sizeof ( uint32_t ) );
    memcpy ( outbuf+12, remsg.data,remsg.len );

    remsg.crc = GetCRC16 ( outbuf,remsg.len+12 );

    memcpy ( outbuf+remsg.len+12,& ( remsg.crc ),sizeof ( uint16_t ) );
    return remsg.len+14;
}


/******************************************************************************
* Function       :modbus_client_register_process
* Author         :
* Date           :2017-3-24
* Description    :modbus_client_register_process   modbusSlave 注册 等待处理
* Calls          :none
* Table Accessed :none
* Table Updated  :none
* Input          :
* Output         :none
* Return         :none
******************************************************************************/
void modbus_client_register_process ( int32_t connfd )
{
    uint8_t recvline[MAXLINE];
    fd_set rset;
    int32_t nread;
    int32_t tempval_rev = 0;
    struct timeval tempval;
    int32_t ret = 0;

    int32_t max_fd  =  0;

    max_fd  = connfd + 1;

    while ( 1 )
    {
        tempval_rev = 3000;
        tempval.tv_sec = tempval_rev/1000;
        tempval.tv_usec = 1000* ( tempval_rev%1000 );

        FD_ZERO ( &rset );
        FD_SET ( connfd, &rset );

        ret = select ( max_fd, &rset, NULL, NULL, &tempval );

        if ( ret == -1 )
        {
            perror ( "select error" );
	    DEBUG_FAULT ( "select error" );
        }
        else if ( ret == 0 )
        {

        }
        else
        {
            if ( FD_ISSET ( connfd, &rset ) )
            {
                //接收到服务器响应
                nread = recv ( connfd, recvline, MAXLINE ,0 );
                if ( nread == 0 )
                {

                }
                else if ( nread == -1 )
                {
                    perror ( "read error" );
		  DEBUG_FAULT ( "read error" );
                }
                else
                {
// modbus_client_register 成功
                    if ( socket_rev_process ( recvline, nread ) >  0 )
                    {
                        break;
                    }


                }
            }
        }
    }
}





/******************************************************************************
* Function       :socket_hand
* Author         :
* Date           :2017-3-24
* Description    :app modbusSlave 握手
* Calls          :none
* Table Accessed :none
* Table Updated  :none
* Input          :
* Output         :none
* Return         :none
******************************************************************************/
int32_t socket_hand ( void )
{

    struct sockaddr_un servaddr;

    sever_sock_communicate_fd = socket ( PF_UNIX, SOCK_STREAM, 0 );

    if ( sever_sock_communicate_fd<0 )
    {
        perror ( "cannot create communication socket" );
	
	DEBUG_FAULT( "cannot create communication socket" );
        return -1;
    }

    bzero ( &servaddr, sizeof ( servaddr ) );
    servaddr.sun_family = AF_UNIX;
    strcpy ( servaddr.sun_path,UNIX_DOMAIN );

    for ( ; connect ( sever_sock_communicate_fd, ( struct sockaddr * ) &servaddr, sizeof ( servaddr ) ) < 0; )
    {
        usleep ( 5000 );
    }

    DEBUG_NET_RECV_INFO ( "connect ok! sever_sock_communicate_fd = %d\n", sever_sock_communicate_fd );
    

    uint8_t obuf[128] = {0};

    int32_t send_len =   create_send_msg ( obuf,MSG_LOGIN,1 ,"ModbusSlave" );
    int32_t send_num = 0;

    ssize_t  send_tmp = 0;

    while ( send_tmp != send_len )
    {
        send_tmp = send ( sever_sock_communicate_fd,& ( obuf[send_num] ),send_len,0 );

        if ( send_tmp == -1 )
        {
            if ( errno == EWOULDBLOCK || errno == EAGAIN || errno == EINPROGRESS )
            {

                continue;
            }
            else
            {
                send_tmp = -1;
                break;
            }
        }
        else    if ( send_tmp <  send_len )
        {
            send_num += send_tmp ;
            send_len -= send_tmp;
        }
        DEBUG_NET_RECV_INFO ( "connect ok!  = %d  send_tmp %d   send_len  %d\n", sever_sock_communicate_fd ,send_tmp,  send_len );

    }



    modbus_client_register_process ( sever_sock_communicate_fd );

    DEBUG_NET_RECV_INFO ( "socket_hand init   modbus_client_register_process  %d \n " ,sever_sock_communicate_fd );

    return sever_sock_communicate_fd;
}





void msg_show_context ( sg_msg_s *tmp_msg )
{

    int32_t i = 0;
    DEBUG_NET_RECV_INFO ( "tmp_msg->cmd = %d para = %d  len = %d \n", tmp_msg->cmd ,tmp_msg->para,tmp_msg->len);
    DEBUG_NET_RECV_INFO ( "tmp_msg->data :\n" );
    for ( i = 0; i < tmp_msg->len; ++i )
    {
        DEBUG_NET_RECV_INFO ( "%d ", tmp_msg->data[i] );
    }
    DEBUG_NET_RECV_INFO ( "\n" );

}
/******************************************************************************
* Function       :msg_handle
* Author         :
* Date           :2017-2-21
* Description    :socket信息接收处理
* Calls          :none
* Table Accessed :none
* Table Updated  :none
* Input          :
* Output         :none
* Return         :none
******************************************************************************/
int32_t msg_context_handle ( sg_msg_s *msg )
{
    int32_t ret = -1;

    msg_show_context ( msg ) ;

    if ( msg->cmd == MSG_LOGIN ) //注册
    {
        if ( 2 == msg->para )
        {
            DEBUG_NET_RECV_INFO ( "收到注册请求回复:client = %d\n", msg->data[0] );

            modbus_client_id =  msg->data[0] ;
            DEBUG_NET_RECV_INFO ( "modbus_client_id %d \n ",modbus_client_id );
            ret = modbus_client_id;
        }
    }
    else if ( msg->cmd == MSG_NETMODF ) //地址传递
    {
        if ( 0 == msg->para )
        {
            DEBUG_NET_RECV_INFO ( "收到ip地址指令：IP1 = %d  IP2 = %d IP3 = %d IP4 = %d \n", msg->data[0],msg->data[1],msg->data[2],msg->data[3] );

         //   get_net_ip1_data ( msg->data[0],msg->data[1],msg->data[2],msg->data[3] );
            ret = 0;
        }
        else if ( 1 == msg->para )
        {
            DEBUG_NET_RECV_INFO ( "收到ip地址指令：IP21 = %d IP22 = %d IP23 = %d IP24 = %d \n", msg->data[0],msg->data[1],msg->data[2],msg->data[3] );

          //  get_net_ip2_data ( msg->data[0],msg->data[1],msg->data[2],msg->data[3] );

            ret = 0;
        }

    }
    else     if ( msg->cmd == MSG_DEVCOMMERR ) //通讯异常
    {

        int32_t index_id  = 0 ;
        uint16_t dev_code = 0;
	
        DEBUG_NET_RECV_INFO ( "收到设备故障状态：msg->data[0] %d msg->data[1] %d msg->data[2] %d msg->data[3] = %d \n", msg->data[0],msg->data[1],msg->data[2],msg->data[3] );

	   dev_code  = ( ( msg->data[1] <<8 ) | ( msg->data[0] ) );
	   
            index_id = ( ( msg->data[3] <<8 ) | ( msg->data[2] ) );

        if ( ( dev_code <= max_dev_code ) && ( dev_code >=  min_dev_code ) )
        {

            uint32_t search_id =   search_dev_gather_case.search_map[dev_code];

            if ( ( search_id != 0xffffffff ) && ( index_id <=   search_dev_gather_case.p_dev_gdata[search_id].same_dev_num ) )
            {

                if ( 1 == msg->para )
                {
                    search_dev_gather_case.p_dev_gdata[search_id].dev_communicate_state[ index_id] = DEV_COMM_FAULT;
                }
                else
                {
                    search_dev_gather_case.p_dev_gdata[search_id].dev_communicate_state[ index_id] = DEV_COMM_NORMAL;

                }
                ret = 0;

            }

        }

    }
    
 
 
    return ret;
}

/******************************************************************************
* Function       :socket_rev_process
* Author         :
* Date           :2017-2-21
* Description    :正常通讯时接收socket命令处理
* Calls          :none
* Table Accessed :none
* Table Updated  :none
* Input          :
* Output         :none
* Return         :none
******************************************************************************/
int32_t socket_rev_process ( uint8_t *inbuf, uint16_t inbuf_len )
{
    sg_msg_s rev_msg = {0};
    int32_t socket_ret = -1 ;

    int16_t rev_len = 0;

    int16_t once_len = 0;

    while (   inbuf_len > 0  )
    {
        memset( &rev_msg ,0,sizeof ( sg_msg_s )  ) ;
	
        if (  ( once_len = msg_load ( &inbuf[ rev_len],inbuf_len,&rev_msg )   ) >  0 )
        {

            socket_ret =   msg_context_handle ( &rev_msg );
	    
            if ( socket_ret < 0 )
            {
                break;
            }
              DEBUG_NET_RECV_INFO ( "msg_load GET once_len  %d !\n",once_len );
             rev_len += once_len ;
	    inbuf_len -= once_len; 

        }
        else
        {
	  DEBUG_FAULT ( "socket_rev_process --socket crc error !\n" );
	  break;
            
        }
    }
    return socket_ret ;
}






//modbus 主站接收数据处理
void   socket_communicate_process ( const  int32_t *p_conn_socket_fd )
{
    struct timeval tempval;
    int32_t tempval_rev;
// struct timeval t_start;
    fd_set rdfds;
    uint8_t inbuf[1024]= {0};
    int32_t ret = 0;
    int32_t inbuf_len = 0;

    int32_t fd = *p_conn_socket_fd;

    int32_t max_fd = fd +1 ;

    while ( 1 )
    {
        tempval_rev = 500;     //超时喂狗时间

        tempval.tv_sec = tempval_rev/1000;  //load timing
        tempval.tv_usec = 1000* ( tempval_rev%1000 );

        FD_ZERO ( &rdfds );
        FD_SET ( fd, &rdfds );
        // max_fd = ( fd +1 );;

        ret = select ( max_fd+1, &rdfds, NULL, NULL, &tempval ); //wait to rev

        if ( ret < 0 )
        {
            perror ( "select" );
	    DEBUG_FAULT( "select" );
        }
        else if ( ret == 0 )
        {
         
        }
        else
        {
            //socket数据
            if ( FD_ISSET ( fd, &rdfds ) )
            {
                inbuf_len = recv ( fd,inbuf,1024,0 );
                if ( inbuf_len > 0 )
                {
                    socket_rev_process ( inbuf, inbuf_len );
                }
                else if ( inbuf_len ==  -1 )
                {
                    DEBUG_FAULT ( "socket disconnected ! \n" );
                }

            }
        }
    }
}

