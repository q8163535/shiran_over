/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :xml-csv.c
* Author :shiran
* Date :2016-02-03
* Description : 解析XML文件读取CSV 文件
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>


#include "../../io.h"
#include "protocol_operate.h"
#include "protocol_cfg.h"
#include "xml_csv.h"
#include "file_operate.h"
#include "debugSwitch.h"
#include "xml_point_list_analysis.h"

#include "init.h"

/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/
//读写管道数量
#define RW_FIFO_NUM (int32_t )4

//网络最多支持端口号数量
#define MAXNETPORTNUM (int32_t)5

//端口类型类型对象数量
#define MAXCOMDEVTPPENUM (int32_t )10

#define XML_VERSION_PATH   "/home/SGLogger/Config/AppVer_ModbusSlave.xml"

static int xml_write_FirmwareVersion_config ( char* szDocName );


static int32_t get_node_parameter_num ( xmlXPathContextPtr context , const xmlChar *xpath );

/************************************************************************
*                   全局变量
*************************************************************************/


CommConnObj  *p_g_com_conn;              			//全局连接设备接口 ：现有 有两个串口 两个网口 两个fifo 根据配置文件读取

int32_t g_slave_dev_num;  				           	 //设备结构体数量

slave_device_config_content   *p_g_slave_device_info;   //设备结构体

int32_t g_protocl_num;		       				          //全局协议个数
protocol_s g_protocol[PROTOCOLNUM];	                   //全局通信协议


CommConnObj  com_device_type_object[ MAXCOMDEVTPPENUM ]; 		 //端口类型类型对象




int32_t g_client_fifo_connet_id  ; //管道读客户端全局链接序号
int32_t g_server_fifo_connect_id  ;//管道写客户端全局链接序号



//全局函数数据初始化
CommObjFunc FuncType[]=
{
    NULL,
    fifo_tran_fill,	////client_fifo  数据处理读管道读数据
    NULL, 	 //client_fifo   数据处理读管道读数据
    fifo_fill
};




/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/


//static int8_t get_tab_from_csv ( protocol_s *protocolData, uint16_t dev_code, int32_t  proCmd,char *PATH );

// static int32_t xmlCommportConfig ( xmlXPathContextPtr context );

/************************************************************************
*                   全局函数
*************************************************************************/
uint16_t get_element_from_str ( char *buf,char *element,char separator );


/*******************************************************************************
 * Function       : search_init_conn_protocol
 * Author          : shiran
 * Date              : 2016.11.08
 * Description  : 端口链接索引协议初始化
 * Calls              :
 * Input          : CommObj_s  *p_tmp_ptl_gather ,slave_device_config_content   *p_conn_slave_device_info ,int32_t dev_code
 * Output       :
 * Return        :max_data_id  同设备编码中最大的数据id
 ********************************************************************************/
static  int32_t search_init_conn_protocol ( CommObj_s  *p_tmp_ptl_gather ,slave_device_config_content   *p_conn_slave_device_info ,uint16_t dev_code )
{
    uint8_t ptl_id = 0;
    uint8_t ptl_num = p_tmp_ptl_gather->protocolNum;

    int32_t max_data_id = 0;
    int32_t tmp_data_id = 0;


    for ( ptl_id = 0 ; ptl_id <  g_protocl_num ; ptl_id++ )
    {

        if ( dev_code  ==    g_protocol[ptl_id].ptl_dev_code )
        {
            p_tmp_ptl_gather->p_comm_ptl[ ptl_num ]= & ( g_protocol[ ptl_id ] ) ;
            p_tmp_ptl_gather->protocolNum = ++ptl_num;

            tmp_data_id =  g_protocol[ ptl_id ].max_ptl_data_id;

            //printf ( " g_protocol:  dev_code %d \n",    g_protocol[ptl_id].ptl_dev_code );
            //printf ( "[%d].ptl_dev_code %d   dev_code %d \n",  ptl_id,  p_tmp_ptl_gather->p_comm_ptl[ ptl_num-1 ]->ptl_dev_code ,dev_code );

            if ( max_data_id < tmp_data_id )
            {
                max_data_id = tmp_data_id;  //同设备编码中最大的数据id
            }

        }

    }

    p_conn_slave_device_info->max_data_id = max_data_id;

//     printf ( "  p_conn_slave_device_info->max_data_id %d \n", p_conn_slave_device_info->max_data_id );

    return max_data_id;

}













