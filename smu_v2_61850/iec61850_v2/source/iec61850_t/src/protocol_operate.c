/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :protocol.c
* Author :shiran
* Date :2016-02-03
* Description : modbus格式数据处理，及回调函数处理
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
#include <string.h>
#include <sys/epoll.h>
#include <time.h>

#include <math.h>

#include "protocol_operate.h"
#include "protocol_cfg.h"
#include "init.h"
#include "xml_csv.h"
#include "debugSwitch.h"

#include "data_process.h"

#include "callback_function_process.h"

#include "debug_config.h"

/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/


int32_t  modbus_client_id ;   //当前进程客户端id


/*******************************************************************************
 * Function       : fifo_fill
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  :采集管道数据填充gdata全局变量
 * Calls           :
 * Input          : client_link_slaver *comm, uint8_t *inbuf, uint16_t inlen
 * Output       : uint8_t *sendbuf
 * Return        :
 ********************************************************************************/
int32_t fifo_fill ( client_link_slaver *comm, uint8_t *inbuf, uint16_t inlen,uint8_t *sendbuf )
{
    int32_t i = 0;
    uint16_t len =0;
    dev_data_t *mybuf = NULL;

    uint16_t  index_id = 0;
    uint16_t  pass_data_id = 0;
    uint16_t dev_code = 0;
    uint8_t  pass_data_type = 0;
//   uint8_t client_id= 0;



    mybuf = ( dev_data_t * ) inbuf;
    len = inlen / sizeof ( dev_data_t );

#if 0
    uint16_t dev_code; //设备类型编码
    uint16_t index;    //设备索引
    uint16_t data_id;       //测点编码
    struct
    {
        uint8_t data_type:4;//数据类型
        uint8_t wr_type:4;//读写类型 0-设置，1-回读
    };
    uint8_t clientID;  //测点上传客户端id
    data_u data;       //数据值
#endif


    for ( i = 0; i < len; ++i )
    {

        index_id = mybuf[i].index;

        pass_data_id = mybuf[i].data_id ;

        dev_code = mybuf[i].dev_code;

        pass_data_type =   mybuf[i].data_type;



        if ( ( dev_code > max_dev_code ) || ( dev_code <  min_dev_code ) )
        {
            DEBUG_FAULT ( "ERROR  BEYOND dev_code_id :%d  max_dev_code %d min_dev_code %d \n",dev_code ,max_dev_code,min_dev_code );
            break;
        }
        else
        {
            //  printf ( "dev_code_id :%d \n",dev_code );
        }

        uint32_t search_id =   search_dev_gather_case.search_map[dev_code];

        if ( ( search_id == 0xffffffff ) || ( pass_data_id > search_dev_gather_case.p_dev_gdata[search_id].dev_max_data_id ) || ( index_id >   search_dev_gather_case.p_dev_gdata[search_id].same_dev_num ) )
        {
            //DEBUG_FIFO_RECV_INFO ( "error data -- no need !\n" );
        }
        else
        {

            if ( pass_data_type == FLOAT_T )
            {
                search_dev_gather_case.p_dev_gdata[search_id].data_p[index_id][pass_data_id].f32 = mybuf[i].data.f32;
            }
            else   if ( pass_data_type == S_INT_T )
            {
                search_dev_gather_case.p_dev_gdata[search_id].data_p[index_id][pass_data_id].s32 = mybuf[i].data.s32;

            }
            else   if ( pass_data_type == U_INT_T )
            {
                search_dev_gather_case.p_dev_gdata[search_id].data_p[index_id][pass_data_id].u32 = mybuf[i].data.u32;
            }
            else
            {
                DEBUG_FIFO_RECV_INFO ( "ERROR  pass_data_type %d  \n",pass_data_type );
                break;
            }


            uint8_t func_id = 0;
            func_id =   search_dev_gather_case.p_dev_gdata[search_id].func_id_p[CB_FIFO][pass_data_id];

            if ( func_id > 0 )
            {
                comm_func[func_id] ( pass_data_id, ( data_u ) 0 ,NULL,index_id, search_id );
            }


//---------------------------------------------------------------2018-10-31 日更新日志信息打印-----------------------------------------------------------------------------------------------------
            if ( debug_module.debug_fifo_config_print  > 0 )
            {

                if ( pass_data_id == debug_module.debug_fifo_data_name )
                {
                    printf ( "dev_code %d   data_id  %d  index  %d   \n" , dev_code, pass_data_id , index_id );
                    printf ( " fifo_rev:   0-F 1-S 2-U:dataType: %d  f32 :%f   u32 :0x%0x \n",pass_data_type,mybuf[i].data.f32 ,mybuf[i].data.u32 );

                }
            }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
            if ( debug_module.debug_set > 0 )
            {
                if ( debug_module.debug_fifo_data_name > 0 )
                {
                    if ( pass_data_id == debug_module.debug_fifo_data_name )
                    {
                        DEBUG_FIFO_RECV_INFO ( "dev_code %d data_id %d  index %d  \n" , dev_code,pass_data_id,index_id );
                        DEBUG_FIFO_RECV_INFO ( " fifo_rev:   0-F 1-S 2-U:dataType: %d  f32 :%f   u32 :0x%0x \n",pass_data_type,mybuf[i].data.f32 ,mybuf[i].data.u32 );


                    }
                }
                else
                {
                    DEBUG_FIFO_RECV_INFO ( "dev_code %d data_id %d  index %d  \n" , dev_code,pass_data_id,index_id );
                    DEBUG_FIFO_RECV_INFO ( " fifo_rev:   0-F 1-S 2-U:dataType: %d  f32 :%f   u32 :0x%0x \n",pass_data_type,mybuf[i].data.f32 ,mybuf[i].data.u32 );

                }

            }
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------



        }


    }


    return 0;
}

