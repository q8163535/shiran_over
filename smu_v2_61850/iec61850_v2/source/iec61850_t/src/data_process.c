/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :data_process.c
* Author :shiran
* Date :2016-10-31
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

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "debug_config.h"
#include "debugSwitch.h"
#include "protocol_operate.h"
#include "protocol_cfg.h"
#include "init.h"
#include "xml_csv.h"

#include "data_process.h"

#include "callback_function_process.h"


/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/




#define  CMD_DATA_LIMIT_BEYOND      0x05


/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/

/************************************************************************
*                   全局变量
*************************************************************************/

search_dev_gather search_dev_gather_case;


uint32_t max_dev_code ;
uint32_t min_dev_code  ;

/************************************************************************
*                   本地变量（即静态全局变量）
*************************************************************************/


/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/
int32_t distribute_dev_gdata ( void );

int32_t RevProcess ( int32_t fd,uint8_t *inbuf,int32_t inlen,uint8_t *sendbuf );


/************************************************************************
*                   全局函数
*************************************************************************/





/*******************************************************************************
 * Function       : RevProcess
 * Author          : shiran
 * Date              : 2016.02.01
 * Description  : 查找对应fd,函数调用.
 * Calls           : p_tmp_obj->func
 * Input          : inbuf
 * Output       : sendbuf
 * Return        :
 ********************************************************************************/



int32_t RevProcess ( int32_t fd,uint8_t *inbuf,int32_t inlen,uint8_t *sendbuf )
{
    client_link_slaver *p_tmp_obj =comm_reg.comm_L->next;
    //p_tmp_obj = comm_reg.comm_L->next;
    int32_t ret = 0;
    int32_t rev_ret  = 0;
    int32_t i = 0;
    for ( i = 0; i < comm_reg.num; ++i )
    {

        if ( fd == p_tmp_obj->link_fd )
        {

            ret = ( p_g_com_conn[p_tmp_obj->comm_conn_id].func ) ( p_tmp_obj,inbuf,inlen,p_tmp_obj->retbuf );

            if ( ret <  0 )
            {
                rev_ret = ret;
            }
            else
            {
                memcpy ( sendbuf,p_tmp_obj->retbuf,p_tmp_obj->retnum );
                rev_ret = p_tmp_obj->retnum;
            }

            break;

        }
        p_tmp_obj = p_tmp_obj->next;
    }

    return  rev_ret ;
}






/*******************************************************************************
 * Function       : distribute_dev_gdata
 * Author          : shiran
 * Date              : 2016.11.01
 * Description  :分配设备全局数据  设备全局回调函数
 * Calls           :
 * Input          : inbuf
 * Output       : sendbuf
 * Return        :
 ********************************************************************************/