/*******************************************************************************
 * Function       : xmlDevTypeInit
 * Author          : shiran
 * Date              : 2016.11.08
 * Description  : 设备类型初始化
 * Calls              :  com_device_type_object 四种设备类型结构初始化     COMTYPE = 1  ：串口设备   NETTYPE 网络设备  FIFOSERVER 管道写设备   FIFOCLIENT 管道读设备
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/


void xmlDevTypeInit ( void )
{




    com_device_type_object[FIFO_TRAN_SERVER  ].cmm_type = FIFO_TRAN_SERVER;
    com_device_type_object[FIFO_TRAN_SERVER  ].func = FuncType[  FIFO_TRAN_SERVER  ];
    com_device_type_object[FIFO_TRAN_SERVER  ].cmm_case_id  = FIFO_TRAN_SERVER;


    com_device_type_object[FIFO_TRAN_CLIENT  ].cmm_type = FIFO_TRAN_CLIENT;
    com_device_type_object[FIFO_TRAN_CLIENT  ].func =  FuncType[  FIFO_TRAN_CLIENT  ];
    com_device_type_object[FIFO_TRAN_CLIENT  ].cmm_case_id  = FIFO_TRAN_CLIENT;



    com_device_type_object[FIFOSERVER  ].cmm_type = FIFOSERVER;
    com_device_type_object[FIFOSERVER  ].func =  FuncType[  FIFOSERVER  ];
    com_device_type_object[FIFOCLIENT  ].cmm_case_id  = FIFOSERVER;


    com_device_type_object[FIFOCLIENT  ].cmm_type  = FIFOCLIENT;
    com_device_type_object[FIFOCLIENT  ].func = FuncType[  FIFOCLIENT  ];
    com_device_type_object[FIFOCLIENT  ].cmm_case_id  = FIFOCLIENT;


}



/*******************************************************************************
 * Function       : read_protocol_table
 * Author          : shiran
 * Date              : 2016.11.08
 * Description  : 读取协议table  中protocol文件名称，解析csv文件内容，更新增加协议总数量
 * Calls              :get_tab_from_csv  读取csv文件内容
 * Input          : xmlNodePtr tempNode,int32_t dev_type,  protocol_s *p_protocol ,int32_t  *p_ptl_num
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t  read_protocol_table ( xmlNodePtr tempNode,int32_t dev_code )
{


    xmlNodePtr propNodePtr ;

    char tmp_ptl_name[100] = { 0 };


    //查找属性
    propNodePtr = tempNode;

    xmlAttrPtr attrPtr = propNodePtr->properties;
    while ( attrPtr != NULL )
    {
        if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "id" ) )
        {
            xmlChar* szAttr =
                xmlGetProp ( propNodePtr,BAD_CAST "id" );

            xmlFree ( szAttr );

        }
        else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "ProtocolName" ) )
        {

            xmlChar* szAttr =
                xmlGetProp ( propNodePtr,BAD_CAST "ProtocolName" );

            bzero ( tmp_ptl_name,sizeof ( tmp_ptl_name ) );

            strcpy ( tmp_ptl_name , ( char * ) szAttr );

            xmlFree ( szAttr );
        }
        attrPtr = attrPtr->next;
    }


    char csvFile[100] = { 0 };
//  测试文件存在与否,如果不存在,继续重新执行

    sprintf ( csvFile,IEC61850ConfigPath"%s",tmp_ptl_name );

    printf ( "csvFile:%s\n",csvFile );

    if ( !DirExist ( csvFile ) )
    {
        fprintf ( stderr,"protocol_name %s file not exist---\n " ,tmp_ptl_name );
    }
    else
    {
        get_tab_from_xml ( dev_code, csvFile ) ;
    }

    return  0;


}

/*******************************************************************************
 * Function       : get_node_parameter_num
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : 根据xpath 路径解析xml文件中的节点数量
 * Calls              :
 * Input          :xmlXPathContextPtr context , const xmlChar *xpath
 * Output       :
 * Return        :
 ********************************************************************************/

static int32_t get_node_parameter_num ( xmlXPathContextPtr context , const xmlChar *xpath )
{

    xmlXPathObjectPtr result = NULL;
    xmlNodeSetPtr nodeset = NULL;
    int32_t ret_parameter_num = 0;

    if ( context == NULL )
    {
        printf ( "context is NULL\n" );
        return 0;
    }
    result = xmlXPathEvalExpression ( xpath, context );

    if ( result == NULL )
    {
        printf ( "xmlXPathEvalExpression return NULL\n" );
        return 0;
    }

    if ( xmlXPathNodeSetIsEmpty ( result->nodesetval ) )
    {

        xmlXPathFreeObject ( result );

        return 0;
    }
    nodeset = result->nodesetval;
    ret_parameter_num =  nodeset->nodeNr ; //计算有多少个设备节点
    xmlXPathFreeObject ( result );
    return ret_parameter_num;
}



/*******************************************************************************
 * Function       : get_node_parameter_num
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : 根据xpath 路径解析xml文件中的节点数量
 * Calls              :
 * Input          :xmlXPathContextPtr context , const xmlChar *xpath
 * Output       :
 * Return        :
 ********************************************************************************/