//建立链表头结点
int32_t create_comm ( void )
{
    client_link_slaver *comm_new;
    comm_new = ( client_link_slaver * ) malloc ( sizeof ( client_link_slaver ) );
    if ( comm_new == NULL )
    {
        exit ( 1 );
    }
    memset ( comm_new,0, ( sizeof ( CommConnObj ) ) );
    comm_new->next = NULL;


    comm_reg.num = 0;
    comm_reg.comm_L = comm_new;
    comm_reg.end = comm_new;

    //printf("creat head %d\n",comm_new);
    return 0;
}


//动态注册通信连接
int32_t insert_comm ( int32_t fd,int32_t comm_conn_id )
{
    uint8_t ret = 1;
//     int32_t i = 0;
    client_link_slaver *comm_new = NULL;

    if ( comm_reg.num>=MAXEVENTS )
    {
        return 0;//超过最大连接数
    }
    comm_new = ( client_link_slaver * ) malloc ( sizeof ( client_link_slaver ) );
    memset ( comm_new,0,sizeof ( client_link_slaver ) );
    if ( comm_new == NULL )
    {
        exit ( 1 );
    }

    if ( comm_conn_id >= 0 )
    {

        comm_new->link_fd = fd;

        comm_new->comm_conn_id  = comm_conn_id;

        comm_reg.end->next = comm_new;
        comm_reg.end = comm_new;

        comm_reg.num += 1;
        DEBUG ( "add comm %d\n",comm_reg.num );

        ret =  comm_reg.num;
    }
    else
    {
        ret = 0;
    }
    return ret;
}



//删除通信连接
int32_t delete_comm ( int32_t del_fd )
{

    client_link_slaver *p_head = NULL;
    client_link_slaver  *p_tmp = NULL;
//     int32_t i = 0;

    int32_t reg_id  = 0;
    int32_t reg_num  = 0;

    //p = comm_reg.comm_L;
    //p = p->next;//头结点是空节点
    //p = p->next;//头结点是空节点


    reg_num = comm_reg.num;

    p_head = comm_reg.comm_L;//头结点是空节点


    if ( reg_num == 0 )
    {
        return  -1;
    }

    for ( reg_id = 0; reg_id < reg_num; ++reg_id )
    {
        if ( p_head->next == NULL )
        {
            break;
        }
        p_tmp = p_head->next;


        if ( p_tmp->link_fd == del_fd )
        {
            p_head->next = p_tmp->next;

            if ( p_tmp->next == NULL )
            {
                comm_reg.end = p_head;
            }
            //printf("del %d\n",p_tmp);
            free ( p_tmp );
            comm_reg.num--;
            return 0;
        }
        else
        {
            p_head  = p_head->next ;

        }
    }
    return -1;//删除失败,不存在要删除的节点
}