int32_t distribute_dev_gdata ( void )
{

    int32_t max_data_id      = 0 ;
    int32_t  max_dev_num  = 0;
    uint16_t  id = 0;



    dev_gather_data *p_tmp_dev_gdata = NULL;

    p_tmp_dev_gdata = ( dev_gather_data * ) malloc ( sizeof ( dev_gather_data ) *g_slave_dev_num ) ;

    if ( p_tmp_dev_gdata == NULL )
    {
        exit ( 1 );
    }

    int32_t dev_case_id = 0;

    uint32_t *map = NULL;



    for ( dev_case_id  = 0; dev_case_id < g_slave_dev_num ; dev_case_id ++ )
    {
        if ( min_dev_code >  p_g_slave_device_info[ dev_case_id ].dev_code )
        {
            min_dev_code =  p_g_slave_device_info[ dev_case_id ].dev_code;
        }
        if ( max_dev_code <  p_g_slave_device_info[ dev_case_id ].dev_code )
        {
            max_dev_code =  p_g_slave_device_info[ dev_case_id ].dev_code;
        }

        p_tmp_dev_gdata[dev_case_id  ].dev_code = p_g_slave_device_info[ dev_case_id ].dev_code;




        max_data_id = p_g_slave_device_info[ dev_case_id ].max_data_id;

        max_dev_num  =p_g_slave_device_info[ dev_case_id ].slave_id_cor_num;

        p_tmp_dev_gdata[dev_case_id  ].dev_max_data_id = max_data_id;
        p_tmp_dev_gdata[dev_case_id  ].same_dev_num = max_dev_num;

// 	printf( " p_tmp_dev_gdata[dev_case_id  ].dev_max_data_id %d \n ", p_tmp_dev_gdata[dev_case_id  ].dev_max_data_id );



        int32_t *dev_comm_state= NULL;

        dev_comm_state= ( int32_t * ) malloc ( sizeof ( int32_t ) * ( max_dev_num+1 ) );
        memset ( dev_comm_state,0,sizeof ( int32_t ) * ( max_dev_num+1 ) );


        //申请  max_dev_num+1 个设备  第0 个设备 作为广播地址

        data_u **data_p = NULL;
        data_p= ( data_u ** ) malloc ( sizeof ( data_u * ) * ( max_dev_num +1 ) );
        if ( data_p ==  NULL )
        {
            exit ( 1 );
        }
        memset ( data_p  ,0,  sizeof ( data_u * ) *  max_dev_num+1 );

//申请   max_data_id+1 边界扩大 1
        for ( id = 0; id < max_dev_num+1; ++id )
        {
            data_p[id] = ( data_u * ) malloc ( sizeof ( data_u ) * ( max_data_id+1 ) );
            if ( data_p[id] ==  NULL )
            {
                exit ( 1 );
            }
            memset ( data_p[id],0,sizeof ( data_u ) * ( max_data_id+1 ) );

        }


        uint8_t **func_id_p= NULL;
        uint16_t  name_id  = 0;

        func_id_p = ( uint8_t ** ) malloc ( sizeof ( uint8_t * ) * MAX_CB_NUM );

        for ( name_id = 0; name_id < MAX_CB_NUM ; name_id++ )
        {
            func_id_p[name_id] = ( uint8_t * ) malloc ( sizeof ( uint8_t ) * ( max_data_id+1 ) );
            memset ( func_id_p[name_id],0,sizeof ( uint8_t ) * ( max_data_id+1 ) );

        }


        p_tmp_dev_gdata[dev_case_id  ].dev_communicate_state = dev_comm_state ;

        p_tmp_dev_gdata[dev_case_id  ].func_id_p = func_id_p;

        p_tmp_dev_gdata[dev_case_id  ].data_p = data_p;

    }
    map = ( uint32_t * ) malloc ( sizeof ( uint32_t ) * ( max_dev_code+1 ) );

    memset ( map,0xff,  sizeof ( uint32_t ) * ( max_dev_code+1 ) );


    for ( dev_case_id  = 0; dev_case_id < g_slave_dev_num ; dev_case_id ++ )
    {
        map[ p_g_slave_device_info[ dev_case_id ].dev_code ] =  dev_case_id;
    }
    search_dev_gather_case.p_dev_gdata = p_tmp_dev_gdata;

    search_dev_gather_case.search_map  = map;

    search_dev_gather_case.dev_type_num = g_slave_dev_num;




    int32_t ptl_id = 0;

    for ( ptl_id  = 0; ptl_id < g_protocl_num ; ptl_id ++ )
    {
      
        uint16_t dev_code  =  g_protocol[ ptl_id ].ptl_dev_code;
        uint16_t search_id =   search_dev_gather_case.search_map[dev_code];

        int32_t ptl_data_num =  g_protocol[ ptl_id ].ptl_data_num;

        int32_t ptl_data_order = 0;
        char rec_function[CALL_FUN_MAX_LEN] = {0 };
        char send_function[CALL_FUN_MAX_LEN] = {0} ;
        char fifo_function[CALL_FUN_MAX_LEN] = {0} ;
        int32_t data_id = 0;

        uint8_t insert_id  = 0;

        for ( ptl_data_order = 0 ; ptl_data_order <ptl_data_num  ; ptl_data_order ++ )
        {
            strcpy ( rec_function    ,  g_protocol[ ptl_id ].protocol_data[ptl_data_order].func_Rev );
            strcpy ( send_function ,  g_protocol[ ptl_id ].protocol_data[ptl_data_order].func_Send );
            strcpy ( fifo_function ,  g_protocol[ ptl_id ].protocol_data[ptl_data_order].func_Fifo );

            data_id  =  g_protocol[ ptl_id ].protocol_data[ptl_data_order].data_id;

            insert_id=insert_function_id ( rec_function );

    

            search_dev_gather_case.p_dev_gdata[search_id].func_id_p[CB_REC][data_id]= insert_id;
	    
	            if ( insert_id > 0 )
            {
                DEBUG_CALLBACK_INFO ( "  insert_id  %d rec_function  %s   %d \n",insert_id,rec_function,data_id );

            }

            insert_id = insert_function_id ( send_function );

            if ( insert_id > 0 )
            {
                DEBUG_CALLBACK_INFO ( "  insert_id  %d  send_function:  %s   data_id %d \n",insert_id,send_function,data_id );

            }

            search_dev_gather_case.p_dev_gdata[search_id].func_id_p[CB_SEND][data_id]= insert_id;

            insert_id = insert_function_id ( fifo_function );
            if ( insert_id > 0 )
            {
                DEBUG_CALLBACK_INFO ( "  insert_id  %d  fifo_function:  %s   data_id %d \n",insert_id,fifo_function,data_id );

            }

            search_dev_gather_case.p_dev_gdata[search_id].func_id_p[CB_FIFO][data_id]= insert_id;


        }


    }


    return 0;

}









/************************************************************************
*                   局部函数
*************************************************************************/