static int32_t get_all_slaver_nodes ( xmlXPathContextPtr context , const xmlChar *xpath )
{
    xmlNodePtr cur_node=NULL;
    xmlNodePtr Node2,Node3; //定义结点指针(你需要它为了在各个结点间移动),四级嵌套
    xmlXPathObjectPtr result = NULL;
    xmlNodeSetPtr nodeset = NULL;


    int32_t id  = 0;


    int32_t record_id  = 0;   //序号读取记录  上次读取的id 小于当期的读取序号 否则 跳出

    xmlNodePtr propNodePtr =NULL;

    if ( context == NULL )
    {
        printf ( "context is NULL\n" );
        return 0;
    }
    result = xmlXPathEvalExpression ( xpath, context );

    if ( result == NULL )
    {
        printf ( "xmlXPathEvalExpression return NULL\n" );
        return 0;
    }

    if ( xmlXPathNodeSetIsEmpty ( result->nodesetval ) )
    {

        xmlXPathFreeObject ( result );

        return 0;
    }



    nodeset = result->nodesetval;

    g_slave_dev_num =  nodeset->nodeNr ; //计算有多少个设备节点

    if ( g_slave_dev_num > 0 )
    {
        p_g_slave_device_info  = ( slave_device_config_content * ) malloc ( sizeof ( slave_device_config_content ) * ( g_slave_dev_num ) );

        memset ( p_g_slave_device_info,0,sizeof ( slave_device_config_content ) * ( g_slave_dev_num ) );
    }

    for ( id = 0; id < g_slave_dev_num ;  id++ )
    {
        cur_node =  nodeset->nodeTab[ id ];

        if ( ( !xmlStrcmp ( cur_node->name, ( const xmlChar * ) "slaver" ) ) ) //devtype 结点内容解析
        {

            printf ( "slaver ---%s n",cur_node->name );


            int32_t  slaver_idx = 0;

            int32_t tmp_dev_code = 0;

            propNodePtr = cur_node;
            xmlAttrPtr attrPtr = propNodePtr->properties;

            while ( attrPtr != NULL )
            {
                if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "id" ) )
                {
                    xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "id" );

                    slaver_idx = strtoul ( ( char * ) szAttr,NULL, 0 );

                    if ( record_id < slaver_idx )
                    {
                        record_id = slaver_idx;
                    }
                    else
                    {
                        fprintf ( stderr,"slaver id read config error _exit modbusSlave " );
                        exit ( 1 );
                    }



                    xmlFree ( szAttr );
                }

                attrPtr = attrPtr->next;
            }

            Node2 = cur_node->xmlChildrenNode;

            while ( Node2 != NULL )
            {
                if ( ( !xmlStrcmp ( Node2->name, ( const xmlChar * ) "name" ) ) )
                {

                    propNodePtr = Node2;
                    xmlAttrPtr attrPtr = propNodePtr->properties;
                    while ( attrPtr != NULL )
                    {
                        if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DevCode" ) )
                        {
                            xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "DevCode" );

                            tmp_dev_code = strtoul ( ( char * ) szAttr,NULL, 0 );

                            p_g_slave_device_info[ slaver_idx -1 ].dev_code = tmp_dev_code;

                            xmlFree ( szAttr );

                        }
                        else  if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DevName" ) )
                        {
                            xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "DevName" );

                            if ( strlen ( ( char * ) szAttr ) < 100 )
                            {
                                strcpy ( p_g_slave_device_info[ slaver_idx -1 ].dev_name, ( char * ) szAttr );
                            }

                            xmlFree ( szAttr );

                        }

                        attrPtr = attrPtr->next;
                    }

                    xmlChar *szKey = NULL; //临时字符串变量

                    szKey = xmlNodeGetContent ( Node2 );

                    if ( strlen ( ( char * ) szKey ) < 100 )
                    {
                        strcpy ( p_g_slave_device_info[ slaver_idx -1 ].dev_type_name, ( char * ) szKey );
                    }

                    xmlFree ( szKey );

                }
                else if ( ( !xmlStrcmp ( Node2->name, ( const xmlChar * ) "protocol" ) ) )
                {

                    Node3 = Node2->xmlChildrenNode;

                    while ( Node3 != NULL )
                    {

                        if ( ( !xmlStrcmp ( Node3->name, ( const xmlChar * ) "table" ) ) )
                        {

                            read_protocol_table ( Node3,tmp_dev_code );

                        }

                        Node3 = Node3->next;
                    }

                }

                Node2 = Node2->next;
            }
        }

    }

    xmlXPathFreeObject ( result );

    return 0;
}