/******************************************************************************
 * Function       : search_proper_protocol
 * Author         : maxm
 * Date           : 2014.08.06
 * Description    : modbus从站数据解析,处理,回复
 * Calls          : None
 * Input          : buf: 接收报文, buflen:接收报文长度,fd:通信句柄
 * Output         : None
 * Return         :
 * MODBUS_CMD_NO_SUPPORT   0x01          //不支持的命令
 * CMD_DATA_NUM_BEYOND   	   0x03          //寄存器数量越界   0x0001≤输出数量≤0x07D0   (2000)
 * CMD_ADDRESS_BEYOND          0x02          //起始地址 != OK       起始地址+输出数量 !＝OK
 * CMD_READ_WRITE_ERR             0x04          /读取离散输出!＝OK
 * MODBUS_DATA_NO_ANSWER          -1          //不应答数据
 *******************************************************************************/

int32_t  search_proper_protocol ( int32_t addr_id, protocol_s  **p_proper_protocol )
{


    int32_t ptl_search_ret = 0;


    int32_t search_result = 0;

    uint16_t ptl_id = 0 ;

    protocol_s *p_tmp_ptl = NULL;

    int32_t ptl_num = 0;

    //查找协议和协议寻址表

    p_tmp_ptl = p_g_com_conn[ FIFO_TRAN_CLIENT ].comm_protocol_gather.p_comm_ptl[0];

    ptl_num = p_g_com_conn[ FIFO_TRAN_CLIENT ].comm_protocol_gather.protocolNum ;


    for ( ptl_id  = 0; ( ptl_id < ptl_num ) && ( p_tmp_ptl != NULL ) ; )
    {


        {

            //站号和地址类型符合
            if ( ( ( p_tmp_ptl->address + p_tmp_ptl->len ) >= ( addr_id ) ) && ( p_tmp_ptl->address <= ( addr_id ) )
	      &&( p_g_com_conn[ FIFO_TRAN_CLIENT ].one_reg_dev_code == p_tmp_ptl->ptl_dev_code ) )
            {
                //地址符合范围
                *p_proper_protocol = p_tmp_ptl;

                search_result = 1;  //存在对应协议，协议地址符合

                break;
            }
            else
            {
                search_result = 2;   //存在对应协议，但协议地址不符合
            }
        }

        ptl_id++;
        p_tmp_ptl = p_g_com_conn[ FIFO_TRAN_CLIENT ].comm_protocol_gather.p_comm_ptl[ ptl_id ];

    }

    if ( 0  ==   search_result )  //search_result == 0 没有查找合适的协议存在
    {
        // ptl_search_ret =    MODBUS_DATA_NO_ANSWER;

#if   DEBUG_BACKGROUND_PRINT
        printf ( "没有查找合适的协议存在 ptl_search_ret   %d \n",ptl_search_ret );
#endif
        //2017-05-27  设备存在  没有查找合适的协议存在  作为寄存器地址越界
        ptl_search_ret =   CMD_ADDRESS_BEYOND;//寄存器地址非法

    }
    else    if ( 1 ==   search_result )  //search_result == 1 查找到合适的协议存在
    {
        //查找合适的协议存在
        ptl_search_ret =    0;
    }
    else    if ( 2  ==   search_result )  //协议存在，地址不符合
    {
        ptl_search_ret =   CMD_ADDRESS_BEYOND;//寄存器地址非法

#if   DEBUG_BACKGROUND_PRINT
        printf ( "寄存器地址非法 ptl_search_ret   %d \n",ptl_search_ret );
#endif

    }

    return ptl_search_ret;

}




