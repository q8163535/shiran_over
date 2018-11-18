/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :protocol_cfg.c
* Author :shiran
* Date :2016-02-03
* Description : 初始化协议，创建索引表
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "protocol_operate.h"
#include "protocol_cfg.h"
#include "xml_csv.h"
#include "../../io.h"

#include "debugSwitch.h"

/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/


/************************************************************************
*                   全局变量
*************************************************************************/
// data_u gdata[MAXGDATANUM][DATA_TOTEL]= {0};
// data_u gdata[MAXGDATANUM][DATA_TOTEL] ;//= {0};
comm_reg_s comm_reg;


/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/
static int protocol_map_creat ( protocol_data_s *ptl_dat,uint32_t *map,uint16_t protocol_num,int32_t cur_ptl_min_addr );

/************************************************************************
*                   全局函数
*************************************************************************/



/*******************************************************************************
 * Function       : protocol_init
 * Author          : shiran
 * Date              : 2016.02.01
 * Description  : 通信协议初始化
 * Calls              :
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/

int protocol_init ( void )
{
    uint32_t *Map=NULL,Map_num = 0;
//     uint16_t tmp = 0;
    uint8_t ptlnum = 0;

    //add shiran++
    ptlnum = g_protocl_num;

    DEBUG ( "g_protocl_num %d \n",g_protocl_num );
    int i;

    for ( i = 0; i < ptlnum; ++i )
    {

        // Map_num = g_protocol[i].address + g_protocol[i].len;

        Map_num =   g_protocol[i].len  + 1 ;

        Map = malloc ( Map_num* ( sizeof ( uint32_t ) ) );
        if ( Map == NULL )
        {
            return -1;
        }


        memset ( Map,0xff,Map_num* ( sizeof ( uint32_t ) ) );


        DEBUG_CONFIG ( "\ng_protocol[i].ptl_no  %d ------g_protocol[i].len %d----  g_protocol[i].ptl_data_num %d  ---------\n " , g_protocol[i].ptl_dev_code,g_protocol[i].len , g_protocol[i].ptl_data_num );

        if ( g_protocol[i].len < g_protocol[i].ptl_data_num )
        {
	   DEBUG_FAULT( "protocol-file analysis error\n" );
            perror ( "protocol-file analysis error\n" );
            return -1;
        }

        protocol_map_creat ( g_protocol[i].protocol_data,Map, g_protocol[i].ptl_data_num ,g_protocol[i].ptl_min_addr );

        g_protocol[i].map = Map;

    }

    return 0;
}


/************************************************************************
*                   局部函数
*************************************************************************/


/*******************************************************************************
 * Function       : protocol_map_creat
 * Author          : shiran
 * Date              : 2016.02.01
 * Description  : 生成协议地址索引表
 * Calls              :
 * Input          :protocol_data_s *ptl_dat,uint32_t *map,uint16_t protocol_num,int32_t cur_ptl_min_addr
 * Output       :
 * Return        :
 ********************************************************************************/

static int protocol_map_creat ( protocol_data_s *ptl_dat,uint32_t *map,uint16_t protocol_num,int32_t cur_ptl_min_addr )
{

    // uint32_t *PointNameMap=NULL;
    uint32_t i;


    for ( i =0; i < protocol_num; i++ )
    {

        map[ptl_dat[i].data_address-cur_ptl_min_addr] = i;


        //map[ptl_dat[i].data_address-cur_ptl_min_addr] = i;
    }


    return 1;
}