/*******************************************************************************
 * Function       : read_slave_corr_index_table
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : 解析xml 设备节点 的设备modbus地址 和  设备序号，填充设备类型结构体 ，将 端口对象 的下挂设备的结构体指针 指向当前的设备类型结构体
 * Calls              :
 * Input          :xmlXPathContextPtr context, xmlNodePtr tempNode, uint16_t dev_code,int32_t table_num
 * Output       :
 * Return        :
 ********************************************************************************/
slave_device_config_content   *   read_slave_corr_index_table ( xmlXPathContextPtr context, xmlNodePtr tempNode, uint16_t dev_code,int32_t table_num )
{

    DEBUG_CONFIG ( " read_slave_corr_index_table \n " );

    slave_device_config_content   *p_tmp_info = NULL;

    xmlNodePtr propNodePtr =NULL ;

    int32_t mbm_slave_num = 0;
    int32_t mbm_slave_id = 0;

    int32_t index_id = 0;

    int32_t slave_dev_id = 0;

    int32_t dev_id = 0;


//设备类型数  g_slave_dev_num   设备类型结构体   p_g_slave_device_info

    for ( dev_id =  0 ; dev_id <  g_slave_dev_num ; dev_id ++ )
    {

        if ( dev_code  ==  p_g_slave_device_info[dev_id].dev_code )
        {

            if ( p_g_slave_device_info[dev_id].slave_flag == 1 )
            {
                p_tmp_info =  & ( p_g_slave_device_info[dev_id] ) ;
            }
            else
            {
                p_g_slave_device_info[dev_id].slave_flag = 1;

                if ( table_num > 0 )
                {
                    p_g_slave_device_info[dev_id].p_slaver_corr_index_arr= ( slaver_corr_index * ) malloc ( sizeof ( slaver_corr_index ) *table_num );
                    if ( p_g_slave_device_info[dev_id].p_slaver_corr_index_arr ==  NULL )
                    {
                        exit ( 1 );
                    }

                    memset ( p_g_slave_device_info[dev_id].p_slaver_corr_index_arr, 0 , sizeof ( slaver_corr_index ) *table_num );
                }
                else
                {
                    p_g_slave_device_info[dev_id].p_slaver_corr_index_arr = NULL;

                }
                p_tmp_info =& ( p_g_slave_device_info[dev_id] ) ;
            }
            break;
        }
    }



    //查找属性
    while ( tempNode != NULL )
    {

        if ( ( !xmlStrcmp ( tempNode->name, ( const xmlChar * ) "table" ) ) )
        {

            mbm_slave_id =   mbm_slave_num;
            mbm_slave_num ++;

            propNodePtr = tempNode;
            xmlAttrPtr attrPtr = propNodePtr->properties;
            while ( attrPtr != NULL )
            {
                if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DevId" ) )
                {

                    xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "DevId" );

                    DEBUG_CONFIG ( "DevId %s   --  ",szAttr );

                    index_id = strtoul ( ( char * ) szAttr, NULL, 0 );

                    xmlFree ( szAttr );
                }
                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "TSlaveID" ) )
                {
                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "TSlaveID" );

                    DEBUG_CONFIG ( "TSlaveID %s   --  ",szAttr );

                    slave_dev_id  = strtoul ( ( char * ) szAttr, NULL,    0 );


                    xmlFree ( szAttr );
                }

                attrPtr = attrPtr->next;

            }

            DEBUG_CONFIG ( " mbm_slave_num, table_num %d %d \n", mbm_slave_num, table_num );

            if ( mbm_slave_num <=  table_num )
            {
                p_tmp_info->p_slaver_corr_index_arr[ mbm_slave_id ].modbus_id = slave_dev_id;

                p_tmp_info->p_slaver_corr_index_arr[mbm_slave_id ].slaveIndex = index_id;

                p_tmp_info->slave_id_cor_num =mbm_slave_num  ;

            }
            else
            {
                DEBUG_FAULT ( "read_slave_corr_index_table read config  slave_id_cor_num error\n" );
                fprintf ( stderr,"read_slave_corr_index_table read config  slave_id_cor_num error\n" );
                break;

            }

        }
        tempNode =tempNode->next;
    }

    return    p_tmp_info;

}


/*******************************************************************************
 * Function       : sysconfig_content_init
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : XML文件读取
 * Calls              :
 * Input          :char* configfilePATH xml文件路径
 * Output       :
 * Return        :
 ********************************************************************************/
