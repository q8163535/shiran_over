/*************************************************
FileName:     xml_csv.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         xml_csv.c
History:
**************************************************/

#ifndef XML_CSV_H
#define   XML_CSV_H

/************************************************************************
*                  头文件
*************************************************************************/
#include "protocol_operate.h"

/************************************************************************
*                  全局函数声明
*************************************************************************/


int32_t sysconfig_content_init ( char* configfilePATH );//XML文件读取

uint16_t  search_transmit_dev ( int32_t exampleType, int32_t modbus_slave_id );

void  get_slaveid_from_str ( slave_device_config_content   *savedev,char *strszAttr,int32_t len );


uint16_t get_element_from_str ( char *buf,char *element,char separator );

int8_t  get_xml_point_data_sign_config ( char *p_sign_element );
int8_t  get_xml_point_data_type_config ( char *p_data_element );
int8_t  get_xml_point_data_type_context ( int8_t sign_flag, int8_t data_type_flag );

//修改modbusSlaver进程配置文件pc端口的波特率和modbus站号
int modify_mbslave_dev(char *configfilePATH,uint16_t *baud,uint8_t *id);


 typedef enum
{
SIGN_U=1,SIGN_S,
}DATA_SING;
 typedef enum
{
TYPE_INT=1,TYPE_F,
}DATA_TYPE;



/************************************************************************
*                  全局变量外部引用
*************************************************************************/

extern int32_t g_protocl_num ;  		        //协议个数

extern protocol_s g_protocol[];		        //通信协议

extern CommConnObj  *p_g_com_conn;    //端口链接对象结构体指针

extern int32_t g_slave_dev_num;                  //设备类型数量

extern slave_device_config_content   *p_g_slave_device_info;  //设备类型对应内容指针



extern CommConnObj  com_device_type_object[  ];  //端口类型类型对象


//XML 配置文件目录


#define IEC61850ConfigPath   "/tmp/source/TransmitProtocol/IEC61850/"

#endif
