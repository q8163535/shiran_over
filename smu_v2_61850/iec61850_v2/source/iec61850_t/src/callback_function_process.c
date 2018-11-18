/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :callback_function_process.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <time.h>

#include "io.h"
#include "callback_function_process.h"
#include "xml_csv.h"

#include "debugSwitch.h"


#include "protocol_operate.h"

#include "init.h"

/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/
uint8_t  insert_function_id ( char *fuction_name );

void network_infomation_init ( void );



uint8_t get_index_from_func_names ( char *element );




//static int32_t get_net_mask_info ( const char* comm_name,int32_t *p_mask1,int32_t *p_mask2,int32_t *p_mask3,int32_t *p_mask4 );



//static void get_dns_element ( char *dns_element, int32_t *p_dns1,int32_t *p_dns2,int32_t *p_dns3,int32_t *p_dns4 );



// static int32_t  get_dns ( void );

// static int get_gateway ( void );

/************************************************************************
*                   全局变量
*************************************************************************/



int32_t   IP1=192;
int32_t   IP2=168;
int32_t   IP3=1;
int32_t   IP4=100;

int32_t   SUBMASK1=255;
int32_t   SUBMASK2=255;
int32_t   SUBMASK3=255;
int32_t   SUBMASK4=0;

int32_t   DNS11=1;
int32_t   DNS12=2;
int32_t   DNS13=4;
int32_t   DNS14=8;

int32_t   DNS21=8;
int32_t   DNS22=8;
int32_t   DNS23=8;
int32_t   DNS24=8;

int32_t   GATEWAY1=192;
int32_t   GATEWAY2=168;
int32_t   GATEWAY3=1;
int32_t   GATEWAY4=254;







int32_t   IP21=192;
int32_t   IP22=168;
int32_t   IP23=0;
int32_t   IP24=100;

int32_t   SUBMASK21=255;
int32_t   SUBMASK22=255;
int32_t   SUBMASK23=255;
int32_t   SUBMASK24=0;

int32_t   DNS211=1;
int32_t   DNS212=2;
int32_t   DNS213=4;
int32_t   DNS214=8;

int32_t   DNS221=8;
int32_t   DNS222=8;
int32_t   DNS223=8;
int32_t   DNS224=8;

// 192.168.64.254
int32_t   GATEWAY21=192;
int32_t   GATEWAY22=168;
int32_t   GATEWAY23=0;
int32_t   GATEWAY24=254;





/************************************************************************
*                   全局函数
*************************************************************************/




char *func_names[MB_FUNC_MAX_NUM] =
{
    " ",
    "fifo_request_signal_send",
    NULL
};

// //发送pc后台信号 至  03 04
// int32_t send_pc_comm_signal_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )

int32_t ( *comm_func[MB_FUNC_MAX_NUM] ) ( uint16_t name,data_u old_data,pro_data_s *,int32_t indexid,uint16_t search_id ) =
{

    NULL,
    NULL

};




uint8_t insert_function_id ( char *fuction_name )
{
    uint8_t index = 0;

    index = get_index_from_func_names ( fuction_name );

    return index;

}

/************************************************************************
*                   局部函数
*************************************************************************/

uint8_t get_index_from_func_names ( char *element )
{
    uint8_t  ret_func_id = 0;

    uint8_t  index = 0;

    // printf ( "element  %s \n ",element );

    for ( index = 0; index < MB_FUNC_MAX_NUM; ++index )
    {
        if ( func_names[ index ] == NULL )
        {
            ret_func_id = 0;
            break;
        }
        else  if ( 0 == strcmp ( element,func_names[ index ] ) )
        {
            ret_func_id = index;
            break;
        }
    }
    return ret_func_id;

}

//管道数据处理

int32_t fifo_request_signal_send ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );










//管道接收年月日 时分秒  设置标记
int32_t fifo_time_finsih_flag_set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );

int32_t fifo_address_set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );

int32_t fifo_dev_comm_state_set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );


//06 10 数据处理




int32_t receive_Net_Mask2_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t receive_Net_Getway2_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t receive_Net_DNS12_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid ,uint16_t search_id );
int32_t receive_Net_DNS22_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );




int32_t receive_Net_Mask3_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t receive_Net_Getway3_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t receive_Net_DNS13_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid ,uint16_t search_id );
int32_t receive_Net_DNS23_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );




int32_t receive_Net_Mask4_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t receive_Net_Getway4_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t receive_Net_DNS14_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid ,uint16_t search_id );
int32_t receive_Net_DNS24_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );


int32_t receive_Net_DHCP_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );




//06 10 数据处理
int32_t     receive_time_year ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_mon ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_day ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_hour ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_min ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_second ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );

int32_t	receive_time_lcd_year ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_mon ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_day ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_hour ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_min ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_second ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );

int32_t	receive_inv_address_change ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid ,uint16_t search_id );




//03 04 命令   回调函数   receive
int32_t send_time_year_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_mon_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_day_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_hour_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_min_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_second_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );

int32_t send_time_year_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_mon_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_day_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_hour_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_min_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t send_time_second_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );

int32_t send_pc_comm_signal_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );

//01 信号变位
int32_t send_cnt_flag_signal_change ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );


#if 0
/*******************************************************************************
 * Function       : initial_request_signal_send
 * Author          : NULL
 * Date              : 2016.05.17
 * Description  : 初始化发送 请求设置信息命令
 * Calls              :
 * Input             :   int32_t  fd  管道fd
 * Output          :
 * Return           :返回发送计数
 ********************************************************************************/
int32_t fifo_request_signal_send ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
    int32_t count = 0;

    int32_t sendnum = sizeof ( dev_data_t ) *1;
    dev_data_t initFifoData = { 0 };
    initFifoData.clientID = modbus_client_id ;
    initFifoData.data_type= S_INT_T;
    initFifoData.index = 1;
    initFifoData.data_id =   name ;
    initFifoData.data.s32 = 0xaa;
    initFifoData.dev_code = 1797;

    count = write ( server_fifo_write_fd, ( uint8_t * ) &initFifoData, sendnum );

    return count;
}







int32_t fifo_net_ip1_set ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_ip !=   IP1 )
    {
        IP1 =  tmp_ip;
        net_ip_reset_flag =  1;
    }

#if 1
    DEBUG_CALLBACK_INFO ( "fifo_net_ip1_set IP1 %d\n" ,IP1 );
#endif

    return 0;
}

int32_t fifo_net_ip2_set ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_ip !=   IP2 )
    {
        IP2 =  tmp_ip;
        net_ip_reset_flag =  1;
    }


#if 1
    DEBUG_CALLBACK_INFO ( "fifo_net_ip2_set IP2 %d\n" ,IP2 );
#endif

    return 0;
}

int32_t fifo_net_ip3_set ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_ip !=   IP3 )
    {
        IP3 =  tmp_ip;
        net_ip_reset_flag =  1;
    }


#if 1
    DEBUG_CALLBACK_INFO ( "fifo_net_ip3_set IP3 %d \n" ,IP3 );
#endif

    return 0;
}

int32_t fifo_net_ip4_set ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_ip !=   IP4 )
    {
        IP4 =  tmp_ip;
        net_ip_reset_flag =  1;
    }


#if 1
    DEBUG_CALLBACK_INFO ( "fifo_net_ip4_set IP4 %d \n" ,IP4 );
#endif

    return 0;
}


int32_t fifo_net_ip2_1_set ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_ip !=   IP21 )
    {
        IP21 =  tmp_ip;
        net_ip_reset_flag =  1;
    }

#if 1
    DEBUG_CALLBACK_INFO ( "fifo_net_ip2_1_set IP21 %d \n",IP21 );
#endif

    return 0;
}

int32_t fifo_net_ip2_2_set ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    IP22 =  search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_ip !=   IP22 )
    {
        IP22 =  tmp_ip;
        net_ip_reset_flag =  1;
    }

#if 1
    DEBUG_CALLBACK_INFO ( "fifo_net_ip2_2_set IP22 %d  \n",IP22 );
#endif

    return 0;
}

int32_t fifo_net_ip2_3_set ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_ip !=   IP23 )
    {
        IP23 =  tmp_ip;
        net_ip_reset_flag =  1;
    }


#if 1
    DEBUG_CALLBACK_INFO ( "fifo_net_ip2_3_set   IP23 %d \n",IP23 );
#endif

    return 0;
}

int32_t fifo_net_ip2_4_set ( uint16_t name ,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_ip !=   IP24 )
    {
        IP24 =  tmp_ip;
        net_ip_reset_flag =  1;
    }