int sysconfig_content_init ( char* configfilePATH )
{
    xmlDocPtr doc; //定义解析文档指针
    xmlNodePtr curNode,Node1,Node2,Node3; //定义结点指针(你需要它为了在各个结点间移动),四级嵌套
    xmlXPathContextPtr context;

    char *szDocName   = configfilePATH;

    uint8_t   comm_port_idx  = 0;

    xmlDevTypeInit ( );

    doc = xmlReadFile ( szDocName,"UTF-8",XML_PARSE_RECOVER ); //解析文件
    //检查解析文档是否成功，如果不成功，libxml将指一个注册的错误并停止。
    //一个常见错误是不适当的编码。XML标准文档除了用UTF-8或UTF-16外还可用其它编码保存。
    //如果文档是这样，libxml将自动地为你转换到UTF-8。更多关于XML编码信息包含在XML标准中.
    if ( NULL == doc )
    {
        fprintf ( stderr,"Document not parsed successfully\n" );
        return -1;
    }

    context = xmlXPathNewContext ( doc );

    p_g_com_conn  = ( CommConnObj * ) malloc ( sizeof ( CommConnObj ) * FIFOCLIENT );
    if ( p_g_com_conn ==  NULL )
    {
        exit ( 1 );
    }

    memset ( p_g_com_conn,0, sizeof ( CommConnObj ) * FIFOCLIENT );

//  FIFO_TRAN_SERVER = 1,     //管道转发写设备
//  FIFO_TRAN_CLIENT,          //管道转发读设备
//  FIFOSERVER,	                       //管道写设备
//   FIFOCLIENT	                      //管道读设备

//初始化 设备类型     FIFO_TRAN_SERVER, 管道写设备  FIFO_TRAN_CLIENT 管道读设备    FIFOSERVER, 管道写设备  FIFOCLIENT 管道读设备

//client_fifo
    memset ( & ( p_g_com_conn[FIFO_TRAN_SERVER  ] ),0,sizeof ( CommConnObj ) *1 );

    memcpy ( & ( p_g_com_conn[FIFO_TRAN_SERVER  ] ),&com_device_type_object[FIFO_TRAN_SERVER   ],sizeof ( CommConnObj ) *1 );


//server_fifo
    memset ( & ( p_g_com_conn[FIFO_TRAN_CLIENT  ] ),0,sizeof ( CommConnObj ) *1 );

    memcpy ( & ( p_g_com_conn[FIFO_TRAN_CLIENT  ] ),&com_device_type_object[FIFO_TRAN_CLIENT  ],sizeof ( CommConnObj ) *1 );


    //server_fifo
    memset ( & ( p_g_com_conn[FIFOSERVER  ] ),0,sizeof ( CommConnObj ) *1 );

    memcpy ( & ( p_g_com_conn[FIFOSERVER  ] ),&com_device_type_object[FIFOSERVER ],sizeof ( CommConnObj ) *1 );


    //server_fifo
    memset ( & ( p_g_com_conn[FIFOCLIENT  ] ),0,sizeof ( CommConnObj ) *1 );

    memcpy ( & ( p_g_com_conn[FIFOCLIENT ] ),&com_device_type_object[FIFOCLIENT  ],sizeof ( CommConnObj ) *1 );





    xmlChar *  xpath2 = ( xmlChar * ) "/root/slaver";

    get_all_slaver_nodes ( context, xpath2 );

    curNode = xmlDocGetRootElement ( doc ); //确定文档根元素
    /*检查确认当前文档中包含内容*/
    if ( NULL == curNode )
    {
        fprintf ( stderr,"empty document\n" );
        xmlFreeDoc ( doc );
        return -1;
    }
    //在这个例子中，我们需要确认文档是正确的类型“root”是在这个示例中使用文档的根类型
    if ( xmlStrcmp ( curNode->name, BAD_CAST "root" ) )
    {
        fprintf ( stderr,"document of the wrong type, root node != root" );
        xmlFreeDoc ( doc );
        return -1;
    }
    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;

    Node1 = curNode;


    while ( Node1 != NULL )
    {


        //commport 结点内容解析

        int32_t record_mbm_id = 0;   //定义序号mbm_slaver 顺序记录状态


        char xml_comm_mbmslaver_path[100] = { 0 };

        sprintf ( xml_comm_mbmslaver_path,"/root/commport/mbmslaver" );


        int32_t mbm_slave_n =0;  //端口链接对象下的mbmslaver 的设备总数



        mbm_slave_n =     get_node_parameter_num ( context , BAD_CAST xml_comm_mbmslaver_path );

        DEBUG_CONFIG ( "xml_comm_mbmslaver_path %s __端口链接对象下的mbmslaver 的设备总数 : mbm_slave_n : %d \n",xml_comm_mbmslaver_path,mbm_slave_n );

        p_g_com_conn[ FIFO_TRAN_CLIENT ].mbm_slaver_num = mbm_slave_n;

        p_g_com_conn[ FIFOCLIENT ].mbm_slaver_num = mbm_slave_n;

        Node2 = Node1->xmlChildrenNode;

        while ( Node2 != NULL )
        {

            if ( ( !xmlStrcmp ( Node2->name, ( const xmlChar * ) "mbmslaver" ) ) )
            {
                Node3 = Node2->xmlChildrenNode;
                uint16_t dev_code = 0;
                int32_t mbm_slaver_id = 0;

                propNodePtr = Node2;

                xmlAttrPtr slaver_attr_ptr = propNodePtr->properties;  //节点属性

                while ( slaver_attr_ptr != NULL )
                {

                    if ( !xmlStrcmp ( slaver_attr_ptr->name, BAD_CAST "id" ) )
                    {

                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "id" );

                        mbm_slaver_id= strtoul ( ( char * ) szAttr,NULL, 0 );

                        DEBUG_CONFIG ( "mbm_slaver_id : %d   record_mbm_id %d \n",mbm_slaver_id,record_mbm_id );

                        if ( record_mbm_id < mbm_slaver_id )
                        {
                            record_mbm_id = mbm_slaver_id;
                        }
                        else
                        {
                            fprintf ( stderr,"mbm_slaver_id id read config error _exit modbusSlave " );
                            exit ( 1 );
                        }

                        xmlFree ( szAttr );


                    }
                    else   if ( !xmlStrcmp ( slaver_attr_ptr->name, BAD_CAST "DevCode" ) )
                    {


                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DevCode" );

                        dev_code= strtoul ( ( char * ) szAttr,NULL, 0 );

                        DEBUG_CONFIG ( "dev_code : %d \n",dev_code );
			
			//-------------------------------------------------------------

                        p_g_com_conn[ FIFO_TRAN_CLIENT ].one_reg_dev_code = dev_code;

                        p_g_com_conn[ FIFOCLIENT ].one_reg_dev_code= dev_code;

                        xmlFree ( szAttr );
                    }

                    slaver_attr_ptr =    slaver_attr_ptr->next ;

                }


                char  mbmslaver_tab_path[100]= {0};

                int32_t table_n = 0;


                sprintf ( mbmslaver_tab_path ,"%s[ @DevCode='%d'  ]/table",xml_comm_mbmslaver_path,dev_code );

                table_n =   get_node_parameter_num ( context , BAD_CAST mbmslaver_tab_path );

                //读取对应关系表	  每个端口下 挂 最多10 个设备类型
                p_g_com_conn[ comm_port_idx ].p_mbm_slaver_config[ mbm_slaver_id-1 ] =   read_slave_corr_index_table ( context,Node3   ,dev_code,table_n );


                //初始化 端口链接对象 协议
                search_init_conn_protocol ( & ( p_g_com_conn[ FIFO_TRAN_CLIENT ].comm_protocol_gather )  ,p_g_com_conn[ FIFO_TRAN_CLIENT ].p_mbm_slaver_config[ mbm_slaver_id-1 ],dev_code );

                //初始化 端口链接对象 协议
                search_init_conn_protocol ( & ( p_g_com_conn[ FIFOCLIENT ].comm_protocol_gather )  ,p_g_com_conn[ FIFOCLIENT ].p_mbm_slaver_config[ mbm_slaver_id-1 ],dev_code );

                DEBUG_CONFIG ( "  p_g_com_conn[%d].p_mbm_slaver_config[ %d-1 ] max_data_id :%d\n ",  comm_port_idx ,mbm_slaver_id, p_g_com_conn[ comm_port_idx ].p_mbm_slaver_config[ mbm_slaver_id-1 ]->max_data_id );


            }

            Node2 = Node2->next;
        }



        Node1 = Node1->next;

    }





