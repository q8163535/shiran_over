/*************************************************
FileName:     data_process.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         data_process.c
History:
**************************************************/
#ifndef DATA_PROCESS_H
#define DATA_PROCESS_H

/************************************************************************
*                  头文件
*************************************************************************/
#include "io.h"

/************************************************************************
*                  全局函数声明
*************************************************************************/
int32_t RevProcess ( int32_t fd,uint8_t *inbuf,int32_t inlen,uint8_t *sendbuf );
int32_t distribute_dev_gdata ( void );
void touch_srceen_sign_solution ( int16_t comm_type ,int16_t trans_dev_type , uint8_t *p_record_num );

void print_buf ( int32_t arr_len,char *in_buf,int32_t in_fd, int32_t int_out_direction );
/************************************************************************
*                结构体声明
*************************************************************************/

/************************************************************************
*                  全局变量外部引用
*************************************************************************/


extern uint32_t max_dev_code  ;
extern uint32_t min_dev_code  ;





#endif
