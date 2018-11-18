/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :debugSwitch.c
* Author :shiran
* Date :2015-12-08
* Description :
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2016-08-24: 版本：V 0.1 作者: shiran
* Modification :
*------------------------------------------------------------------------------
******************************************************************************/





/************************************************************************
*                   头文件
*************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/

/************************************************************************
*                   全局变量
*************************************************************************/


/************************************************************************
*                   本地变量（即静态全局变量）
*************************************************************************/


/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/


/************************************************************************
*                   全局函数
*************************************************************************/


void terminal_print_time_lable ( void )
{
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );
    printf ( "time: %d.%d.%d-%d:%d:%d\n",ptm.tm_year+1900,ptm.tm_mon+1,ptm.tm_mday,ptm.tm_hour,ptm.tm_min,ptm.tm_sec );

}



void print_time_lable_show ( char *p_rec_str )
{
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );
    printf ( "time: %d.%d.%d-%d:%d:%d  --: %s \n",ptm.tm_year+1900,ptm.tm_mon+1,ptm.tm_mday,ptm.tm_hour,ptm.tm_min,ptm.tm_sec,p_rec_str );

}




void fun_print_time(char * input_str)
{
    struct timeval t1;
    
    gettimeofday ( &t1, NULL );
    
    long out_put_time = ( ( long ) t1.tv_sec ) *1000+ ( long ) t1.tv_usec/1000;
    
    printf ( "modbus_slave_%s %lu ms\n", input_str,out_put_time );
 
}

/*******************************************************************************
 * Function       : print_buf
 * Author          : shiran
 * Date              : 2016.11.13
 * Description  : 打印接受数据  ，
 * Calls              :
 * Input          : in_fd  接受句柄   int32_t arr_len 数据长度 int32_t *in_buf 数据缓冲  int32_t int_out_direction 接受或者发送
 * Output       :
 * Return        :oid
 ********************************************************************************/
void print_buf ( int32_t arr_len,char *in_buf,int32_t in_fd, int32_t int_out_direction )
{
#if BUF_PRINT
    if ( int_out_direction  ==  0 )
    {
        printf ( "\n begin receive: readptr %d fd: %d \n ",arr_len ,in_fd );
    }
    else
    {
        printf ( "\n begin send : readptr %d fd: %d \n ",arr_len ,in_fd );
    }
    int32_t arr_id = 0 ;

    for ( arr_id = 0 ; arr_id<arr_len  ; arr_id++ )
    {
        printf ( "0x%0x ",in_buf[ arr_id ] );
    }
    printf ( "\n end\n " );

#endif
}


/************************************************************************
*                   局部函数
*************************************************************************/