//修改 配置文件
    // xmlCommportConfig ( context );


    //保存修改配置文件 2018-04-19 去除xml文件保存
    //xmlSaveFile ( configfilePATH, doc );
    printf ( "\n----------------xmlXPathFreeContext-----------------------------\n" );


    xmlXPathFreeContext ( context );

//   printf ( "\n----------------xmlXPathFreeContext----end-------------------------\n" );



    xmlFreeDoc ( doc );

    printf ( "\n----------------xmlFreeDoc----doc-------------------------\n" );

    //写入配置文件 版本号
    xml_write_FirmwareVersion_config ( XML_VERSION_PATH );


    return 0;
}



/*******************************************************************************
* Function       : xml_write_FirmwareVersion_config
* Author          : shiran
* Date              : 2016.06.23
* Description  : 配置写入xml版本信息
* Calls              :
* Input             : char *pzaname 文件名称
* Output          :
* Return           : 0
********************************************************************************/
static int xml_write_FirmwareVersion_config ( char* szDocName )
{


    xmlNodeSetPtr nodeset;
    xmlXPathObjectPtr result;
    xmlXPathContextPtr context;
    xmlDocPtr doc = NULL;              //定义解析文档指针
    xmlNodePtr CurrentNode = NULL;
    int i = 0;

//   char* szDocName = pzaname;  //保存xml文档名

    doc = xmlReadFile ( szDocName, "utf-8", XML_PARSE_RECOVER ); //解析文档
    if ( NULL == doc )
    {

        fprintf ( stderr, "Document not parsed successfully.\n" );
        xmlFreeDoc ( doc );

        return -1;
    }


    context = xmlXPathNewContext ( doc );

    xmlChar *xpath0 =BAD_CAST "/root/FirmwareVersion";

    result = xmlXPathEvalExpression ( xpath0, context );

    nodeset = result->nodesetval;

    for ( i = 0; i < nodeset->nodeNr ;  i++ )
    {

        CurrentNode =  nodeset->nodeTab[i];

        xmlNodeSetContent ( CurrentNode, BAD_CAST soft_ver );

    }


    xmlSaveFile ( szDocName, doc );


    xmlXPathFreeObject ( result );

    xmlXPathFreeContext ( context );

    xmlFreeDoc ( doc );

    return 0;
}