#if 1
    DEBUG_CALLBACK_INFO ( "fifo_net_ip2_4_set  IP24 %d \n",IP24 );
#endif

    return 0;
}








//管道接收年月日 时分秒  设置标记    对时标记  FIFO_TIME_FINISH
int32_t fifo_time_finsih_flag_set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    //   if ( ( pass_data_id == YEAR ) || ( pass_data_id == MONTH ) || ( pass_data_id == DATE ) || ( pass_data_id == HOUR ) || ( pass_data_id == MIN ) || ( pass_data_id == SECOND ) )
#if 1
    DEBUG_CALLBACK_INFO ( "fifo_time_finsih_flag_set \n" );
#endif
    time_setting_start = 1;
    fifo_get_time_flag = 1;

    //search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][FIFO_TIME_FINISH].s32 = 0;

    sock_send_time_finish_flag =  0 ;
    //change shiran  FIFO_TIME_FINISH 约定修改
    return 0;
}

int32_t fifo_address_set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "fifo_address_set \n" );
#endif
    //   if ( pass_data_id == ADDRESS )

    int32_t inv_first_slave_id  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;
    uint16_t dev_code  =      search_dev_gather_case.p_dev_gdata[search_id].dev_code ;

    changIdCorIndex ( inv_first_slave_id , dev_code );

    return 0 ;

}




/********************************************************************************
 * Function       : fifo_dev_comm_state_set
 * Author          : shiran
 * Date              : 2016.05.21
 * Description  : 管道接收设置设备通讯状态回调函数
 * Calls              : None
 * Input             :
 * Output          :
 * Return           : int32_t
 *********************************************************************************/
int32_t fifo_dev_comm_state_set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

    int32_t dev_comm_state_data  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 ;

    search_dev_gather_case.p_dev_gdata[search_id].dev_communicate_state[indexid] = dev_comm_state_data;

#if 1
    //print_time_lable_show ( "fifo_rec_dev_comm_state" );
    DEBUG_CALLBACK_INFO ( "fifo_dev_comm_state_set  : dev_comm_state_data %d dev_code:%d  indexid : %d \n",    dev_comm_state_data,search_dev_gather_case.p_dev_gdata[search_id].dev_code,indexid );
#endif

    return 0;
}






//06 10

void system_time_flag_set ( int32_t pcTimeSetFlag  ,char  *p_lcdTimeArr,char *p_pcTimeArr )
{
    if ( 1 ==    pcTimeSetFlag )
    {
        pcTimeSetFlag =  0;
        // date 2016.10.11-12:22:30
        system ( p_pcTimeArr );
        system ( "hwclock -u -w" );
	
	printf ( "system_time_flag_set \n" );

        time_setting_start = 1;
    }
#if 0
//修改 去除 触摸屏下发时间时 不设置当前时间
    if ( 1  !=  time_setting_start )
    {

        system ( p_lcdTimeArr );
        system ( "hwclock -u -w" );
    }
#endif

}

int32_t  receive_time_year ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_year \n" );
#endif

    return 0;
}
int32_t	receive_time_mon ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_mon \n" );
#endif
   
    return 0;

}
int32_t	receive_time_day ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_day \n" );
#endif

    return 0;

}
int32_t	receive_time_hour ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_hour \n" );
#endif
  
    return 0;
}
int32_t	receive_time_min ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_min \n" );
#endif

    return 0;
}
int32_t	receive_time_second ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_second \n" );
#endif
   

    return 0;
}


int32_t	receive_time_lcd_year ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_lcd_year \n" );
#endif

    return 0;
}
int32_t	receive_time_lcd_mon ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if DEBUG_CALLBACK_PRINT
    DEBUG_CALLBACK_INFO ( "receive_time_lcd_mon \n" );
#endif
   
    return 0;

}
int32_t	receive_time_lcd_day ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_lcd_day \n" );
#endif

    return 0;
}
int32_t	receive_time_lcd_hour ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_lcd_hour \n" );
#endif
  
    return 0;
}
int32_t	receive_time_lcd_min ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_lcd_min \n" );
#endif

    return 0;
}
int32_t	receive_time_lcd_second ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "receive_time_lcd_second \n" );
#endif

    return 0;
}



