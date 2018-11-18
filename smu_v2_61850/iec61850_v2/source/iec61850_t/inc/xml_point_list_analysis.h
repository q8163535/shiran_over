/*************************************************
FileName:     XX.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         XX.c
History:
**************************************************/
#ifndef XML_POINT_LIST_ANALYSIS_H
#define XML_POINT_LIST_ANALYSIS_H

/************************************************************************
*                  头文件
*************************************************************************/


/************************************************************************
*                  全局函数声明
*************************************************************************/

/************************************************************************
*                结构体声明
*************************************************************************/

/************************************************************************
*                  全局变量外部引用
*************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "protocol_operate.h"
#include  "file_operate.h"


uint32_t get_tab_from_xml ( uint16_t dev_code,char *p_protocol_name );

uint32_t get_XML_tab_num ( xmlChar *xpath,xmlXPathContextPtr context );

uint32_t get_tab_from_xml_union ( uint16_t dev_code,char *p_protocol_name );

#endif