/************************************************************************
*                   局部函数
*************************************************************************/

/*******************************************************************************
 * Function       : insert_slave_config
 * Author          : shiran
 * Date              : 2016.11.10
 * Description  : 根据全局设备类型指向端口下挂接的设备类型
 * Calls              :
 * Input             : slave_device_config_content   *p_slave_device, int32_t dev_code
 * Output          :
 * Return           :
 ********************************************************************************/

void insert_slave_config ( slave_device_config_content   *p_slave_device, int32_t dev_code )
{
    int32_t slave_id = 0;

    for ( slave_id = 0 ;   slave_id < g_slave_dev_num ;   slave_id++ )
    {
        if ( p_g_slave_device_info[ slave_id ].dev_code == dev_code )
        {
            p_slave_device=&p_g_slave_device_info[ slave_id ];
        }

    }
}



/*******************************************************************************
 * Function       : get_element_from_str
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : 使用逗号分隔符解析字符串,
 * Calls              :
 * Input             : char *buf,字符串,char separator 分隔符
 * Output          :char *element 返回字段
 * Return           :字符串位置
 ********************************************************************************/
uint16_t get_element_from_str ( char *buf,char *element,char separator )
{
    uint32_t i = 0;
    for ( i = 0; ( buf[i]!= separator ) && ( buf[i]!= '\0' ) && ( i< 127 ); ++i )
    {
        element[i] = buf[i];
    }
    element[i] = '\0';
//     return i;

    if ( element[i-1] == '\r' )
    {
        element[i-1] = '\0';
    }
    return ( i+1 );

}


#if 0
/*******************************************************************************
 * Function       : get_id_from_names
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : 根据data_name 获取ID ,
 * Calls              :
 * Input             :char *element
 * Output          :
 * Return           :id序号或者0
 ********************************************************************************/


static uint16_t get_id_from_names ( char *element )
{
    uint32_t i = 0;
    for ( i = 0; i < DATA_TOTEL; ++i )
    {
        if ( 0 == strcmp ( element,data_names[i] ) )
        {
            return i;
        }
    }
    return 0;
}



static int32_t get_headline_from_csv ( int32_t context_position ,int32_t line_num,int32_t list_num, char *p_csv_context )
{

    char tmp_element[128] =  { 0 };
    int32_t csv_context_position_record= context_position;

    int32_t line_id = 0;
    int32_t list_id = 0;

    for ( line_id = 0; line_id < line_num ; line_id++ )
    {
//         printf ( " headLine %d:  \n ", line_id );
        for ( list_id = 0; list_id < list_num ; list_id ++ )
        {
            memset ( tmp_element,0,sizeof ( tmp_element ) );
            csv_context_position_record +=  get_element_from_str ( &p_csv_context[csv_context_position_record],tmp_element,',' );
            //   printf ( " %s ",tmp_element );
        }
        printf ( "  \n " );
    }

    return csv_context_position_record;


}

#endif



int8_t  get_xml_point_data_sign_config ( char *p_sign_element );
int8_t  get_xml_point_data_type_config ( char *p_data_element );
int8_t  get_xml_point_data_type_context ( int8_t sign_flag, int8_t data_type_flag );


int8_t  get_xml_point_data_sign_config ( char *p_sign_element )
{
    int8_t ret  = 0;
    if ( strchr ( p_sign_element,'U' ) )
    {
        ret = SIGN_U;
    }
    else if ( strchr ( p_sign_element,'S' ) )
    {

        ret = SIGN_S;
    }
    else
    {
        ret  = -1;
    }

    return ret;

}


int8_t  get_xml_point_data_type_config ( char *p_data_element )
{


    int8_t ret  = 0;
    if ( strchr ( p_data_element,'I' ) )
    {
        ret = TYPE_INT;
    }
    else if ( strchr ( p_data_element,'F' ) )
    {

        ret = TYPE_F;
    }
    else
    {
        ret  = -1;
    }

    return ret;


}


// #define FLOAT_T 0  //浮点
// #define U_INT_T 1  //无符号数
// #define S_INT_T 2  //有符号数