//06 10
/*******************************************************************************
 * Function       : inv_address_change
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : 上位机修改 逆变器地址
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t	receive_inv_address_change ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid ,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "receive_inv_address_change \n" );
#endif
    int32_t inv_first_slave_id  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    uint16_t dev_code  =      search_dev_gather_case.p_dev_gdata[search_id].dev_code ;

    changIdCorIndex ( inv_first_slave_id , dev_code );

    return 0;
}









//03 04 命令
int32_t send_time_year_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "send_time_year_get \n" );
#endif

    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

    //  if ( 1  == time_setting_start )
    //  {

    search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_year+1900;
//       recycle_change_time_data_flag = 1;
//   }

    return 0 ;

}


//03 04 命令
int32_t send_time_mon_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "send_time_mon_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

//    if ( 1  == time_setting_start )
//   {

    search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_mon+1;
//        recycle_change_time_data_flag = 1;
//   }

    return 0 ;

}

//03 04 命令
int32_t send_time_day_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "send_time_day_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

    //  if ( 1  == time_setting_start )
    //  {

    search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_mday;
    //      recycle_change_time_data_flag = 1;
//   }

    return 0 ;

}

//03 04 命令
int32_t send_time_hour_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if 1
    DEBUG_CALLBACK_INFO ( "send_time_hour_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

//   if ( 1  == time_setting_start )
    //  {

    search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_hour;
    //     recycle_change_time_data_flag = 1;
    //  }

    return 0 ;

}

//03 04 命令
int32_t send_time_min_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "send_time_min_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

    //  if ( 1  == time_setting_start )
    //  {

    search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_min;
//       recycle_change_time_data_flag = 1;
//   }

    return 0 ;

}

//03 04 命令
int32_t send_time_second_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "send_time_second_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

//   if ( 1  == time_setting_start )
//   {

    search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_sec;
//       recycle_change_time_data_flag = 1;
//   }

    return 0 ;

}

//03 04 命令
int32_t send_time_year_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "send_time_year_lcd_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

    if ( 1  == time_setting_start )
    {
//   printf ( "time_setting_start \n" );
        search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_year+1900;
        recycle_change_time_data_flag = 1;
    }

    return 0 ;

}

//03 04 命令
int32_t send_time_mon_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "send_time_mon_lcd_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

    if ( 1  == time_setting_start )
    {
//   printf ( "time_setting_start \n" );
        search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_mon+1;
        recycle_change_time_data_flag = 1;
    }

    return 0 ;

}

//03 04 命令
int32_t send_time_day_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "send_time_day_lcd_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

    if ( 1  == time_setting_start )
    {
//   printf ( "time_setting_start \n" );
        search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_mday;
        recycle_change_time_data_flag = 1;
    }

    return 0 ;

}

//03 04 命令
int32_t send_time_hour_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "send_time_hour_lcd_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

    if ( 1  == time_setting_start )
    {
        search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_hour;
        recycle_change_time_data_flag = 1;
    }

    return 0 ;

}

//03 04 命令
int32_t send_time_min_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "send_time_min_lcd_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

    if ( 1  == time_setting_start )
    {
 // printf ( "time_setting_start \n" );
        search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_min;
        recycle_change_time_data_flag = 1;
    }

    return 0 ;

}

//03 04 命令
int32_t send_time_second_lcd_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "03 04 :send_time_second_lcd_get \n" );
#endif
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );

    if ( 1  == time_setting_start )
    {
//  printf ( "time_setting_start \n" );
        search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 = ptm.tm_sec;
        recycle_change_time_data_flag = 1;
    }

    return 0 ;

}


//发送pc后台信号 至  03 04
int32_t send_pc_comm_signal_get ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "03 04:send_pc_comm_signal_get \n" );
#endif
//    if ( COM_PC_TOT == data_name )
    search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 =  pc_background_signal_var;
    return 0 ;

}



//01 信号变位
int32_t send_cnt_flag_signal_change ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{

#if 1
    DEBUG_CALLBACK_INFO ( "01 信号变位:send_cnt_flag_signal_change recycle_change_time_data_flag %d  \n",recycle_change_time_data_flag );
#endif
    if ( recycle_change_time_data_flag == 1 )
    {
        search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 ^= 0x01;
	
    }
    else
    {
        search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32 &= ~0x0001;
    }
    return 0 ;

}






















/*******************************************************************************
 * Function       : receive_Net_IP1_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络IP地址数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t	receive_Net_IP1_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
    //addshiran
#if PRINTOPEN
    printf ( "\nNet_IP1_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ) \n" );
#endif
    // char setipstr[128];

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_ip  !=   IP1 )
    {
        //     IP1 =  tmp_ip;
        //    sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", IP1,IP2,IP3,IP4 );
        //    system ( setipstr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

        //      net_ip_reset_flag =  1;

    }

    return 0;

}


/*******************************************************************************
 * Function       : receive_Net_IP2_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络IP地址数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t	receive_Net_IP2_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
    //addshiran
#if PRINTOPEN
    printf ( "\nNet_IP1_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ) \n" );
#endif
    //  char setipstr[128];

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    // sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", IP1,IP2,IP3,IP4);

    if ( tmp_ip  !=   IP2 )
    {
        //      IP2 =  tmp_ip;

        //sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", tmp_ip,IP2,IP3,IP4 );

        //   sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", IP1,IP2,IP3,IP4 );

        //     system ( setipstr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

        //     net_ip_reset_flag =  1;

    }

    return 0;

}


/*******************************************************************************
 * Function       : receive_Net_IP3_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络IP地址数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t	receive_Net_IP3_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
    //addshiran
#if PRINTOPEN
    printf ( "\nNet_IP1_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ) \n" );
#endif
//   char setipstr[128];

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    // sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", IP1,IP2,IP3,IP4);

    if ( tmp_ip  !=   IP3 )
    {
        //    IP3 =  tmp_ip;

        //   sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", IP1,IP2,IP3,IP4 );
        //  system ( setipstr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

        //    net_ip_reset_flag =  1;

    }

    return 0;

}



/*******************************************************************************
 * Function       : receive_Net_IP4_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络IP地址数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t	receive_Net_IP4_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
    //addshiran
#if PRINTOPEN
    printf ( "\nNet_IP1_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ) \n" );
#endif
    // char setipstr[128];

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    // sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", IP1,IP2,IP3,IP4);

    if ( tmp_ip  !=   IP4 )
    {
        //    IP4=  tmp_ip;

        //      sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", IP1,IP2,IP3,IP4 );

        //      system ( setipstr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

        //     net_ip_reset_flag =  1;

    }

    return 0;

}


#if 0

/*******************************************************************************
 * Function       : receive_Net_IP21_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络IP地址数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t	receive_Net_IP21_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
    //addshiran
#if PRINTOPEN
    printf ( "\nNet_IP1_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ) \n" );
#endif
    char setipstr[128];

    int32_t  tmp_ip  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    // sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", IP1,IP2,IP3,IP4);

    if ( tmp_ip  !=   IP21 )
    {
        IP21 =  tmp_ip;

        sprintf ( setipstr,"ifconfig eth0 %d.%d.%d.%d", IP21,IP22,IP23,IP24 );

        system ( setipstr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

        net_ip_reset_flag =  1;

    }

    return 0;

}

#endif


/*******************************************************************************
 * Function       : receive_Net_Mask1_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络MAST地址数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_Mask1_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_Mask_Set 1----------------------------------- \n" );
#endif
    //addshiran

//     char setmaskstr[128]= {0};

    int32_t  tmp_mask  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_mask  !=   SUBMASK1 )
    {
        //      SUBMASK1 =      tmp_mask  ;
        //      sprintf ( setmaskstr,"ifconfig eth0 %d.%d.%d.%d netmask %d.%d.%d.%d",IP1,IP2,IP3,IP4, SUBMASK1,SUBMASK2,SUBMASK3,SUBMASK4 );
//
        //      system ( setmaskstr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }
    return 0;
}


/*******************************************************************************
 * Function       : receive_Net_Mask2_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络MAST地址数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_Mask2_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_Mask_Set 2----------------------------------- \n" );
#endif
    //addshiran

//     char setmaskstr[128]= {0};

    int32_t  tmp_mask  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;


    if ( tmp_mask  !=   SUBMASK2 )
    {
//         SUBMASK2 =      tmp_mask  ;
//         sprintf ( setmaskstr,"ifconfig eth0 %d.%d.%d.%d netmask %d.%d.%d.%d",IP1,IP2,IP3,IP4, SUBMASK1,SUBMASK2,SUBMASK3,SUBMASK4 );
//
//         system ( setmaskstr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }
    return 0;
}

/*******************************************************************************
 * Function       : receive_Net_Mask3_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络MAST地址数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_Mask3_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_Mask_Set 3----------------------------------- \n" );
#endif
    //addshiran

//     char setmaskstr[128]= {0};

    int32_t  tmp_mask  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;



    if ( tmp_mask  !=   SUBMASK3 )
    {
//         SUBMASK3 =      tmp_mask  ;
//         sprintf ( setmaskstr,"ifconfig eth0 %d.%d.%d.%d netmask %d.%d.%d.%d",IP1,IP2,IP3,IP4, SUBMASK1,SUBMASK2,SUBMASK3,SUBMASK4 );
//
//         system ( setmaskstr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }
    return 0;
}

/*******************************************************************************
 * Function       : receive_Net_Mask4_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络MAST地址数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_Mask4_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_Mask_Set 4----------------------------------- \n" );
#endif
    //addshiran

//     char setmaskstr[128]= {0};

    int32_t  tmp_mask  = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;


    if ( tmp_mask  !=   SUBMASK4 )
    {
//         SUBMASK4 =      tmp_mask  ;
//         sprintf ( setmaskstr,"ifconfig eth0 %d.%d.%d.%d netmask %d.%d.%d.%d",IP1,IP2,IP3,IP4, SUBMASK1,SUBMASK2,SUBMASK3,SUBMASK4 );
//
//         system ( setmaskstr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }
    return 0;
}


/*******************************************************************************
 * Function       : Net_Getway_Set1
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :网络getway数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_Getway1_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{

#if PRINTOPEN
    printf ( "\nNet_Getway_Set1 ----------------------------------- \n" );
#endif
    //addshiran


    int32_t  tmp_gateway = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;


    if ( tmp_gateway  !=   GATEWAY1 )
    {
//         GATEWAY1 = tmp_gateway;
//         char setgetwaystr[128];
//
//         sprintf ( setgetwaystr,"route add default gw %d.%d.%d.%d",GATEWAY1,GATEWAY2,GATEWAY3,GATEWAY4 );
//         system ( setgetwaystr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }

    return 0;
}





int32_t receive_Net_Getway2_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{

#if PRINTOPEN
    printf ( "\nNet_Getway_Set2 ----------------------------------- \n" );
#endif
    //addshiran


    int32_t  tmp_gateway = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;


    if ( tmp_gateway  !=   GATEWAY2 )
    {
//         GATEWAY2 = tmp_gateway;
//         char setgetwaystr[128];
//
//         sprintf ( setgetwaystr,"route add default gw %d.%d.%d.%d",GATEWAY1,GATEWAY2,GATEWAY3,GATEWAY4 );
//         system ( setgetwaystr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }

    return 0;
}




int32_t receive_Net_Getway3_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{

#if PRINTOPEN
    printf ( "\nNet_Getway_Set3 ----------------------------------- \n" );
#endif
    //addshiran


    int32_t  tmp_gateway = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;


    if ( tmp_gateway  !=   GATEWAY3 )
    {
//         GATEWAY3 = tmp_gateway;
//         char setgetwaystr[128];
//
//         sprintf ( setgetwaystr,"route add default gw %d.%d.%d.%d",GATEWAY1,GATEWAY2,GATEWAY3,GATEWAY4 );
//         system ( setgetwaystr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }

    return 0;
}


int32_t receive_Net_Getway4_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id )
{

#if PRINTOPEN
    printf ( "\nNet_Getway_Set4 ----------------------------------- \n" );
#endif
    //addshiran


    int32_t  tmp_gateway = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;


    if ( tmp_gateway  !=   GATEWAY4 )
    {
//         GATEWAY4 = tmp_gateway;
//         char setgetwaystr[128];
//
//         sprintf ( setgetwaystr,"route add default gw %d.%d.%d.%d",GATEWAY1,GATEWAY2,GATEWAY3,GATEWAY4 );
//         system ( setgetwaystr );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }

    return 0;
}






/*******************************************************************************
 * Function       : receive_Net_DNS11_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :DNS1数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_DNS11_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid ,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_DNS1_Set ----------------------------------- \n" );
#endif
    //addshiran

//     char setDNS1str[128];

    int32_t  tmp_dns = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_dns  !=   DNS11 )
    {
//         sprintf ( setDNS1str,"echo \"nameserver %d.%d.%d.%d\" > /etc/resolv.conf", DNS11,DNS12,DNS13,DNS14 );
//
//         printf ( setDNS1str );
//         system ( setDNS1str );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }

    return 0;
}





/*******************************************************************************
 * Function       : receive_Net_DNS12_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :DNS1数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_DNS12_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid ,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_DNS12_Set ----------------------------------- \n" );
#endif
    //addshiran

//     char setDNS1str[128];

    int32_t  tmp_dns = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_dns  !=   DNS12 )
    {
//         sprintf ( setDNS1str,"echo \"nameserver %d.%d.%d.%d\" > /etc/resolv.conf", DNS11,DNS12,DNS13,DNS14 );
//
//         printf ( setDNS1str );
//         system ( setDNS1str );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }

    return 0;
}



/*******************************************************************************
 * Function       : receive_Net_DNS13_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :DNS1数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_DNS13_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid ,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_DNS13_Set ----------------------------------- \n" );
#endif
    //addshiran

//     char setDNS1str[128];

    int32_t  tmp_dns = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_dns  !=   DNS13 )
    {
//         sprintf ( setDNS1str,"echo \"nameserver %d.%d.%d.%d\" > /etc/resolv.conf", DNS11,DNS12,DNS13,DNS14 );
//
//         printf ( setDNS1str );
//         system ( setDNS1str );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }

    return 0;
}


/*******************************************************************************
 * Function       : receive_Net_DNS14_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :DNS1数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_DNS14_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid ,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_DNS14_Set ----------------------------------- \n" );
#endif
    //addshiran

//char setDNS1str[128];

    int32_t  tmp_dns = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_dns  !=   DNS14 )
    {
//         sprintf ( setDNS1str,"echo \"nameserver %d.%d.%d.%d\" > /etc/resolv.conf", DNS11,DNS12,DNS13,DNS14 );
//
//         printf ( setDNS1str );
//         system ( setDNS1str );

        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }

    return 0;
}


/*******************************************************************************
 * Function       : receive_Net_DNS21_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :DNS1数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_DNS21_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_DNS21_Set ----------------------------------- \n" );
#endif
    //addshiran

//     char setDNS1str[128];//echo "nameserver 192.168.100.100" > /etc/resolv.conf
//
//     char setDNS2str[128];//echo "nameserver 192.168.100.100" >> /etc/resolv.conf

    int32_t  tmp_dns = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_dns  !=   DNS21 )
    {
//         sprintf ( setDNS1str,"echo \"nameserver %d.%d.%d.%d\" > /etc/resolv.conf", DNS11,DNS12,DNS13,DNS14 );
//         sprintf ( setDNS2str,"echo \"nameserver %d.%d.%d.%d\" >> /etc/resolv.conf", DNS21,DNS22,DNS23,DNS24 );
//         system ( setDNS1str );
//         system ( setDNS2str );


        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }
    return 0;

}

/*******************************************************************************
 * Function       : receive_Net_DNS22_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :DNS1数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_DNS22_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_DNS22_Set ----------------------------------- \n" );
#endif
    //addshiran

//     char setDNS1str[128];//echo "nameserver 192.168.100.100" > /etc/resolv.conf
//
//     char setDNS2str[128];//echo "nameserver 192.168.100.100" >> /etc/resolv.conf

    int32_t  tmp_dns = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_dns  !=   DNS22 )
    {
//         sprintf ( setDNS1str,"echo \"nameserver %d.%d.%d.%d\" > /etc/resolv.conf", DNS11,DNS12,DNS13,DNS14 );
//         sprintf ( setDNS2str,"echo \"nameserver %d.%d.%d.%d\" >> /etc/resolv.conf", DNS21,DNS22,DNS23,DNS24 );
//         system ( setDNS1str );
//         system ( setDNS2str );


        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }
    return 0;

}

/*******************************************************************************
 * Function       : receive_Net_DNS23_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :DNS1数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_DNS23_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_DNS23_Set ----------------------------------- \n" );
#endif
    //addshiran

//     char setDNS1str[128];//echo "nameserver 192.168.100.100" > /etc/resolv.conf
//
//     char setDNS2str[128];//echo "nameserver 192.168.100.100" >> /etc/resolv.conf

    int32_t  tmp_dns = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_dns  !=   DNS23 )
    {
//         sprintf ( setDNS1str,"echo \"nameserver %d.%d.%d.%d\" > /etc/resolv.conf", DNS11,DNS12,DNS13,DNS14 );
//         sprintf ( setDNS2str,"echo \"nameserver %d.%d.%d.%d\" >> /etc/resolv.conf", DNS21,DNS22,DNS23,DNS24 );
//         system ( setDNS1str );
//         system ( setDNS2str );


        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }
    return 0;

}


/*******************************************************************************
 * Function       : receive_Net_DNS24_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :DNS1数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_DNS24_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_DNS24_Set ----------------------------------- \n" );
#endif
    //addshiran

//     char setDNS1str[128];//echo "nameserver 192.168.100.100" > /etc/resolv.conf
//
//     char setDNS2str[128];//echo "nameserver 192.168.100.100" >> /etc/resolv.conf

    int32_t  tmp_dns = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    if ( tmp_dns  !=   DNS24 )
    {
//         sprintf ( setDNS1str,"echo \"nameserver %d.%d.%d.%d\" > /etc/resolv.conf", DNS11,DNS12,DNS13,DNS14 );
//         sprintf ( setDNS2str,"echo \"nameserver %d.%d.%d.%d\" >> /etc/resolv.conf", DNS21,DNS22,DNS23,DNS24 );
//         system ( setDNS1str );
//         system ( setDNS2str );


        databuf->id[databuf->ptr++] =  name;
        databuf->type[databuf->ptr] = S_INT_T;

    }
    return 0;

}



/*******************************************************************************
 * Function       : receive_Net_DHCP_Set
 * Author          : shiran
 * Date              : 2017.04.17
 * Description  :DNS1数据设置
 * Calls           :  uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t receive_Net_DHCP_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id )
{
#if PRINTOPEN
    printf ( "\nNet_tmp_dhcp_Set ----------------------------------- \n" );
#endif

   // int32_t  tmp_dhcp = search_dev_gather_case.p_dev_gdata[search_id].data_p[indexid][name].s32;

    databuf->id[databuf->ptr++] =  name;
    databuf->type[databuf->ptr] = S_INT_T;

    return 0;

}








static  int32_t get_net_ip_info ( const char* comm_name ,int32_t *p_ip1,int32_t *p_ip2,int32_t *p_ip3,int32_t *p_ip4 )
{

    char ip1[20] = {0} ;
    char ip2[20] = {0} ;
    char ip3[20] = {0} ;
    char ip4[20] = {0} ;
    int32_t ret  = 0;
    int32_t len = 0;
    struct ifreq ifr ;
    int32_t  socket_fd = socket ( AF_INET, SOCK_DGRAM, 0 );
    if ( socket_fd < 0 )
    {
        fprintf ( stderr, "Create socket failed!errno=%d", errno );
        ret  = -1;
    }
    else
    {

        char ip_info[30] = {0};

        strcpy ( ifr.ifr_name, comm_name );
        if ( ioctl ( socket_fd, SIOCGIFADDR, &ifr ) < 0 )
        {
            ret = -1;
        }
        else
        {

            struct sockaddr_in *myaddr;
            myaddr = ( struct sockaddr_in* ) & ( ifr.ifr_addr );
            strcpy ( ip_info , inet_ntoa ( myaddr->sin_addr ) );



            len +=   get_element_from_str ( & ip_info[len],ip1,'.' );
            //  printf ( "IP: %s: tmp_ip_info %s \n", ip_info,ip1 );

            *p_ip1 =  strtoul ( ip1 ,NULL, 0 );

            len +=   get_element_from_str ( &ip_info[len],ip2,'.' );
            DEBUG_MESSAGE_PRINT_INFO ( "IP: %s: tmp_ip_info %s \n", ip_info,ip2 );
            *p_ip2 =  strtoul ( ip2 ,NULL, 0 );

            len +=   get_element_from_str ( & ip_info[len],ip3,'.' );
            DEBUG_MESSAGE_PRINT_INFO ( "IP: %s: tmp_ip_info %s \n", ip_info,ip3 );
            *p_ip3 =  strtoul ( ip3 ,NULL, 0 );

            len +=   get_element_from_str ( & ip_info[len],ip4,'.' );
            DEBUG_MESSAGE_PRINT_INFO ( "IP: %s: tmp_ip_info %s \n", ip_info,ip4 );
            *p_ip4 =  strtoul ( ip4 ,NULL, 0 );


        }
    }
    close ( socket_fd );
    return ret;
}

#endif