int32_t fifo_tran_fill ( client_link_slaver *comm, uint8_t *inbuf, uint16_t inlen,uint8_t *sendbuf )
{

    int32_t addr_rev = 0;
    int32_t data_check_result = 0;
    int32_t protocol_search_result = 0;

    protocol_s *proper_protocol = NULL;


    protocol_search_result =   search_proper_protocol ( addr_rev, &proper_protocol );


    //设备不在
    if ( protocol_search_result  < 0 )
    {

        return  1 ;

    }
    else if ( protocol_search_result > 0 )   //寄存器地址不正确
    {

        return  2 ;

    }
    else    if ( protocol_search_result  ==  0 )  //寄存器地址正确
    {

    }


    protocol_data_s    *protocol_data = proper_protocol->protocol_data;
    uint32_t  *map =  proper_protocol->map;
    uint16_t ptl_no  =  proper_protocol->ptl_dev_code;
    uint16_t first_addr =  proper_protocol->address;
    uint16_t cur_addr = addr_rev;

    int32_t tmp32 = 0;
    int16_t tmp16 = 0;
    uint16_t index = 0;
    uint16_t data_name=0;
    uint16_t end_address = 0;


    int32_t ret = 0;

    uint16_t post_offset = 0;

    uint32_t ptl = p_g_com_conn[ FIFO_TRAN_CLIENT ].one_reg_dev_code;
    
   uint16_t  search_id =   search_dev_gather_case.search_map[ptl_no];
    if ( search_id == 0xffffffff )
    {
        DEBUG_NET_RECV_INFO ( " 设备不存在 不应答    dev_code %d\n", ptl_no );

        return 3;
    }


    uint16_t addr_id = 0;

    int32_t config_index_id =0;

    int32_t slave_index_id = 0 ;


    do
    {

        addr_id = cur_addr - ( first_addr -1 ) ;

        index = map[ addr_id ];
        {

            slave_index_id = 1;

            data_name = protocol_data[index].data_id;


            //设备通讯异常 应答无效数据
            int32_t dev_comm_flag  = 0;

            dev_comm_flag  =    search_dev_gather_case.p_dev_gdata[search_id].dev_communicate_state[ slave_index_id];

            if ( DEV_COMM_FAULT ==  dev_comm_flag )
            {
                ret = CMD_READ_WRITE_ERR;   //设备通讯异常  读写错误
                DEBUG_NET_RECV_INFO ( "  设备通讯异常 应答无效数据   dev_code  %d    \n", ptl_no );

                break;
            }

            uint8_t func_id = 0;

            func_id =   search_dev_gather_case.p_dev_gdata[search_id].func_id_p[CB_SEND][data_name];

            if ( func_id > 0 )
            {
                comm_func[func_id] ( data_name, ( data_u ) 0 ,NULL,slave_index_id, search_id );
            }



            if ( protocol_data[index].data_type ==  S_INT_T )
            {
                tmp32 = search_dev_gather_case.p_dev_gdata[search_id].data_p[slave_index_id][data_name].s32;
            }
            else  if ( protocol_data[index].data_type ==  U_INT_T )
            {
                tmp32 = search_dev_gather_case.p_dev_gdata[search_id].data_p[slave_index_id][data_name].u32;

            }
            else if ( protocol_data[index].data_type ==  FLOAT_T )
            {

                if ( search_dev_gather_case.p_dev_gdata[search_id].data_p[slave_index_id][data_name].f32 >= 0 )
                {
                    tmp32 = ( int32_t ) ( search_dev_gather_case.p_dev_gdata[search_id].data_p[slave_index_id][data_name].f32/protocol_data[index].dec_p + 0.5 ) ;
                }
                else
                {
                    tmp32 = ( int32_t ) ( search_dev_gather_case.p_dev_gdata[search_id].data_p[slave_index_id][data_name].f32/protocol_data[index].dec_p - 0.5 ) ;
                }

            }


        }
    }
    while ( 0 );

    return ret;

}
