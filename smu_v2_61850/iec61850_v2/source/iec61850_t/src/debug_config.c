
/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :xml-csv.c
* Author :shiran
* Date :2017-07-24
* Description : 解析XML文件读取 debug 配置文件
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2017-07-24 版本：V 0.1 作者: shiran
* Modification :
*------------------------------------------------------------------------------
******************************************************************************/



/************************************************************************
*                   头文件
*************************************************************************/

#include <stdio.h>
#include <string.h>


#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "io.h"

#include "xml_csv.h"
#include "debug_config.h"

/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/

/************************************************************************
*                   全局变量
*************************************************************************/

/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/

/************************************************************************
*                   全局函数
*************************************************************************/
int32_t xml_debug_config_read ( char* configfilePATH );


debug_module_slave  debug_module;


/*******************************************************************************
* Function       : xml_debug_config_read
* Author          : shiran
* Date              : 2016.06.23
* Description  : 配置写入xml版本信息
* Calls              :
* Input             : char *pzaname 文件名称
* Output          :
* Return           : 0
********************************************************************************/



int32_t xml_debug_config_read ( char* configfilePATH )
{
    xmlDocPtr doc; //定义解析文档指针
//     xmlNodePtr rootNode,Node1,Node2,Node3;//,Node4; //定义结点指针(你需要它为了在各个结点间移动),四级嵌套

    char *szDocName;

    szDocName = configfilePATH;
    doc = xmlReadFile ( szDocName,"UTF-8",XML_PARSE_RECOVER ); //解析文件
    //检查解析文档是否成功，如果不成功，libxml将指一个注册的错误并停止。
    //一个常见错误是不适当的编码。XML标准文档除了用UTF-8或UTF-16外还可用其它编码保存。
    //如果文档是这样，libxml将自动地为你转换到UTF-8。更多关于XML编码信息包含在XML标准中.
    if ( NULL == doc )
    {
        fprintf ( stderr,"Document not parsed successfully\n" );
        return -1;
    }




    xmlNodeSetPtr nodeset=NULL;
    xmlXPathObjectPtr result=NULL;
    xmlXPathContextPtr context=NULL;

    context = xmlXPathNewContext ( doc );


    xmlNodePtr tmp_node = NULL;


    xmlChar *xpath_fifo_config = ( xmlChar * )  "/root/debug/transfer/iec61850/fifo_print" ;
    xmlChar *xpath_pc_rev_config= ( xmlChar * ) "/root/debug/transfer/iec61850/pc_rev_print" ;
    xmlChar *xpath_debug_set_config= ( xmlChar * ) "/root/debug/transfer/iec61850/debug_set_mask" ;


    result = xmlXPathEvalExpression ( xpath_fifo_config, context );
    if ( result )
    {

        nodeset = result->nodesetval;

        int32_t node_type_id = 0;

        for ( node_type_id = 0; node_type_id < nodeset->nodeNr ; node_type_id++ )
        {
            tmp_node =  nodeset->nodeTab[  node_type_id ];

            if ( !xmlStrcmp ( tmp_node->name,BAD_CAST "fifo_print" ) )
            {
                //查找属性
                xmlNodePtr  type_node_ptr =NULL;
                type_node_ptr = tmp_node;
                xmlAttrPtr type_node_attr_ptr = type_node_ptr->properties;

                while ( type_node_attr_ptr != NULL )
                {

                    if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "enable" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "enable" );

                        debug_module.debug_fifo_config_print=    strtoul ( ( char * ) szAttr,NULL, 10 );
		      printf("debug_module.debug_fifo_config_print__ %d \n",debug_module.debug_fifo_config_print);

                        xmlFree ( szAttr );
                    }
                    else  if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "data_name" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "data_name" );


                        debug_module.debug_fifo_data_name    =    strtoul ( ( char * ) szAttr,NULL, 10 );
		
			printf("debug_module.debug_fifo_data_name __ %d \n",debug_module.debug_fifo_data_name );

                        xmlFree ( szAttr );
                    }
                    else  if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "index" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "index" );

                        debug_module.debug_fifo_dev_index =    strtoul ( ( char * ) szAttr,NULL, 10 );

		      printf("debug_module.debug_fifo_dev_index __ %d \n",   debug_module.debug_fifo_dev_index );

                        xmlFree ( szAttr );
                    }

                    type_node_attr_ptr = type_node_attr_ptr->next;

                }
            }
        }
    }

    xmlXPathFreeObject ( result );


    result = xmlXPathEvalExpression ( xpath_pc_rev_config, context );
    if ( result )
    {

        nodeset = result->nodesetval;

        int32_t node_type_id = 0;

        for ( node_type_id = 0; node_type_id < nodeset->nodeNr ; node_type_id++ )
        {
            tmp_node =  nodeset->nodeTab[  node_type_id ];

            if ( !xmlStrcmp ( tmp_node->name,BAD_CAST "pc_rev_print" ) )
            {
                //查找属性
                xmlNodePtr  type_node_ptr =NULL;
                type_node_ptr = tmp_node;
                xmlAttrPtr type_node_attr_ptr = type_node_ptr->properties;

                while ( type_node_attr_ptr != NULL )
                {
		  
                    if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "enable" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "enable" );

                        debug_module.debug_pc_rev_config_print =    strtoul ( ( char * ) szAttr,NULL, 10 );
                        //   get_id_from_names


                        xmlFree ( szAttr );
                    }
                    else  if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "modbus_id" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "modbus_id" );

                        
			debug_module.debug_modbus_id =     strtoul ( ( char * ) szAttr,NULL, 10 );


                        xmlFree ( szAttr );
                    }
                    else  if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "cmd" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "cmd" );

                  
                     debug_module.debug_cmd =    strtoul ( ( char * ) szAttr,NULL, 10 );

                        xmlFree ( szAttr );
                    }
                
                    type_node_attr_ptr = type_node_attr_ptr->next;

                }
            }
        }
    }
    xmlXPathFreeObject ( result );
    
    
    
    
    
    //-------------------------------------------
    
      result = xmlXPathEvalExpression ( xpath_debug_set_config, context );
    if ( result )
    {

        nodeset = result->nodesetval;

        int32_t node_type_id = 0;

        for ( node_type_id = 0; node_type_id < nodeset->nodeNr ; node_type_id++ )
        {
            tmp_node =  nodeset->nodeTab[  node_type_id ];

            if ( !xmlStrcmp ( tmp_node->name,BAD_CAST "debug_set_mask" ) )
            {
                //查找属性
                xmlNodePtr  type_node_ptr =NULL;
                type_node_ptr = tmp_node;
                xmlAttrPtr type_node_attr_ptr = type_node_ptr->properties;

                while ( type_node_attr_ptr != NULL )
                {
		   if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "debug_set" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "debug_set" );

                  
                     debug_module.debug_set =    strtoul ( ( char * ) szAttr,NULL, 0 );

                        xmlFree ( szAttr );
                    }
                  
                  
                    type_node_attr_ptr = type_node_attr_ptr->next;

                }
            }
        }
    }
    xmlXPathFreeObject ( result );
    //-------------------------------------------

    xmlFreeDoc ( doc );

    return 0;
}