int8_t  get_xml_point_data_type_context ( int8_t sign_flag, int8_t data_type_flag )
{
    //SIGN_U,SIGN_S,TYPE_INT,TYPE_F,
    int8_t ret  = 0;

    if ( ( SIGN_U ==   sign_flag ) && ( TYPE_INT ==  data_type_flag ) )
    {
        ret = U_INT_T ;
    }
    else   if ( ( SIGN_S ==   sign_flag ) && ( TYPE_INT ==  data_type_flag ) )
    {
        ret = S_INT_T ;
    }
    else   if ( TYPE_F ==  data_type_flag )
    {
        ret  = FLOAT_T ;
    }
    else
    {
        ret =  -1;
    }

    return ret;

}





//xpath寻找节点
xmlXPathObjectPtr getNodeset ( xmlDocPtr doc, const xmlChar *xpath )
{
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;
    context = xmlXPathNewContext ( doc );

    if ( context == NULL )
    {
        printf ( "context is NULL\n" );
        return NULL;
    }

    result = xmlXPathEvalExpression ( xpath, context );
    xmlXPathFreeContext ( context );
    if ( result == NULL )
    {
        printf ( "xmlXPathEvalExpression return NULL\n" );
        return NULL;
    }

    if ( xmlXPathNodeSetIsEmpty ( result->nodesetval ) )
    {
        xmlXPathFreeObject ( result );
        printf ( "%s nodeset is empty\n",xpath );
        return NULL;
    }
    return result;
}

//修改modbusSlaver进程配置文件pc端口的波特率和modbus站号
int modify_mbslave_dev ( char *configfilePATH,uint16_t *baud,uint8_t *id )
{
    //printf("client 初始化开始---------------------------------------\n");
    xmlDocPtr doc; //定义解析文档指针
    xmlNodePtr Node1,Node2; //定义结点指针(你需要它为了在各个结点间移动)
    //xmlChar *szKey; //临时字符串变量
    //xmlNodeSetPtr nodeset;
    xmlXPathObjectPtr app_result;
    xmlChar *xpath = NULL;
    uint16_t i = 0;

    doc = xmlReadFile ( configfilePATH,"UTF-8",XML_PARSE_RECOVER ); //解析文件
    if ( NULL == doc )
    {
        fprintf ( stderr,"Document not parsed successfully\n" );
        DEBUG_FAULT ( "Document not parsed successfully\n" );
        return -1;
    }
    else
    {
        //printf("Open Xml success!\n");
    }

    xpath = BAD_CAST ( "/root/commport[@commdef='PC']" );
    app_result = getNodeset ( doc, ( const xmlChar * ) xpath );

    if ( app_result )
    {

        xmlNodeSetPtr nodeset = app_result->nodesetval;

        //解析节点内容
        for ( i=0; i < nodeset->nodeNr; i++ )
        {
            Node1 = nodeset->nodeTab[i]->xmlChildrenNode;
            while ( Node1 != NULL )
            {
                if ( !xmlStrcmp ( Node1->name, ( const xmlChar * ) "uartbaud" ) )
                {
                    if ( baud != NULL )
                    {
                        char buf[8] = {0};
                        sprintf ( buf,"%d",*baud );

                        DEBUG_CONFIG ( "uartbaud set Prop %s",buf );

                        xmlNodeSetContent ( Node1, ( const xmlChar * ) buf );
                    }
                }
                else if ( !xmlStrcmp ( Node1->name, ( const xmlChar * ) "mbmslaver" ) )
                {
                    Node2 = Node1->xmlChildrenNode;
                    while ( Node2 != NULL )
                    {
                        if ( !xmlStrcmp ( Node2->name, ( const xmlChar * ) "table" ) )
                        {
                            xmlNodePtr propNodePtr = Node2;
                            xmlAttrPtr attrPtr = propNodePtr->properties;
                            while ( attrPtr != NULL )
                            {
                                if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "TSlaveID" ) )
                                {
                                    if ( id != NULL )
                                    {
                                        char buf[8] = {0};
                                        sprintf ( buf,"%d",*id );

                                        DEBUG_CONFIG ( "TSlaveID set Prop %s",buf );

                                        xmlSetProp ( propNodePtr,BAD_CAST "TSlaveID", ( xmlChar * ) buf );
                                    }
                                }
                                attrPtr = attrPtr->next;
                            }
                        }
                        Node2 = Node2->next;
                    }
                }
                Node1 = Node1->next;
            }
        }
        xmlXPathFreeObject ( app_result );
    }
    xmlSaveFile ( configfilePATH, doc );
    system ( "sync" );

    DEBUG_CONFIG ( "xmlSaveFile sync configfilePATH %s \n ",configfilePATH );

    xmlFreeDoc ( doc );
    return 0;
}
