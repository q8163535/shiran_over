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
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "../../io.h"
#include "protocol.h"
#include "protocol_cfg.h"

#include "xml_csv.h"
#include "init.h"
#include "xmlAddressAnalysis.h"
#include "debugSwitch.h"
#include "dataProcess.h"
#include "xml_point_list_analysis.h"


#include "xml_create_export_csv.h"

#include "IEC104RTU.h"
/************************************************************************
*                    宏定义-常量定-静态全局变量
*************************************************************************/

#define  MAXNETPORTNUM          (int32_t )5
#define  SINGLECOMMAXLINK      (int32_t)5
#define  MAXBACKGROUND          (int32_t)10

#define NET_INITIALIZE_NUM (int32_t)2      //网口数量

#define XML_VERSION_PATH   "/home/SGLogger/Config/AppVer_IEC104Slave.xml"
/************************************************************************
*                   全局变量
*************************************************************************/
int32_t  g_dev_case_num ;                      //dev 实例数量;
dev_protocol_gather *dev_ptl_gather;  //设备协议结构体


total_protocol_gather *total_bg_ptl;    //总协议 结构体

int32_t g_totol_bg_ptl_num ;                //总协议数量

int32_t g_dev_type_update_flag  ;

CommConnObj  GlobalComMasterConn[4]; //端口链接对象结构体指针


mbm_slaver_uuid_info  uuid_info_gather;   //uuid_info_gather  全局数据 uuid映射


int32_t measure_yk_yx_control_type    =  IEC_DP_TYPE ;  //遥信 遥控 单点 双点配置  默认 双点

int32_t soe_update_config; //SOE 变位上传
int32_t cos_update_config; //COS 变位上传



int32_t yc_interval = 500 ;     //遥测数据上传间隔  默认 300ms
int32_t yc_period =   5000;    //遥测数据上传周期 默认 3s
int32_t yx_interval = 0;
int32_t yx_period   = 0;
int32_t yc_period_record_time = 6;


/************************************************************************
*                   局部函数原型（声明）
*************************************************************************/
static uint32_t get_dev_attr_config ( xmlNodePtr  node_tmp,dev_protocol_gather  *p_dev_context );

static uint32_t get_total_protocl_attr_config ( xmlNodePtr  node_tmp,total_protocol_gather  *p_totoal_context );
uint16_t get_element_from_str ( char *buf,char *element,char separator );

// static uint16_t get_id_from_names ( char *element );

//static uint32_t get_tab_from_csv ( dev_protocol *p_dev_ptl, char *p_protocol_name, int32_t measure_type_data );

static int xml_write_FirmwareVersion_config ( char *pzaname );

int32_t get_measure_type ( char * measure_type_name );
static uint32_t get_tab_from_total_csv ( total_protocol_gather *p_protocol_gather_case   ,char *p_protocol_name, int32_t measure_type_data );


/************************************************************************
*                   全局函数
*************************************************************************/





int32_t get_headline_from_csv ( int32_t context_position ,int32_t line_num,int32_t list_num, char *p_csv_context )
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



/*******************************************************************************
 * Function       : commport_attr_analysis
 * Author          : shiran
 * Date              : 2016.08.10
 * Description  : XML commport 节点属性读取
 * Calls              :
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t commport_attr_analysis ( xmlNodePtr  node_tmp )
{

    xmlNodePtr  propNodePtr =NULL;

    int32_t net_id = -1 ;

    //查找属性
    propNodePtr = node_tmp;
    xmlAttrPtr node_tmp_attr_ptr = propNodePtr->properties;
    while ( node_tmp_attr_ptr != NULL )
    {

        if ( !xmlStrcmp ( node_tmp_attr_ptr->name, BAD_CAST "netid" ) )
        {
            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "netid" );

            net_id = strtoul ( ( char * ) szAttr,NULL, 0 );

            printf ( "  --  commport_attr_analysis net_id   %d  \n   ",net_id );

            xmlFree ( szAttr );
        }

        node_tmp_attr_ptr = node_tmp_attr_ptr->next;

    }

    return net_id;


}





/*******************************************************************************
 * Function       : background_attr_analysis
 * Author          : shiran
 * Date              : 2016.08.10
 * Description  : XML background 节点属性读取
 * Calls              :
 * Input          :
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t background_attr_analysis ( xmlNodePtr  node_tmp  , background_link *p_background_dev )
{

    int32_t background_id = 0;

    char   allow_link_ip_name[30] = { 0 };
    char  ptl_global_name[50] = { 0 };
    int32_t net_port  = 0;

    
        int32_t allow_client_link_n = 1 ;

	
	
    //查找属性
    xmlNodePtr  propNodePtr = node_tmp;

    xmlAttrPtr node_tmp_attrPtr = propNodePtr->properties;

    while ( node_tmp_attrPtr != NULL )
    {

        if ( !xmlStrcmp ( node_tmp_attrPtr->name, BAD_CAST "id" ) )
        {
            xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "id" );

            printf ( "background_support id %s   --  ",szAttr );

            background_id  = strtoul ( ( char * ) szAttr, NULL, 0 );

            if ( background_id > SINGLECOMMAXLINK )
            {
                break;
            }

            xmlFree ( szAttr );
        }
        else if ( !xmlStrcmp ( node_tmp_attrPtr->name, BAD_CAST "port" ) )
        {
            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "port" );
            net_port = strtoul ( ( char * ) szAttr,NULL, 0 );
            printf ( "port %d \n",net_port );

            xmlFree ( szAttr );
        }
        else if ( !xmlStrcmp ( node_tmp_attrPtr->name, BAD_CAST "allow_link_ip" ) )
        {
            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "allow_link_ip" );

            strcpy ( allow_link_ip_name , ( char * ) szAttr );

            xmlFree ( szAttr );
        }
        else if ( !xmlStrcmp ( node_tmp_attrPtr->name, BAD_CAST "allow_client_link" ) )
        {
            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "allow_client_link" );

            allow_client_link_n = strtoul ( ( char * ) szAttr,NULL, 10 );
            xmlFree ( szAttr );
	    
        }
        else if ( !xmlStrcmp ( node_tmp_attrPtr->name, BAD_CAST "protocol_tar_name" ) )
        {
            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "protocol_tar_name" );
            strcpy ( ptl_global_name , ( char * ) szAttr );

            xmlFree ( szAttr );
        }

        node_tmp_attrPtr = node_tmp_attrPtr->next;
    }

    printf ( "background_id  %d port %d allow_link_ip_name %s ptl_global_name %s \n", background_id ,net_port,  allow_link_ip_name,ptl_global_name );

    strcpy ( p_background_dev[background_id -1 ].allow_ip,allow_link_ip_name );
    strcpy ( p_background_dev[background_id -1 ].protocol_global_name,ptl_global_name );


    if ( 0 == strncmp ( allow_link_ip_name ,"0.0.0.0",7 ) )
    {
        p_background_dev[background_id -1 ].allow_all_flag = 1;
    }
    else
    {
        p_background_dev[background_id -1 ].allow_all_flag = 0;
    }


    p_background_dev[background_id -1 ].background_id = background_id;
    
    p_background_dev[background_id -1 ].netport =net_port;

    
    p_background_dev[background_id -1 ].bg_allow_client_link_n = allow_client_link_n;
    
    
    return background_id;
}



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



void init_comm_conn ( void )
{
    int32_t net_id = 0;
    int32_t bg_id = 0;
    int32_t measure_type_id = 0;
    int32_t ptl_id = 0;
    int32_t bg_num = 0;

    printf ( "g_totol_bg_ptl_num %d \n",g_totol_bg_ptl_num );


    for ( ptl_id = 0; ptl_id < g_totol_bg_ptl_num ; ptl_id ++ )
    {

        for ( measure_type_id  =0  ; measure_type_id < 5 ; measure_type_id ++ )
        {

            get_tab_from_total_csv ( & ( total_bg_ptl[ptl_id] ), total_bg_ptl[ptl_id].totol_name,measure_type_id );

        }
    }



    int32_t background_dir_exist_flag = 0;

    for ( net_id = 0; net_id < g_net_case_num ; net_id ++ )
    {
        bg_num = GlobalComMasterConn[net_id].background_num;

        if ( bg_num  > 0 )
        {

            background_dir_exist_flag = 0;

            for ( bg_id = 0 ; bg_id < bg_num ; bg_id++ )
            {
                for ( ptl_id = 0; ptl_id < g_totol_bg_ptl_num ; ptl_id ++ )
                {

                    if ( strcmp ( GlobalComMasterConn[net_id].background_dev[bg_id ].protocol_global_name, total_bg_ptl[ptl_id].totol_name ) == 0 )
                    {

                        background_dir_exist_flag = 1;

                        for ( measure_type_id  = 0  ; measure_type_id < 5 ; measure_type_id ++ )
                        {

                            GlobalComMasterConn[net_id].background_dev[ bg_id ].bgd_protocol[measure_type_id] = total_bg_ptl[ ptl_id ].total_ptl_context[measure_type_id].csv_ptl ;


                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_adjust_begin_addr = total_bg_ptl[ ptl_id].remote_adjust_begin_addr ;
                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_adjust_end_addr = total_bg_ptl[ ptl_id].remote_adjust_end_addr ;

                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_control_begin_addr = total_bg_ptl[ ptl_id].remote_control_begin_addr ;
                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_control_end_addr = total_bg_ptl[ ptl_id].remote_control_end_addr ;

                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_mesure_begin_addr = total_bg_ptl[ ptl_id].remote_mesure_begin_addr ;
                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_mesure_end_addr = total_bg_ptl[ ptl_id].remote_mesure_end_addr ;

                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_pluse_begin_addr = total_bg_ptl[ ptl_id].remote_pluse_begin_addr ;
                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_pluse_end_addr = total_bg_ptl[ ptl_id].remote_pluse_end_addr ;

                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_signal_begin_addr = total_bg_ptl[ ptl_id].remote_signal_begin_addr ;
                            GlobalComMasterConn[net_id].background_dev[ bg_id ].remote_signal_end_addr = total_bg_ptl[ ptl_id].remote_signal_end_addr ;
                        }

                    }


                }
            }

            if ( background_dir_exist_flag  == 0 )
            {
                fprintf ( stderr,"Comm Connect  protocol_global_name  not exist\n" );
                exit ( 1 );
            }


        }

    }
}

/*******************************************************************************
 * Function       : get_protocol_tar_content_node
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : XML文件读取
 * Calls              :
 * Input          :char* configfilePATH xml文件路径
 * Output       :
 * Return        :
 ********************************************************************************/

int32_t  get_protocol_tar_content_node ( xmlXPathContextPtr context )
{

    xmlNodePtr tmp_node=NULL;
    xmlXPathObjectPtr result = NULL;
    xmlNodeSetPtr nodeset = NULL;

    int32_t ret_parameter_num = 0;

    int32_t  node_id = 0;

    int32_t  total_ptl_id  = 0;

    int32_t node_num = 0;

    char total_ptl_name[100] = { 0 };

    xmlChar *xpath_ptl_total_context = ( xmlChar * ) "/root/protocol_tar_content";

    printf ( "get_protocol_tar_content_node \n" );

    result = xmlXPathEvalExpression ( xpath_ptl_total_context, context );

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

    node_num =  nodeset->nodeNr ;

    g_totol_bg_ptl_num = node_num;

    total_bg_ptl = ( total_protocol_gather * ) data_safe_distribution ( sizeof ( total_protocol_gather ) *node_num,"total_bg_ptl   :" )  ;

    memset ( total_bg_ptl , 0,sizeof ( total_protocol_gather ) *node_num );



    for ( node_id = 0; node_id < nodeset->nodeNr ; node_id++ )
    {

        tmp_node =  nodeset->nodeTab[  node_id ];

        if ( !xmlStrcmp ( tmp_node->name,BAD_CAST "protocol_tar_content" ) )
        {

            //查找属性
            xmlNodePtr  propNodePtr =NULL;
            propNodePtr = tmp_node;
            xmlAttrPtr node_attr_ptr = propNodePtr->properties;
            while ( node_attr_ptr != NULL )
            {

                if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "id" ) )
                {
                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "id" );


                    total_ptl_id = strtoul ( ( char * ) szAttr,NULL, 0 );

                    printf ( "  -- protocol_tar_content  id   %s      \n   ",szAttr );

                    xmlFree ( szAttr );
                }
                else    if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "name" ) )
                {
                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "name" );

                    strcpy ( total_ptl_name, ( char * ) szAttr );

                    printf ( "  -- protocol_tar_content  name   %s      \n   ",szAttr );

                    xmlFree ( szAttr );
                }

                node_attr_ptr = node_attr_ptr->next;

            }

            if ( total_ptl_id > 0 )
            {
                printf ( " total_bg_ptl[total_ptl_id -1 ].totol_name   %s      \n   ",total_bg_ptl[total_ptl_id -1 ].totol_name );
                strcpy ( total_bg_ptl[total_ptl_id -1 ].totol_name,total_ptl_name );
                get_total_protocl_attr_config ( tmp_node,&total_bg_ptl[total_ptl_id -1 ] );


            }

        }
    }

    xmlXPathFreeObject ( result );
    return ret_parameter_num;

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
static int xml_write_dev_type_update_flag_config ( char *pzaname )
{


    xmlNodeSetPtr nodeset;
    xmlXPathObjectPtr result;
    xmlXPathContextPtr context;
    xmlDocPtr doc = NULL;              //定义解析文档指针
    xmlNodePtr dev_node = NULL;
    int i = 0;

    char* szDocName = pzaname;  //保存xml文档名

    doc = xmlReadFile ( szDocName, "utf-8", XML_PARSE_RECOVER ); //解析文档
    if ( NULL == doc )
    {

        fprintf ( stderr, "Document not parsed successfully.\n" );
        xmlFreeDoc ( doc );

        return -1;
    }


    context = xmlXPathNewContext ( doc );

    xmlChar *xpath0 =BAD_CAST "/root/devtype";

    result = xmlXPathEvalExpression ( xpath0, context );

    nodeset = result->nodesetval;

    for ( i = 0; i < nodeset->nodeNr ;  i++ )
    {

        dev_node =  nodeset->nodeTab[i];

        xmlSetProp ( dev_node, ( xmlChar * ) "update_flag", ( xmlChar * ) "0" );

    }


    xmlSaveFile ( pzaname, doc );


    xmlXPathFreeObject ( result );

    xmlXPathFreeContext ( context );

    xmlFreeDoc ( doc );

    return 0;
}


/*******************************************************************************
 * Function       : get_remote_addr
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : get_remote_addr
 * Calls              :
 * Input             :
 * Output          :
 * Return           :void
 ********************************************************************************/
void get_remote_addr ( int32_t measure_id,uint16_t *p_begin_addr, uint16_t * p_end_addr )
{


    if ( measure_id ==  YC_104 )
    {
        // 遥测地址
        printf ( "  --遥测  YC_104     \n   " );

        *p_begin_addr =    REMOTE_MESURE_BEGIN_ADDR  ;  //   0x4001
        *p_end_addr    =    REMOTE_MESURE_END_ADDR     ;  //  0x4036
    }
    else  if ( measure_id ==  YK_104 )
    {
        // 遥控地址
        printf ( "  --遥控  YK_104     \n   " );

        *p_begin_addr =     REMOTE_CONTROL_BEGIN_ADDR  ;  //   0x6001
        *p_end_addr =  REMOTE_CONTROL_END_ADDR    ;  //    0x6007

    }
    else  if ( measure_id ==  YT_104 )
    {
        // 浮点遥调地址
        printf ( "  --遥调  YT_104     \n   " );

        *p_begin_addr =       REMOTE_ADJUST_BEGIN_ADDR    ;  //  0x6201
        *p_end_addr	  =       REMOTE_ADJUST_END_ADDR       ;  //  0x6208
    }
    else  if ( measure_id ==  YX_104 )
    {
        // 遥信地址
        printf ( "  --遥信  YX_104     \n   " );

        *p_begin_addr =       REMOTE_SIGNAL_BEGIN_ADDR  ;  //  0x0001
        *p_end_addr    =       REMOTE_SIGNAL_END_ADDR    ;  //    0x14d
    }
    else  if ( measure_id ==  YM_104 )
    {
// 累计量 遥脉
        printf ( "  --遥脉  YM_104     \n   " );

        *p_begin_addr =      REMOTE_PLUSE_BEGIN_ADDR    ;  //   0x6401
        *p_end_addr    =      REMOTE_PLUSE_END_ADDR       ;  //   0x6415

    }
}

/*******************************************************************************
 * Function       : protocol_tar_content_csv_create_init
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : protocol_tar_content_csv_create_init
 * Calls              :
 * Input             :
 * Output          :
 * Return           :void
 ********************************************************************************/
int32_t  protocol_tar_content_csv_create_init ( void )
{

    uint16_t begin_addr = 0;
    uint16_t end_addr = 0;

    int32_t  measure_id  = 0;
    int32_t dev_case_id = 0;

//    char total_ptl_name[100] = { 0 };
    //  char total_ptl_dir_name[100] = { 0 };

    char total_ptl_name_owner[100] = { 0 };
    char total_ptl_dir_name_owner[100] = { 0 };

    char total_ptl_name_export[100] = { 0 };
    char total_ptl_dir_name_export[100] = { 0 };


    // IEC104TOTALPROTOCOL_EXPORT



    if ( strcmp ( total_bg_ptl[ 0  ].totol_name,"IEC104_total_point" ) == 0 )
    {
        sprintf ( total_ptl_dir_name_owner ,IEC104TOTALPROTOCOL_OWNER"%s",total_bg_ptl[ 0  ].totol_name );

        sprintf ( total_ptl_dir_name_export ,IEC104TOTALPROTOCOL_EXPORT"%s",total_bg_ptl[ 0  ].totol_name );


        printf ( "total_ptl_dir_name %s \n", total_ptl_dir_name_owner );

        if ( !DirExist ( total_ptl_dir_name_owner ) )
        {
            mkdir_ex ( total_ptl_dir_name_owner );
        }

        printf ( "total_ptl_dir_name_export %s \n", total_ptl_dir_name_export );
        if ( !DirExist ( total_ptl_dir_name_export ) )
        {
            mkdir_ex ( total_ptl_dir_name_export );
        }


        for ( measure_id = 0 ; measure_id < 5 ; measure_id ++ )
        {

            get_remote_addr ( measure_id,&begin_addr, &end_addr );

            uint16_t address =begin_addr ;

            sprintf ( total_ptl_name_owner , "%s/%s",total_ptl_dir_name_owner, total_bg_ptl[ 0  ] .total_ptl_context[ measure_id ].measure_prtocol_name );

            sprintf ( total_ptl_name_export , "%s/%s",total_ptl_dir_name_export, total_bg_ptl[ 0  ] .total_ptl_context[ measure_id ].measure_prtocol_name );


            printf ( "total_ptl_name_owner %s \n", total_ptl_name_owner );

            printf ( "total_ptl_dir_name_export %s \n", total_ptl_dir_name_export );

            FILE *owner_fp = NULL ;




            owner_fp=fopen ( total_ptl_name_owner,"w+" );
            if ( NULL==owner_fp )
            {
                return -1;
            }
            fseek ( owner_fp, 0, SEEK_SET );

            sprintf ( total_ptl_name_owner , "%s/%s",total_ptl_dir_name_owner, total_bg_ptl[ 0  ] .total_ptl_context[ measure_id ].measure_prtocol_name );

            printf ( "total_ptl_name_owner %s \n", total_ptl_name_owner );




            FILE *export_fp = NULL ;
            export_fp=fopen ( total_ptl_name_export,"w+" );
            if ( NULL==export_fp )
            {
                return -1;
            }
            fseek ( export_fp, 0, SEEK_SET );

            sprintf ( total_ptl_name_export , "%s/%s",total_ptl_dir_name_export, total_bg_ptl[ 0  ] .total_ptl_context[ measure_id ].measure_prtocol_name );

            printf ( "total_ptl_name_export %s \n", total_ptl_name_export );

            if ( measure_id !=  YX_104 )
            {
//  长度 19
                fprintf ( owner_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",  "Ver","1.0.0","Author","lisx","Update","2016-11-02 11:10:56","ProtocolNo","100"," ","  "," ","   "," ","  "," "," ","  " ,"  ","  " );
                fprintf ( owner_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",  "dev_code","dev_index_id","commid","c_slaver_id","data_name","data_id","address","data_info","data_sign"," accuracy","scale_coefficient","data_unit","low_limit","  high_limit","low_limit_1"," high_limit_1","func_Rev","  func_Send","func_Fifo" );


                //    fprintf ( export_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",  "Ver","1.0.0","Author","lisx","Update","2016-11-02 11:10:56","ProtocolNo","100"," "," "," ");
                //    fprintf ( export_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",  "dev_code","dev_index_id","commid","c_slaver_id","data_name","address","data_info","data_sign"," accuracy","scale_coefficient","data_unit");

#if  0
//  长度 11
		
                fprintf ( export_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",  "Ver","1.0.0","Author","lisx","Update","2017-09-15 11:10:56","ProtocolNo","100"," "," "," " );
                fprintf ( export_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",  "设备名称","设备编号","端口号","采集地址","数据名称","地址","数据信息","数据类型"," 精确度","比例系数","数据单位" );
#endif

//2017-09-15 修改
		//  长度 4
                fprintf ( export_fp ,"%s,%s,%s,%s\n",  "Ver_1.0.0","Author_lisx","Update_2017-09-15" ,"ProtocolNo_101");
                fprintf ( export_fp ,"%s,%s,%s,%s\n",  "设备名称","地址","测点名称","数据单位" );



            }
            else
            {

                fprintf ( owner_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n","Ver","1.0.0","Author","lisx","Update","2016-11-02 11:10:56","ProtocolNo","100"," "," "," ","   ","  ","  ","  " ,"  " );
                fprintf ( owner_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n","dev_code","dev_index_id","commid","c_slaver_id","address","yx_name","data_name ","data_id","data_sign","data_post","fixed_fault_value_name","low_limit","  high_limit"," func_Rev","  func_Send","func_Fifo" );


                // fprintf ( export_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n","Ver","1.0.0","Author","lisx","Update","2016-11-02 11:10:56","ProtocolNo","100"," "," " );
	      //  fprintf ( export_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n","dev_code","dev_index_id","commid","c_slaver_id","address","yx_name","data_name ","data_sign","data_post","fixed_fault_value_name" );

#if 0
		//  长度 10
                fprintf ( export_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n","Ver","1.0.0","Author","lisx","Update","2016-11-02 11:10:56","ProtocolNo","100"," "," " );

                // "设备名称","设备编号","端口号","采集地址","数据名称","地址","数据信息","数据类型"," 精确度","比例系数","数据单位"
                fprintf ( export_fp ,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n","设备名称","设备编号","端口号","采集地址","地址","遥信测带点名称","数据名称 ","数据类型","数据位","遥信状态匹配值" );
#endif

		
	      fprintf ( export_fp ,"%s,%s,%s,%s\n","Ver_1.0.0","Author_lisx","Update_2017-09-15" ,"ProtocolNo_101" );
                // "设备名称","设备编号","端口号","采集地址","数据名称","地址","数据信息","数据类型"," 精确度","比例系数","数据单位"
               fprintf ( export_fp ,"%s,%s,%s,%s\n","设备名称","地址","测点名称","遥信名称" );

		
		
            }

            uint16_t uuid_num =   uuid_info_gather.uuid_num   ;
            int32_t index_id = 0;
            int32_t map_id = 0;
            int32_t dev_id = 0;
            uint16_t dev_code = 0;
            char commid[20] = { 0 };
            char c_slaver_id_str[20] = { 0 };

            int32_t  first_to_last_index= 0;


            for ( index_id = 0; index_id < uuid_num; index_id ++ )
            {
                first_to_last_index = index_id +1;

                if ( first_to_last_index == uuid_num )
                {
                    first_to_last_index = 0;
                }


                map_id    =  uuid_info_gather.uuid_map[first_to_last_index ];

                dev_id      =   uuid_info_gather.p_mbm_slaver_gather[ map_id].dev_id;
                dev_code = uuid_info_gather.p_mbm_slaver_gather[ map_id].dev_code;
                strcpy ( c_slaver_id_str ,uuid_info_gather.p_mbm_slaver_gather[ map_id].c_slaver_id );
                strcpy ( commid ,uuid_info_gather.p_mbm_slaver_gather[ map_id].commid );

                //  printf ( "g_dev_case_num %d \n",g_dev_case_num );

                for ( dev_case_id  = 0  ; dev_case_id < g_dev_case_num; dev_case_id ++ )
                {
                    if ( dev_code == dev_ptl_gather[dev_case_id].dev_code )
                    {
                        break;
                    }
                }
                if ( dev_case_id == g_dev_case_num )
                {
                    break;
                }


                int32_t dev_index_id = dev_id ;
		
		                        char dev_name[100] = {0 };

                        uint16_t ptl_data_id = 0;


                        char  yx_name[40]= { 0 };//bit 或者 value数据名称
                    //    char  data_name[40]= {0}; //数据名称

                        char data_id_name[20] = {0};
                        char data_sign_name[20]= { 0 };
                        char data_post_name[20] = {0};//数据类型
                        char fixed_fault_value_name[20] = {0};//固定值

                     //   char low_limit[20] = {0}; //下限
                  //      char high_limit[20] = {0};//上限

                        char func_Rev[40] = {0};//数据处理函数序号
                        char func_Send[40] = {0};//数据处理函数序号
                        char func_Fifo[40]= { 0 };//数据处理函数序号
                        
                          char data_name[40] = {0};
                        char data_info[40] = {0};

                        char data_id[20] = { 0 };
                     //   char data_sign_name[20] = { 0 };
                        //    char dec_p_arr[20] = { 0 }; //小数点位数
                        char accuracy_arr[20] = { 0 }; //精确度
                        char scale_coefficient_arr[20] = { 0 }; //比例系数
                        //accuracy,scale_coefficient,

                        char low_limit[20] = { 0 }; //下限
                        char high_limit[20] = { 0 };//上限
                        char low_limit_1[20] = { 0 }; //下限
                        char high_limit_1[20] = { 0 };//上
                   //     char func_Rev[40] = { 0 };//数据处理函数序号
                   //     char func_Send[40] = { 0 };//数据处理函数序号
                       // char func_Fifo[40] = { 0 };//数据处理函数序号

                        char data_unit[40] = { 0 };//数据单位
			
			int32_t dev_code = 0;


                if ( dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl != NULL )
                {
                    if ( dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->yx_flag == 0 )
                    {

                        int32_t ptl_data_num =dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->ptl_data_num;


			

                        for ( ptl_data_id = 0; ptl_data_id < ptl_data_num ; ptl_data_id ++ )
                        {

                            if ( address   >   end_addr )
                            {
                                break;
                            }
                            dev_code =dev_ptl_gather[dev_case_id].dev_code;
			    
			    
			    
			   memset ( dev_name,0,sizeof ( dev_name ) );
                            memset ( accuracy_arr,0,sizeof ( accuracy_arr ) );
                            memset ( data_name,0,sizeof ( data_name ) );
                            memset ( data_id,0,sizeof ( data_id ) );
                            memset ( data_sign_name,0,sizeof ( data_sign_name ) );
                            memset ( data_info,0,sizeof ( data_info ) );
                            //   memset(dev_name,0,sizeof(dev_name ));
                            memset ( scale_coefficient_arr,0,sizeof ( scale_coefficient_arr ) );

                            memset ( low_limit,0,sizeof ( low_limit ) );

                            memset ( high_limit,0,sizeof ( low_limit ) );
			    
			   memset ( low_limit_1,0,sizeof ( low_limit_1 ) );

                            memset ( high_limit_1,0,sizeof ( low_limit_1 ) );

                            memset ( func_Rev,0,sizeof ( func_Rev ) );

                            memset ( func_Send,0,sizeof ( func_Send ) );


                            memset ( func_Fifo,0,sizeof ( func_Fifo ) );
			    
			   memset ( data_unit,0,sizeof ( data_unit ) );
			    
			    
			    

                          //  strcpy ( dev_name   , dev_ptl_gather[dev_case_id].dev_name );
			       strcpy ( dev_name   , dev_ptl_gather[dev_case_id].dev_csv_display_name );
			    

                            strcpy ( data_name                ,dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].data_name );

                            strcpy ( data_id                       , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].data_id );

                            strcpy ( data_sign_name        , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].data_sign );

                            //   strcpy ( dec_p_arr                   ,dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].dec_p_arr );
                            //  accuracy_arr,scale_coefficient_arr

                            strcpy ( accuracy_arr                   ,dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].dec_p_arr );



                            float tmp_scale_coellicient = atof ( dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].scale_coefficient_arr );   //更新 比例系数

                            printf ( "tmp_scale_coellicient %f \n",tmp_scale_coellicient );

                            if ( tmp_scale_coellicient   < 0.0000001 && ( tmp_scale_coellicient    > -0.0000001 ) )
                            {
                                strcpy ( scale_coefficient_arr       ,"1" );
                            }
                            else
                            {
                                strcpy ( scale_coefficient_arr       ,dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].scale_coefficient_arr );
                            }




                            strcpy ( data_info                    ,dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].data_info );

                            strcpy ( func_Rev       ,dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].func_Rev );
                            strcpy ( func_Send     , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].func_Send );
                            strcpy ( func_Fifo       , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].func_Fifo );
                            strcpy ( high_limit      , dev_ptl_gather[  dev_case_id ].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].high_limit );
                            strcpy ( low_limit        , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].low_limit );
                            strcpy ( high_limit_1   , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].high_limit_1 );
                            strcpy ( low_limit_1     , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].low_limit_1 );


                            strcpy ( data_unit     , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data[ptl_data_id].data_unit );


                            printf ( "%d,%d,%s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                                     dev_code,dev_index_id,commid,c_slaver_id_str,data_name,data_id,address ,data_info,data_sign_name,  accuracy_arr,scale_coefficient_arr,low_limit,  high_limit, low_limit_1, high_limit_1,func_Rev,  func_Send ,func_Fifo );


                            fprintf ( owner_fp ,"%d,%d,%s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                                      dev_code,dev_index_id,commid,c_slaver_id_str,data_name,data_id,address ,data_info,data_sign_name, accuracy_arr,scale_coefficient_arr,data_unit,low_limit,  high_limit, low_limit_1, high_limit_1,func_Rev,  func_Send ,func_Fifo );

#if 0
                            fprintf ( export_fp ,"%s,%d,%s,%s,%s,%d,%s,%s,%s,%s,%s\n",
                                      dev_name,dev_index_id,commid,c_slaver_id_str,data_name,address ,data_info,data_sign_name, accuracy_arr,scale_coefficient_arr,data_unit );
#endif

			  fprintf ( export_fp ,"%s,%d,%s,%s\n", dev_name, address,  data_name,data_unit );


                            address++;



                        }// end for ?

                    }
                    else
                    {
                        int32_t ptl_data_num =dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->ptl_data_num;

                         dev_code = dev_ptl_gather[dev_case_id].dev_code;



                        for ( ptl_data_id = 0; ptl_data_id < ptl_data_num ; ptl_data_id ++ )
                        {
                            if ( address   >   end_addr )
                            {
                                break;
                            }


                            memset ( dev_name,0,sizeof ( dev_name ) );
                            memset ( yx_name,0,sizeof ( yx_name ) );
                            memset ( data_name,0,sizeof ( data_name ) );
                            memset ( data_id_name,0,sizeof ( data_id_name ) );
                            memset ( data_sign_name,0,sizeof ( data_sign_name ) );
                            memset ( data_post_name,0,sizeof ( data_post_name ) );
                            memset ( fixed_fault_value_name,0,sizeof ( fixed_fault_value_name ) );

                            memset ( low_limit,0,sizeof ( low_limit ) );

                            memset ( high_limit,0,sizeof ( high_limit ) );

                            memset ( func_Rev,0,sizeof ( func_Rev ) );

                            memset ( func_Send,0,sizeof ( func_Send ) );

                            memset ( func_Fifo,0,sizeof ( func_Fifo ) );
			    
			    
			    

                          //  strcpy ( dev_name   , dev_ptl_gather[dev_case_id].dev_name );
			    
			    	       strcpy ( dev_name   , dev_ptl_gather[dev_case_id].dev_csv_display_name );
				       

                            strcpy ( yx_name,dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].yx_name );
                            strcpy ( data_name,dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].data_name );

                            strcpy ( data_id_name , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].yx_data_id_name );
                            strcpy ( data_sign_name , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].data_sign_name );

                            strcpy ( data_post_name,     dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].data_post_name );

                            strcpy ( fixed_fault_value_name,     dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].fixed_fault_value_name );

                            strcpy ( high_limit    ,  dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].high_limit );
                            strcpy ( low_limit   ,   dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].low_limit );
                            strcpy ( func_Rev   ,      dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].func_Rev );
                            strcpy ( func_Send   ,     dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].func_Send );

                            strcpy ( func_Fifo     , dev_ptl_gather[dev_case_id].dev_ptl_context[ measure_id ].csv_ptl->protocol_data_yx[ptl_data_id].func_Fifo );


                            printf ( "%d,%d,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                                     dev_code, dev_index_id,commid,c_slaver_id_str,address,yx_name,data_name ,data_id_name,data_sign_name,data_post_name,fixed_fault_value_name,low_limit,  high_limit, func_Rev,  func_Send,func_Fifo );


                            fprintf ( owner_fp ,"%d,%d,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                                      dev_code, dev_index_id,commid,c_slaver_id_str,address,yx_name,data_name ,data_id_name,data_sign_name,data_post_name,fixed_fault_value_name,low_limit,  high_limit, func_Rev,  func_Send,func_Fifo );

#if 0
			    fprintf ( export_fp ,"%s,%d,%s,%s,%d,%s,%s,%s,%s,%s\n",
                                      dev_name, dev_index_id,commid,c_slaver_id_str,address,yx_name,data_name ,data_sign_name,data_post_name,fixed_fault_value_name );
#endif
			    
			       fprintf ( export_fp ,"%s,%d,%s,%s\n", dev_name, address,data_name,yx_name );

 
                            address++;



                        }//end for ?

                    } // end else


                }

            }


            fclose ( owner_fp );


            fclose ( export_fp );


        }

    }


    printf ( "creat _ csv _ end \n" );
    return 0;

}



/*******************************************************************************
 * Function       : measure_yk_yx_type_init
 * Author          : shiran
 * Date              : 2017.05.11
 * Description  : 遥控 遥信 单点双点数据 配置  默认使用双点
 * Calls              :
 * Input             :
 * Output          :
 * Return           :void
 ********************************************************************************/
int32_t measure_yk_yx_type_init ( xmlXPathContextPtr context )
{
    xmlNodePtr tmp_node=NULL;
    xmlXPathObjectPtr result = NULL;
    xmlNodeSetPtr nodeset = NULL;
//     int32_t ret_parameter_num = 0;

    xmlChar *xpath_update_measure = ( xmlChar * ) "/root/program_config_type";

    result = xmlXPathEvalExpression ( xpath_update_measure, context );
    if ( result )
    {

        nodeset = result->nodesetval;

        int32_t node_type_id = 0;

        for ( node_type_id = 0; node_type_id < nodeset->nodeNr ; node_type_id++ )
        {
            tmp_node =  nodeset->nodeTab[  node_type_id ];

            if ( !xmlStrcmp ( tmp_node->name,BAD_CAST "program_config_type" ) )
            {
                //查找属性
                xmlNodePtr  type_node_ptr =NULL;
                type_node_ptr = tmp_node;
                xmlAttrPtr type_node_attr_ptr = type_node_ptr->properties;

                while ( type_node_attr_ptr != NULL )
                {

                    if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "measure_form" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "measure_form" );

                        if ( 0  == xmlStrncmp ( szAttr, ( xmlChar * ) "IEC_SP_TYPE" ,11 ) )
                        {
                            measure_yk_yx_control_type = IEC_SP_TYPE;
                        }
                        else
                        {
                            measure_yk_yx_control_type = IEC_DP_TYPE ;
                        }

                        xmlFree ( szAttr );
                    }
                     else  if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "yc_interval" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "yc_interval" );

                        //   int32_t yc_interval = 0;
                        yc_interval = strtoul ( ( char * ) szAttr,NULL, 10 );


                        xmlFree ( szAttr );
                    }
                    else  if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "yc_period" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "yc_period" );

                        //   int32_t yc_period = 0;
                        yc_period = strtoul ( ( char * ) szAttr,NULL, 10 );

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "yx_interval" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "yx_interval" );

                        //   int32_t yx_interval = 0;
                        yx_interval = strtoul ( ( char * ) szAttr,NULL, 10 );

                        xmlFree ( szAttr );
                    }
                    else  if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "yx_period" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "yx_period" );

                        //  int32_t tmp_yx_period = 0;
                        yx_period = strtoul ( ( char * ) szAttr,NULL, 10 );

                        printf ( "yx_perison %d \n" , yx_period );

                        xmlFree ( szAttr );
                    }
                    else  if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "COS" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "COS" );

                        if ( 0  == xmlStrncmp ( szAttr, ( xmlChar * ) "Enabled" ,7 ) )
                        {
                            cos_update_config = 1;
                        }
                        else
                        {
                            cos_update_config = 0 ;
                        }

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( type_node_attr_ptr->name, BAD_CAST "SOE" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( type_node_ptr,BAD_CAST "SOE" );

                        printf ( "szattr %s \n",szAttr );

                        if ( 0  == xmlStrncmp ( szAttr, ( xmlChar * ) "Enabled" ,7 ) )
                        {
                            soe_update_config = 1;
                        }
                        else
                        {
                            soe_update_config = 0 ;
                        }

                        xmlFree ( szAttr );
                    }


                    type_node_attr_ptr = type_node_attr_ptr->next;

                }
            }
        }

    }

    printf ( "--------------------measure_yk_yx_control_type %d  soe_update_config  %d cos_update_config %d  \n",measure_yk_yx_control_type,soe_update_config,cos_update_config );

    xmlXPathFreeObject ( result );

    return 0;
}
/*******************************************************************************
 * Function       : measure_standard_data_init
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : measure_standard_data_init
 * Calls              :
 * Input             :
 * Output          :
 * Return           :void
 ********************************************************************************/

int32_t  measure_standard_data_init ( xmlXPathContextPtr context )
{
    xmlNodePtr tmp_node=NULL;
    xmlXPathObjectPtr result = NULL;
    xmlNodeSetPtr nodeset = NULL;
//     int32_t ret_parameter_num = 0;
    int32_t  node_id = 0;

    printf ( "function:measure_standard_data_init \n" );

    xmlChar *xpath_ptl_total_context = ( xmlChar * ) "/root/measure_standard";

    result = xmlXPathEvalExpression ( xpath_ptl_total_context, context );

    if ( result == NULL )
    {
        printf ( "xmlXPathEvalExpression measure_standard  NULL\n" );
        return 0;
    }

    if ( xmlXPathNodeSetIsEmpty ( result->nodesetval ) )
    {

        xmlXPathFreeObject ( result );

        return 0;
    }


    nodeset = result->nodesetval;

    int32_t node_num =  nodeset->nodeNr ;

    for ( node_id = 0; node_id < node_num ; node_id++ )
    {

        tmp_node =  nodeset->nodeTab[  node_id ];


        if ( !xmlStrcmp ( tmp_node->name,BAD_CAST "measure_standard" ) )
        {
            int32_t  total_ptl_id  = 0;
            int32_t  measure_type_data  = 0;
            uint16_t  min_addr  = 0;
            uint16_t  max_addr  = 0;

            //查找属性
            xmlNodePtr next_node=NULL;
            xmlNodePtr  propNodePtr =NULL;

            next_node = tmp_node->xmlChildrenNode;

            while ( next_node != NULL )
            {
                //取出节点中的内容

                if ( ( !xmlStrcmp ( next_node->name,BAD_CAST "table" ) ) )   //commport 结点内容解析
                {
                    propNodePtr = next_node;
                    xmlAttrPtr node_attr_ptr = propNodePtr->properties;

                    while ( node_attr_ptr != NULL )
                    {

                        if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "id" ) )
                        {
                            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "id" );

                            total_ptl_id = strtoul ( ( char * ) szAttr,NULL, 0 );

                            printf ( "  --  total_ptl_id   %d      \n   ",total_ptl_id );

                            xmlFree ( szAttr );
                        }
                        else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "measure_type" ) )
                        {
                            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "measure_type" );

                            measure_type_data =   get_measure_type ( ( char * ) szAttr );

                            printf ( "  --    measure_type_data %d   \n   ",measure_type_data );


                            xmlFree ( szAttr );
                        }
                        else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "MINADDR" ) )
                        {
                            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "MINADDR" );

                            min_addr = strtoul ( ( char * ) szAttr,NULL, 0 );

                            xmlFree ( szAttr );
                        }
                        else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "MAXADDR" ) )
                        {
                            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "MAXADDR" );

                            max_addr = strtoul ( ( char * ) szAttr,NULL, 0 );

                            xmlFree ( szAttr );
                        }


                        node_attr_ptr = node_attr_ptr->next;

                    }

                    if ( measure_type_data ==  YC_104 )
                    {
// 遥测地址
                        REMOTE_MESURE_BEGIN_ADDR = min_addr ;  //   0x4001
                        REMOTE_MESURE_END_ADDR     = max_addr ;  //  0x4036 //shiran

                    }
                    else  if ( measure_type_data ==  YK_104 )
                    {
// 遥控地址
                        REMOTE_CONTROL_BEGIN_ADDR = min_addr;  //   0x6001
                        REMOTE_CONTROL_END_ADDR   = max_addr;  //    0x6007   //shiran
                    }
                    else  if ( measure_type_data ==  YT_104 )
                    {
// 浮点遥调地址
                        REMOTE_ADJUST_BEGIN_ADDR = min_addr  ;  //  0x6201
                        REMOTE_ADJUST_END_ADDR    = max_addr  ;  //  0x6208  //shiran

                    }
                    else  if ( measure_type_data ==  YX_104 )
                    {
// 遥信地址
                        REMOTE_SIGNAL_BEGIN_ADDR = min_addr;  //  0x0001  //xusa 2013.11.5
                        REMOTE_SIGNAL_END_ADDR = max_addr  ;  //    0x14d    //shiran 333

                    }
                    else  if ( measure_type_data ==  YM_104 )
                    {
// 累计量
                        REMOTE_PLUSE_BEGIN_ADDR = min_addr  ;  //   0x6401
                        REMOTE_PLUSE_END_ADDR     = max_addr ;  //   0x6415

                    }
                    printf ( "  --  REMOTE_MESURE_BEGIN_ADDR   0x%0x      \n   ",min_addr );
                    printf ( "  --  REMOTE_MESURE_END_ADDR   0x%0x      \n   ",max_addr );
                }
                next_node =next_node->next ;
            }

        }

    }

    xmlXPathFreeObject ( result );

    return 1;



}






/*******************************************************************************
 * Function       : mbm_slaver_uuid_sort
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : mbm_slaver_uuid_sort  uuid从0 开始排序  加入内部设备排序  
 * Calls              :
 * Input             :
 * Output          :
 * Return           :void
 ********************************************************************************/
void mbm_slaver_uuid_sort ( void )
{
    uint16_t  data_num  =  uuid_info_gather.uuid_num;
    uint16_t  id = 0;
    int32_t search_key = -1;
    int32_t tmp_data    = 0;
    int32_t base_data   = 0 ;
    int32_t base_index  = 0;

    int32_t slaver_index = 0;

    int32_t  remain_id = 0;


    uuid_info_gather.uuid_map = ( uint32_t * ) data_safe_distribution ( ( uuid_info_gather.uuid_num ) *sizeof ( uint32_t ) ,"uuid_info_gather   :" ) ;
    memset ( uuid_info_gather.uuid_map,0, ( uuid_info_gather.uuid_num ) *sizeof ( uint32_t ) );

    //全部设备开始 uuid 排序

    for ( id = 0 ; id < data_num ;  id  ++ )
    {
      
        base_data =  0x0fffffff;

        for ( slaver_index = 0 ; slaver_index < data_num ; slaver_index ++ )
        {
            tmp_data = uuid_info_gather.p_mbm_slaver_gather[ slaver_index  ].dev_uuid;

            if ( ( tmp_data  >search_key )  && ( tmp_data  <   base_data ) )
            {
                base_data   =  tmp_data;
                base_index =  slaver_index;
            }
        }  //找到 大于key 的最小值  检索标准 dev_uuid  

        search_key  =  base_data;
        uuid_info_gather.uuid_map[id] = base_index;

    }

    //更新 内部设备 排序  从第二个设备开始 第一个设备是默认 logger 本身
    //从第二设备开始和当前设备之后的所有设备进行检测，查找是否是内部设备，并且内部设备序号小的，将设备向后移动，当前设备替换成查询到的设备

    for ( id = 1 ; id < data_num-1 ;  id  ++ )
    {

        int32_t   slaver_id  =  id  ;


        int32_t tmp_base_index   = uuid_info_gather.uuid_map[ slaver_id ] ;

        int32_t search_inside_no_key =  uuid_info_gather.p_mbm_slaver_gather[ tmp_base_index   ].inside_no;

        int32_t search_inside_dev_flag  =   uuid_info_gather.p_mbm_slaver_gather[ tmp_base_index   ].inside_dev_flag ;

        int32_t tmp_inside_no = 0;

        int32_t slave_exist_swap_flag = 0;

        int32_t uuid_map_record = 0 ;


        for ( remain_id = id+1 ; remain_id < data_num ;  remain_id  ++ )
        {

            int32_t mbm_slave_id   = uuid_info_gather.uuid_map[ remain_id ] ;


            if ( 1 == uuid_info_gather.p_mbm_slaver_gather[  mbm_slave_id   ].inside_dev_flag )
            {

                tmp_inside_no = uuid_info_gather.p_mbm_slaver_gather[ mbm_slave_id  ].inside_no;

                if ( ( search_inside_dev_flag < 1 ) || ( tmp_inside_no   < search_inside_no_key ) )  //判断之前需要对比的是外部设备还是内部设备序号较大的条件
                {
		  
		   uuid_map_record  = remain_id;    
		       
                    search_inside_no_key   =  tmp_inside_no;

                    tmp_base_index =  mbm_slave_id;

                    search_inside_dev_flag = 1;  //条件允许，之后内循环的设备直接找设备序号较小的就可以，外部设备条件取消

                    slave_exist_swap_flag = 1;   //确定查找到对应的内部设备，内部设备序号最小的设备
                }

            }

        }

        if ( slave_exist_swap_flag )   //完成内部设备号最小的设备和之前对比的当前设备进行对比交换
        {

            int32_t tmp_index =  0 ;

            tmp_index =  uuid_info_gather.uuid_map[ slaver_id ]  ;

            uuid_info_gather.uuid_map[ slaver_id ]    = tmp_base_index  ;

            uuid_info_gather.uuid_map[ uuid_map_record ] = tmp_index;
	    
	    
 
	    printf("uuid_info_gather.uuid_map[ slaver_id %d  ]  %d  uuid_info_gather.uuid_map[ uuid_map_record %d  ]  %d \n",slaver_id,uuid_info_gather.uuid_map[ slaver_id ], uuid_map_record,uuid_info_gather.uuid_map[ uuid_map_record ] );


        }

    }


    for ( id = 0 ; id < data_num ;  id  ++ )
    {
      
        int32_t slaver_id =  uuid_info_gather.uuid_map[id] ;
        printf ( "  slaver_id %d dev_uuid:%d devc_ode %d \n",slaver_id , uuid_info_gather.p_mbm_slaver_gather[ slaver_id  ].dev_uuid,uuid_info_gather.p_mbm_slaver_gather[ slaver_id  ].dev_code );

    }



}



/*******************************************************************************
 * Function       : get_mbm_slaver_table_attr_config
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : get_mbm_slaver_table_attr_config,
 * Calls              :
 * Input             :
 * Output          :
 * Return           :存在返回真  不存在返回假
 ********************************************************************************/
static uint32_t get_mbm_slaver_table_attr_config ( xmlNodePtr  node_tmp,mbm_slaver_table  *p_tmp_mbm_slaver,int32_t dev_code )
{

    int32_t dev_id          = 0;
    char c_slaver_id_str[20]  = {0};
    int32_t dev_uuid     = 0;
    int32_t dev_inside_flag = 0;
    int32_t dev_inside_no = 0;
    char com_name[20] = { 0 };

    xmlNodePtr  propNodePtr;
    //查找属性
    propNodePtr = node_tmp;
    xmlAttrPtr attrPtr_temp = propNodePtr->properties;

// DevId="1" CSlaveID="1" COM="" Uuid="0" UnionDev="1" VirtualDev="0"
    while ( attrPtr_temp != NULL )
    {
        if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "DevId" ) )
        {

            xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "DevId" );

            dev_id =    strtoul ( ( char * ) szAttr, NULL, 0 );

            xmlFree ( szAttr );
        }
        else  if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "CSlaveID" ) )
        {  
           xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "CSlaveID" );

	     strcpy ( c_slaver_id_str, ( char * ) szAttr );

           xmlFree ( szAttr );
        }
        else    if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "COM" ) )
        {

            xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "COM" );

         //   strcpy ( p_tmp_mbm_slaver->commid, ( char * ) szAttr );
	     strcpy ( com_name, ( char * ) szAttr );

            xmlFree ( szAttr );
        }
        else   if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "Uuid" ) )
        {

            xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "Uuid" );

            dev_uuid =    strtoul ( ( char * ) szAttr, NULL, 0 );

            xmlFree ( szAttr );
        }
        else   if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "InsideDev" ) )
        {

            xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "InsideDev" );

            dev_inside_flag =    strtoul ( ( char * ) szAttr, NULL, 0 );

            xmlFree ( szAttr );
        }
        else   if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "InsideNO" ) )
        {

            xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "InsideNO" );

            dev_inside_no =    strtoul ( ( char * ) szAttr, NULL, 0 );

            xmlFree ( szAttr );
        }
        attrPtr_temp = attrPtr_temp->next ;
    }

    strcpy ( p_tmp_mbm_slaver->c_slaver_id,c_slaver_id_str ) ;
    p_tmp_mbm_slaver->dev_code = (uint16_t)dev_code ;
    p_tmp_mbm_slaver->dev_id =  dev_id ;
    p_tmp_mbm_slaver->dev_uuid = dev_uuid;
    p_tmp_mbm_slaver->inside_dev_flag = dev_inside_flag;
    p_tmp_mbm_slaver->inside_no = dev_inside_no ;
    
     strcpy ( p_tmp_mbm_slaver->commid,com_name ) ;
   

    printf ( "  c_slaver_id_str %s dev_code  %d dev_id %d  dev_uuid %d  com_name %s \n",    c_slaver_id_str , dev_code , dev_id , dev_uuid,com_name );
    
    printf(  " p_tmp_mbm_slaver->c_slaver_id %s     p_tmp_mbm_slaver->dev_code %d      p_tmp_mbm_slaver->dev_id %d         p_tmp_mbm_slaver->dev_uuid %d    p_tmp_mbm_slaver->inside_dev_flag %d      p_tmp_mbm_slaver->inside_no %d        p_tmp_mbm_slaver->commid %s \n", 
    p_tmp_mbm_slaver->c_slaver_id,
    p_tmp_mbm_slaver->dev_code,
    p_tmp_mbm_slaver->dev_id ,
    p_tmp_mbm_slaver->dev_uuid,
    p_tmp_mbm_slaver->inside_dev_flag ,
    p_tmp_mbm_slaver->inside_no ,
  p_tmp_mbm_slaver->commid);

    return 0;

}





/*******************************************************************************
 * Function       : xml_config_content_init
 * Author          : shiran
 * Date              : 2016.02.03
 * Description  : XML文件读取
 * Calls              :
 * Input          :char* configfilePATH xml文件路径
 * Output       :
 * Return        :
 ********************************************************************************/
int32_t xml_config_content_init ( char* configfilePATH )
{
    xmlDocPtr doc; //定义解析文档指针
    xmlNodePtr rootNode=NULL;
    xmlNodePtr Node1=NULL;
    xmlNodePtr Node2 =NULL;
    xmlNodePtr Node3=NULL;//,Node4; //定义结点指针(你需要它为了在各个结点间移动),四级嵌套
    int32_t mbm_slaver_table_num = 0;
    char *szDocName=NULL;

    int32_t record_dev_type = 0;   //设备序号记录  当前读取的设备序号小于之前读取的设备序号 跳出

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

    xmlXPathContextPtr context=NULL;

    context = xmlXPathNewContext ( doc );
    if ( context == NULL )
    {
        printf ( "context is NULL\n" );
        return 0;
    }

    //读取总协议点表名称

    measure_standard_data_init ( context );  //遥信 遥调 遥测 遥脉 初始化范围

    measure_yk_yx_type_init ( context ); //单点双点初始化

    get_protocol_tar_content_node ( context );


    xmlChar *xpathCommport = ( xmlChar * ) "/root/commport";

    g_net_case_num = get_node_parameter_num ( context , xpathCommport );



    g_net_case_num   =  NET_INITIALIZE_NUM;



    xmlChar *xpath_dev_type = ( xmlChar * ) "/root/devtype";

    g_dev_case_num = get_node_parameter_num ( context , xpath_dev_type );


    dev_ptl_gather = ( dev_protocol_gather * ) data_safe_distribution ( sizeof ( dev_protocol_gather ) *g_dev_case_num,"dev_ptl_gather   :" );

    memset ( dev_ptl_gather ,0,sizeof ( dev_protocol_gather ) * ( g_dev_case_num ) );

    xmlChar *xpath_mbmslaver_table = ( xmlChar * ) "/root/devtype/mbmslaver/table";

    uuid_info_gather.uuid_num = get_node_parameter_num ( context , xpath_mbmslaver_table );


    uuid_info_gather.p_mbm_slaver_gather = ( mbm_slaver_table * ) data_safe_distribution ( sizeof ( mbm_slaver_table ) * ( uuid_info_gather.uuid_num ) ,"uuid_info_gather.p_mbm_slaver_gather   :" );

    memset ( uuid_info_gather.p_mbm_slaver_gather ,0,sizeof ( mbm_slaver_table ) * ( uuid_info_gather.uuid_num ) );


    rootNode = xmlDocGetRootElement ( doc ); //确定文档根元素
    /*检查确认当前文档中包含内容*/
    if ( NULL == rootNode )
    {
        fprintf ( stderr,"empty document\n" );
        xmlFreeDoc ( doc );
        return -1;
    }
    //在这个例子中，我们需要确认文档是正确的类型“root”是在这个示例中使用文档的根类型
    if ( xmlStrcmp ( rootNode->name, BAD_CAST "root" ) )
    {
        fprintf ( stderr,"document of the wrong type, root node != root" );
        xmlFreeDoc ( doc );
        return -1;
    }
    Node1 = rootNode->xmlChildrenNode;

    while ( Node1 != NULL )
    {
        //取出节点中的内容

        if ( ( !xmlStrcmp ( Node1->name,BAD_CAST "commport" ) ) )   //commport 结点内容解析
        {

            Node2 = Node1->xmlChildrenNode;

            int32_t net_comm_id = 0;  //端口实例 序号 网口0 网口1

            //commport 节点属性解析
            net_comm_id =     commport_attr_analysis ( Node1 );

            if ( ( net_comm_id < 0 ) || ( net_comm_id >= 2 ) )
            {

                terminal_print_time_lable( );

                printf ( "\n---net_case_type %d  errror-----\n \n",net_comm_id );

                return  -1;
            }

            GlobalComMasterConn[net_comm_id].net_id  =  net_comm_id ;

            int32_t bg_num = 0;

            char xpath_bg[100 ] = { 0 };

            sprintf ( xpath_bg ,"%s[@netid='%d']/background_support",xpathCommport,net_comm_id );

            xmlChar *xpath_background = ( xmlChar * ) xpath_bg;


            bg_num =  get_node_parameter_num ( context , xpath_background );
            printf ( "%s  - bg_num %d\n",xpath_bg,bg_num );


            while ( Node2 != NULL )
            {


                if ( ( !xmlStrcmp ( Node2->name, BAD_CAST"background_support" ) ) )
                {
                    Node3 = Node2->xmlChildrenNode;

                    int32_t background_id = 0;

                    background_id =    background_attr_analysis ( Node2  ,GlobalComMasterConn[net_comm_id].background_dev );

                    if ( background_id < 0 )
                    {
                        terminal_print_time_lable( );
                        printf ( "\nbackground_id   error\n" );
                        break;
                    }

                    if ( GlobalComMasterConn[net_comm_id].background_num < background_id )
                    {
                        GlobalComMasterConn[net_comm_id].background_num  =  background_id ;
                    }

                }

                Node2= Node2->next ;
            }
        }
        else  if ( ( !xmlStrcmp ( Node1->name, ( const xmlChar * ) "devtype" ) ) ) //devtype 结点内容解析
        {

            printf ( "\n------------devtype --------------------\n" );

            xmlNodePtr propNodePtr = Node1;

            int32_t  dev_type_slaver_id = 0;

            int32_t dev_code_data = 0;

            char  dev_code_name[100] = { 0 };

            int32_t dev_update_flag = 0;

            int32_t dev_num = 0;


          char  dev_totol_name[100] = { 0 };

          char  dev_display_name[100] = { 0 };

            propNodePtr = Node1;
            xmlAttrPtr attrPtr = propNodePtr->properties;

            while ( attrPtr != NULL )
            {
                if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "id" ) )
                {
                    xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "id" );

                    dev_type_slaver_id = strtoul ( ( char * ) szAttr,NULL, 0 );

                    if ( record_dev_type < dev_type_slaver_id )
                    {
                        record_dev_type = dev_type_slaver_id;
                    }
                    else
                    {
                        fprintf ( stderr,"dev_type_slaver_id  read config error _exit modbusSlave " );
                        exit ( 1 );
                    }




                    xmlFree ( szAttr );
                }
                else  if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DevCode" ) )
                {
                    xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "DevCode" );

                    dev_code_data = strtoul ( ( char * ) szAttr,NULL, 0 );

                    xmlFree ( szAttr );
                }
                else  if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DevName" ) )
                {
                    xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "DevName" );

                    strcpy ( dev_code_name, ( char * ) szAttr );

                    xmlFree ( szAttr );
                }
                else  if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "name" ) )
                {
                    xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "name" );

                    strcpy ( dev_totol_name, ( char * ) szAttr );

                    xmlFree ( szAttr );
                }
                else  if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DisplayName" ) )
                {
                    xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "DisplayName" );

                    strcpy ( dev_display_name, ( char * ) szAttr );

                    xmlFree ( szAttr );
                }
                else  if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DevNum" ) )
                {
                    xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "DevNum" );

                    dev_num = strtoul ( ( char * ) szAttr,NULL, 0 );

                    xmlFree ( szAttr );
                }
                else  if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "update_flag" ) )
                {
                    xmlChar* szAttr =   xmlGetProp ( propNodePtr,BAD_CAST "update_flag" );


                    dev_update_flag = strtoul ( ( char * ) szAttr,NULL, 0 );

                    xmlFree ( szAttr );
                }


                attrPtr = attrPtr->next;
            }


            //     printf ( "dev_code_name %s dev_totol_name %s dev_code_data %d dev_num %d dev_update_flag %d \n ",dev_code_name,dev_totol_name,dev_code_data,dev_num,dev_update_flag );
            int32_t max_ptl_data_id  = 0;
            int32_t tmp_ptl_data_id  = 0;


            strcpy ( dev_ptl_gather[ dev_type_slaver_id -1 ].dev_name , dev_code_name );
	    
	    strcpy ( dev_ptl_gather[ dev_type_slaver_id -1 ].dev_csv_display_name,dev_display_name);
	    
            strcpy ( dev_ptl_gather[ dev_type_slaver_id -1 ].totol_dev_name ,    dev_totol_name );

            dev_ptl_gather[ dev_type_slaver_id -1 ].dev_code = dev_code_data;
            dev_ptl_gather[ dev_type_slaver_id -1 ].dev_num =   dev_num;
            dev_ptl_gather[ dev_type_slaver_id -1 ].dev_update_flag =  dev_update_flag;

            if ( dev_update_flag > 0 )
            {
                g_dev_type_update_flag =  1;
            }

            Node2 = Node1->xmlChildrenNode;

            while ( Node2 != NULL )
            {
                if ( ( !xmlStrcmp ( Node2->name, ( const xmlChar * ) "protocol" ) ) )
                {

                    Node3 = Node2->xmlChildrenNode;

                    while ( Node3 != NULL )
                    {

                        if ( ( !xmlStrcmp ( Node3->name, ( const xmlChar * ) "table" ) ) )
                        {

                            tmp_ptl_data_id     =    get_dev_attr_config ( Node3,& ( dev_ptl_gather[ dev_type_slaver_id -1 ] ) );

                            if ( max_ptl_data_id < tmp_ptl_data_id )
                            {
                                max_ptl_data_id = tmp_ptl_data_id;
                            }
                            printf("  max_ptl_data_id  %d \n",max_ptl_data_id);
                        }

                        Node3 = Node3->next;
                    }

                }
                else     if ( ( !xmlStrcmp ( Node2->name, ( const xmlChar * ) "mbmslaver" ) ) )
                {

                    Node3 = Node2->xmlChildrenNode;

                    while ( Node3 != NULL )
                    {

                        if ( ( !xmlStrcmp ( Node3->name, ( const xmlChar * ) "table" ) ) )
                        {

                            int32_t tab_id =   mbm_slaver_table_num;
                            if ( tab_id < uuid_info_gather.uuid_num )
                            {

                                get_mbm_slaver_table_attr_config ( Node3,& ( uuid_info_gather.p_mbm_slaver_gather[ tab_id ] ) ,dev_code_data );
                                mbm_slaver_table_num  ++;
                            }
                            else
                            {
                                fprintf ( stderr,"mbm_slaver_table_num beyong set \n" );
                            }


                        }

                        Node3 = Node3->next;
                    }

                }

                Node2 = Node2->next;
            }


            //更新最大的data_id
            dev_ptl_gather[ dev_type_slaver_id -1 ].max_data_id =  max_ptl_data_id;
        }
        Node1 = Node1->next;
    }


    xmlFreeDoc ( doc );




//   mbm_slaver_uuid_sort ( );
    if ( g_dev_type_update_flag > 0 )
    {
        printf ( "   mbm_slaver_uuid_sort ( )\n" );
        mbm_slaver_uuid_sort ( );


        protocol_tar_content_csv_create_init();

        xml_write_dev_type_update_flag_config ( configfilePATH );

    }


    init_create_import_csv ( configfilePATH );

    init_comm_conn ( );

//写入配置文件 版本号
    xml_write_FirmwareVersion_config ( XML_VERSION_PATH );



    return 0;

}




/************************************************************************
*                   局部函数
*************************************************************************/


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


    if ( element[i-1] == '\r' )
    {
        element[i-1] = '\0';
    }
    return ( i+1 );
//     return i;
}


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

#if 0
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
#endif
int32_t get_measure_type ( char * measure_type_name )
{

    int32_t ret_type = -1;

    if ( ( NULL !=strstr ( measure_type_name,"YX" ) ) || ( NULL !=strstr ( measure_type_name,"yx" ) ) )
    {
        //  printf ( "-----------------------YX------------\n" );
        // 遥信地址
        ret_type =   YX_104;// 0

    }
    else   if ( ( NULL !=strstr ( measure_type_name,"YC" ) ) || ( NULL !=strstr ( measure_type_name,"yc" ) ) )
    {
        //   printf ( "------------------------YC------------------------\n" );
        // 遥测地址
        ret_type =    YC_104;// 0

    }
    else   if ( ( NULL !=strstr ( measure_type_name,"YT" ) ) || ( NULL !=strstr ( measure_type_name,"yt" ) ) )
    {
        //    printf ( "-----------------------YT-----------------------\n " );
        // 浮点遥调地址
        ret_type =    YT_104;// 0

    }
    else    if ( ( NULL !=strstr ( measure_type_name,"YK" ) ) || ( NULL !=strstr ( measure_type_name,"yk" ) ) )
    {
        //     printf ( "-----------------------YK------------------------\n" );
        // 遥控地址
        ret_type =    YK_104;// 0
    }
    else   if ( ( NULL !=strstr ( measure_type_name,"YM" ) ) || ( NULL !=strstr ( measure_type_name,"ym" ) ) )
    {
        //  printf ( "------------------------YM------------------------\n" );
        ret_type =    YM_104;// 0
        // 累计量
    }
    return ret_type;
}

/*******************************************************************************
 * Function       : get_dev_attr_config
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : get_dev_attr_config,
 * Calls              :
 * Input             :
 * Output          :
 * Return           :存在返回真  不存在返回假
 ********************************************************************************/
static uint32_t get_dev_attr_config ( xmlNodePtr  node_tmp,dev_protocol_gather  *p_dev_context )
{

    char protocol_name[100] = { 0 };

    char protocol_path_name[100] = { 0 };
    char measure_type_str[100] = { 0 };
    int32_t measure_type_data = 0;

    xmlNodePtr  propNodePtr;
    //查找属性
    propNodePtr = node_tmp;
    xmlAttrPtr attrPtr_temp = propNodePtr->properties;

    int32_t ptl_max_data_id = 0;



    while ( attrPtr_temp != NULL )
    {
        if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "id" ) )
        {

            xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "id" );
#if 0
            int32_t ptl_id =    strtoul ( ( char * ) szAttr, NULL, 0 );
            printf ( "ptl_id %d \n",ptl_id );
#endif

            xmlFree ( szAttr );
        }
        else  if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "measure_type" ) )
        {

            xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "measure_type" );

            strcpy ( measure_type_str , ( char * ) szAttr );

            measure_type_data =   get_measure_type ( measure_type_str );

            xmlFree ( szAttr );
        }
        else       if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "ProtocolName" ) )
        {

            xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "ProtocolName" );

            strcpy ( protocol_name , ( char * ) szAttr );


            xmlFree ( szAttr );
        }
        attrPtr_temp = attrPtr_temp->next ;
    }



    //  测试文件存在与否,如果不存在,继续重新执行
    sprintf ( protocol_path_name,IEC104SLAVEPROTOCOL"%s",protocol_name );
    if ( !DirExist ( protocol_path_name ) )
    {
        terminal_print_time_lable( );
        perror ( "xml param protocol_name  file not exist---\n " );
        p_dev_context->dev_ptl_context[ measure_type_data  ].csv_ptl = NULL;

        printf ( "measure_type_data %d protocol_path_name %s \n",measure_type_data ,protocol_path_name );

        return -1;
    }

    ptl_max_data_id =   get_tab_from_xml ( p_dev_context->dev_ptl_context,protocol_path_name );
    printf ( "ptl_max_data_id %d  \n ",ptl_max_data_id );
    return ptl_max_data_id;

}












/*******************************************************************************
 * Function       : get_dev_attr_config
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : get_dev_attr_config,
 * Calls              :
 * Input             :
 * Output          :
 * Return           :存在返回真  不存在返回假
 ********************************************************************************/
static uint32_t get_total_protocl_attr_config ( xmlNodePtr  node_tmp,total_protocol_gather  *p_totoal_context )
{
    xmlNodePtr  next_node = NULL;

    char protocol_name[100] = { 0 };

    char measure_type_str[100] = { 0 };
    int32_t measure_type_data = 0;
    int32_t ptl_id = 0;
    xmlNodePtr  propNodePtr;



    next_node = node_tmp->xmlChildrenNode;

    while ( next_node != NULL )
    {
        //取出节点中的内容

        if ( ( !xmlStrcmp ( next_node->name,BAD_CAST "table" ) ) )   //commport 结点内容解析
        {
            //查找属性
            propNodePtr = next_node;
            xmlAttrPtr attrPtr_temp = propNodePtr->properties;

            while ( attrPtr_temp != NULL )
            {
                if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "id" ) )
                {

                    xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "id" );

                    ptl_id =         strtoul ( ( char * ) szAttr, NULL, 0 );
//                     printf ( "ptl_id  %d     ---\n",ptl_id );
                    xmlFree ( szAttr );
                }
                else  if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "measure_type" ) )
                {

                    xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "measure_type" );

                    strcpy ( measure_type_str , ( char * ) szAttr );

                    measure_type_data =   get_measure_type ( measure_type_str );

                    xmlFree ( szAttr );
                }
                else       if ( !xmlStrcmp ( attrPtr_temp->name, BAD_CAST "ProtocolName" ) )
                {

                    xmlChar* szAttr =  xmlGetProp ( propNodePtr,BAD_CAST "ProtocolName" );

                    strcpy ( protocol_name , ( char * ) szAttr );

                    xmlFree ( szAttr );
                }
                attrPtr_temp = attrPtr_temp->next;
            }

            printf ( "\nget_total_protocl_attr_config ---ptl_id %d protocol_name %s measure_type_data %d    ---\n",ptl_id,protocol_name ,measure_type_data );

            if ( measure_type_data >=  0 )
            {


                strcpy ( p_totoal_context->total_ptl_context[measure_type_data ].measure_prtocol_name,protocol_name );
            }


        }

        next_node =  next_node->next;
    }

    return 1;


}





/*******************************************************************************
 * Function       : get_csv_data_sign
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : 获取数据类型,
 * Calls              :
 * Input             :
 * Output          :
 * Return           :存在返回对应宏定义  不存在返回1
 ********************************************************************************/
int8_t  get_csv_data_sign ( char *p_sign_element )
{
    int8_t ret  = 0;
    if ( strchr ( p_sign_element,'F' ) )
    {
        ret = FLOAT_T;
    }
    else if ( strchr ( p_sign_element,'U' ) )
    {

        ret = U_INT_T;
    }
    else if ( strchr ( p_sign_element,'S' ) )
    {
        ret = S_INT_T;
    }
    else
    {
        ret  = -1;
    }

    return ret;


}


/*******************************************************************************
 * Function       : verification_addr_data
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : 验证地址数据
 * Calls              :
 * Input             :
 * Output          :
 * Return           :存在返回真  不存在返回假
 ********************************************************************************/
int32_t verification_addr_data ( uint16_t  addr_tmp, int32_t  measure_id )
{
//     <table   id="1" measure_type="YX"   MINADDR= "0x0001"  MAXADDR="0x4000"  />
// 	    <table   id="2" measure_type="YC"   MINADDR= "0x4001"  MAXADDR="0x6000"  />
// 	    <table   id="3" measure_type="YK"   MINADDR= "0x6001"  MAXADDR="0x6200"  />
// 	    <table   id="4" measure_type="YT"   MINADDR= "0x6201"  MAXADDR="0x6400"  />
// 	    <table   id="5" measure_type="YM"   MINADDR= "0x6401"  MAXADDR="0x7000"  />
    int32_t ret_ver = 0;

    if ( measure_id ==  YC_104 )
    {
// 遥测地址
        // printf ( "  --  REMOTE_MESURE_BEGIN_ADDR   0x%0x  REMOTE_MESURE_END_ADDR   0x%0x    addr_tmp 0x%0x \n   ",REMOTE_MESURE_BEGIN_ADDR,REMOTE_MESURE_END_ADDR,addr_tmp );

        if ( ( addr_tmp >= REMOTE_MESURE_BEGIN_ADDR ) && ( addr_tmp <=    REMOTE_MESURE_END_ADDR ) )
        {
            ret_ver =  1;
        }
        else
        {
            ret_ver = -1;
        }
    }
    else  if ( measure_id ==  YK_104 )
    {
// 遥控地址
//         REMOTE_CONTROL_BEGIN_ADDR = min_addr;  //   0x6001
//         REMOTE_CONTROL_END_ADDR   = max_addr;  //    0x6007   //shiran
        // printf ( "  --  REMOTE_CONTROL_BEGIN_ADDR   0x%0x  REMOTE_CONTROL_END_ADDR   0x%0x    addr_tmp 0x%0x \n   ",REMOTE_CONTROL_BEGIN_ADDR,REMOTE_CONTROL_END_ADDR,addr_tmp );

        if ( ( addr_tmp >= REMOTE_CONTROL_BEGIN_ADDR ) && ( addr_tmp <=    REMOTE_CONTROL_END_ADDR ) )
        {
            ret_ver = 1;
        }
        else
        {
            ret_ver = -1;
        }

    }
    else  if ( measure_id ==  YT_104 )
    {
// 浮点遥调地址
//         REMOTE_ADJUST_BEGIN_ADDR = min_addr  ;  //  0x6201
//         REMOTE_ADJUST_END_ADDR    = max_addr  ;  //  0x6208  //shiran
        //printf ( "  --  REMOTE_ADJUST_BEGIN_ADDR   0x%0x  REMOTE_ADJUST_END_ADDR   0x%0x    addr_tmp 0x%0x \n   ",REMOTE_ADJUST_BEGIN_ADDR,REMOTE_ADJUST_END_ADDR,addr_tmp );

        if ( ( addr_tmp >= REMOTE_ADJUST_BEGIN_ADDR ) && ( addr_tmp <=    REMOTE_ADJUST_END_ADDR ) )
        {
            ret_ver = 1;
        }
        else
        {
            ret_ver = -1;
        }

    }
    else  if ( measure_id ==  YX_104 )
    {
// 遥信地址
        //printf ( "  --  REMOTE_SIGNAL_BEGIN_ADDR   0x%0x  REMOTE_SIGNAL_END_ADDR   0x%0x    addr_tmp 0x%0x \n   ",REMOTE_SIGNAL_BEGIN_ADDR,REMOTE_SIGNAL_END_ADDR,addr_tmp );

//         REMOTE_SIGNAL_BEGIN_ADDR = min_addr;  //  0x0001  //xusa 2013.11.5
//         REMOTE_SIGNAL_END_ADDR = max_addr  ;  //    0x14d    //shiran 333

        if ( ( addr_tmp >= REMOTE_SIGNAL_BEGIN_ADDR ) && ( addr_tmp <=    REMOTE_SIGNAL_END_ADDR ) )
        {
            ret_ver = 1;
        }
        else
        {
            ret_ver = -1;
        }

    }
    else  if ( measure_id ==  YM_104 )
    {
// 累计量
        // printf ( "  --  REMOTE_PLUSE_BEGIN_ADDR   0x%0x  REMOTE_PLUSE_END_ADDR   0x%0x    addr_tmp 0x%0x \n   ",REMOTE_PLUSE_BEGIN_ADDR,REMOTE_PLUSE_END_ADDR,addr_tmp );

//         REMOTE_PLUSE_BEGIN_ADDR = min_addr  ;  //   0x6401
//         REMOTE_PLUSE_END_ADDR     = max_addr ;  //   0x6415

        if ( ( addr_tmp >= REMOTE_PLUSE_BEGIN_ADDR ) && ( addr_tmp <=    REMOTE_PLUSE_END_ADDR ) )
        {
            ret_ver = 1;
        }
        else
        {
            ret_ver = -1;
        }

    }
    return  ret_ver ;


}

/*******************************************************************************
 * Function       : get_tab_from_total_csv
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : get_tab_from_csv根据生成的点表csv文件名称解析文件内容至结构体protocol_total_data_s中,
 * Calls              :access 测试文件存在与否
 * Input             : char* configfilePATH xml文件路径
 * Output          :
 * Return           :存在返回真  不存在返回假
 ********************************************************************************/
static uint32_t get_tab_from_total_csv ( total_protocol_gather *p_protocol_gather_case   ,char *p_protocol_name, int32_t measure_type_data )
{

    char absolute_measure_name[100] = { 0 };

    FILE *fp=NULL;

    protocol_s_total *tmp_protocol=NULL;

    protocol_total_data_s *protocol_data = NULL;

    protocol_total_data_s_yx *protocol_data_yx = NULL;


    uint32_t i = 0;
    int32_t line_num = 0;
    uint32_t line_len = 0;
    char element[128] = {0};

    int32_t file_read_error_flag = 0; //文件读取错误标记
    
  if ( get_measure_type ( p_protocol_gather_case->total_ptl_context[measure_type_data ].measure_prtocol_name )  < 0 )
    {
      p_protocol_gather_case->total_ptl_context[measure_type_data ].csv_ptl = NULL;
      printf ( " get_tab_from_total_csv  measure_prtocol_name  :%s  not exist or not ok \n" , p_protocol_gather_case->total_ptl_context[measure_type_data ].measure_prtocol_name );
      //free ( tmp_protocol );
      return  -1;

    }

    sprintf ( absolute_measure_name,IEC104TOTALPROTOCOL_OWNER"%s/%s",p_protocol_name, p_protocol_gather_case->total_ptl_context[measure_type_data ].measure_prtocol_name );

    tmp_protocol = ( protocol_s_total * ) data_safe_distribution ( sizeof ( protocol_s_total ) ,"get_tab_from_total_csv :" ); //分配协议点表内存

    memset ( tmp_protocol,0,sizeof ( protocol_s_total ) );


    if ( !DirExist ( absolute_measure_name ) )
    {
        p_protocol_gather_case->total_ptl_context[measure_type_data ].csv_ptl = NULL;
        printf ( " get_tab_from_total_csv :%s  not exist  \n" , absolute_measure_name );
        free ( tmp_protocol );
        return  -1;
    }




    fp=fopen ( absolute_measure_name,"r" );
    if ( NULL==fp )
    {
        p_protocol_gather_case->total_ptl_context[measure_type_data ].csv_ptl = NULL;
        printf ( " absolute_measure_name get_tab_from_total_csv :%s  not exist  \n" , absolute_measure_name );
        free ( tmp_protocol );


        printf ( " get_tab_from_total_csv  PATH--%s open  error   \n", absolute_measure_name );
        return 0;
    }

    fseek ( fp, 0, SEEK_END );
    int size = ftell ( fp );

    // printf ( " PATH--%s size = %d\n", PATH,size );
    char *buf = ( char * ) data_safe_distribution ( ( size+10 ),"get_tab_from_total_csv:" );


    fseek ( fp, 0, SEEK_SET );
    fread ( buf,size,1,fp );
    fclose ( fp );

    if ( size )
    {
        for ( i = 0; i < size; ++i )
        {

            if ( buf[i] == '\n' )
            {
                buf[i] = '\0';
                line_num++;
            }
        }

        if ( YX_104 == measure_type_data )
        {
            line_len = get_headline_from_csv ( line_len ,2,16, buf );
            line_num -= 2 ;

            tmp_protocol->yx_flag = 1;
            protocol_data_yx = ( protocol_total_data_s_yx * ) data_safe_distribution ( line_num*sizeof ( protocol_total_data_s_yx ) ,"protocol_data_yx  :" ); //分配协议点表内存


            memset ( protocol_data_yx,0,line_num*sizeof ( protocol_total_data_s_yx ) );

            for ( i = 0; i < line_num; i++ )
            {



                line_len += get_element_from_str ( &buf[line_len],element,',' );
                protocol_data_yx[i].dev_code = strtoul ( ( char * ) element, NULL, 0 );



                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data_yx[i].index_no = strtoul ( ( char * ) element, NULL,0 );


                line_len += ( get_element_from_str ( &buf[line_len],element,',' ) );
                memcpy ( protocol_data_yx[i].com_name ,  element ,strlen ( element ) );

                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data_yx[i].c_slaver_id = strtoul ( ( char * ) element, NULL,0 );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data_yx[i].address = strtoul ( ( char * ) element, NULL, 0 );


                if ( verification_addr_data ( protocol_data_yx[i].address, measure_type_data ) <= 0 )
                {
                    file_read_error_flag = 1;
                    break;
                }

                line_len += ( get_element_from_str ( &buf[line_len],element,',' ) );
                memcpy ( protocol_data_yx[i].yx_name ,  element ,strlen ( element ) );




                line_len += ( get_element_from_str ( &buf[line_len],element,',' ) );
                memcpy ( protocol_data_yx[i].data_name ,  element ,strlen ( element ) );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                //  protocol_data_yx[i].state_data_name= get_id_from_names ( element );
                protocol_data_yx[i].state_data_name = strtoul ( ( char * ) element, NULL,0 );

                line_len +=  get_element_from_str ( &buf[line_len],element,',' );

                int8_t data_sign_ret = get_csv_data_sign ( element );
                if ( data_sign_ret >= 0 )
                {
                    protocol_data_yx[i].data_sign  = data_sign_ret ;
                }

// 	uint8_t  data_post;//数据value位
//     uint32_t fixed_fault_value; //固定故障数据值
                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data_yx[i].data_post = strtoul ( ( char * ) element, NULL,0 );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data_yx[i].fixed_fault_value = strtoul ( ( char * ) element, NULL,0 );



                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data_yx[i].low_limit = strtoul ( ( char * ) element, NULL, 0 );



                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data_yx[i].high_limit = strtoul ( ( char * ) element, NULL, 0 );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                // protocol_data_yx[i].func_Rev = strtoul ( ( char * ) element, NULL, 0 );
                memcpy ( protocol_data_yx[i].func_Rev ,  element ,strlen ( element ) );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );

                memcpy ( protocol_data_yx[i].func_Send ,  element ,strlen ( element ) );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );

                memcpy ( protocol_data_yx[i].func_Fifo ,  element ,strlen ( element ) );

            }

        }
        else
        {

            line_len = get_headline_from_csv ( line_len ,2,19, buf );
            line_num -= 2 ;

            tmp_protocol->yx_flag =  0 ;

            protocol_data = ( protocol_total_data_s * ) data_safe_distribution ( line_num*sizeof ( protocol_total_data_s ), "protocol_data  :" ); //分配协议点表内存


            memset ( protocol_data,0,line_num*sizeof ( protocol_total_data_s ) );

//8706,1,COM2,2,第十二路电流,16,16480,运行信息,F,0.1,A,0x80000000,0x7fffffff,0,0,0,0,0

            for ( i = 0; i < line_num; i++ )
            {

                line_len += ( get_element_from_str ( &buf[line_len],element,',' ) );
                //    protocol_data[i].dev_code = get_id_from_names ( element );
                protocol_data[i].dev_code = strtoul ( ( char * ) element, NULL, 0 );
                //if( protocol_data[i].dev_code > 0xffff)


                line_len += ( get_element_from_str ( &buf[line_len],element,',' ) );
                protocol_data[i].index_no = strtoul ( ( char * ) element, NULL, 0 );

                line_len += ( get_element_from_str ( &buf[line_len],element,',' ) );
                memcpy ( protocol_data[i].com_name ,  element ,strlen ( element ) );

                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data[i].c_slaver_id = strtoul ( ( char * ) element, NULL,0 );


                line_len += ( get_element_from_str ( &buf[line_len],element,',' ) );
                strcpy ( protocol_data[i].data_name ,element );

                line_len += ( get_element_from_str ( &buf[line_len],element,',' ) );
                protocol_data[i].name = strtoul ( ( char * ) element, NULL, 0 );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data[i].address = strtoul ( ( char * ) element, NULL, 0 );

                if ( verification_addr_data ( protocol_data[i].address, measure_type_data ) <= 0 )
                {
                    file_read_error_flag = 1;
                    break;
                }



                line_len += ( get_element_from_str ( &buf[line_len],element,',' ) );
                strcpy ( protocol_data[i].data_info ,element );



                line_len +=  get_element_from_str ( &buf[line_len],element,',' );

                int8_t data_sign_ret = get_csv_data_sign ( element );
                if ( data_sign_ret >= 0 )
                {
                    protocol_data[i].data_sign  = data_sign_ret ;
                }


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data[i].dec_p = atof ( element );

                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data[i].scale_coefficient = atof ( element );   //更新 比例系数


                //  printf(" protocol_data[i].scale_coefficient -----------------%f \n ",protocol_data[i].scale_coefficient);

                if ( protocol_data[i].scale_coefficient   < 0.00000001 && ( protocol_data[i].scale_coefficient    > -0.00000001 ) )
                {
                    protocol_data[i].scale_coefficient = 1.0;
                }

                line_len +=  get_element_from_str ( &buf[line_len],element,',' );

                //data_unit


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data[i].low_limit = strtoul ( ( char * ) element, NULL, 0 );



                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data[i].high_limit = strtoul ( ( char * ) element, NULL, 0 );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data[i].low_limit_1 = strtoul ( ( char * ) element, NULL, 0 );



                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                protocol_data[i].high_limit_1 = strtoul ( ( char * ) element, NULL, 0 );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                memcpy ( protocol_data[i].func_Rev ,  element ,strlen ( element ) );


                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                memcpy ( protocol_data[i].func_Send ,  element ,strlen ( element ) );

                line_len +=  get_element_from_str ( &buf[line_len],element,',' );
                memcpy ( protocol_data[i].func_Fifo ,  element ,strlen ( element ) );

            }

        }

    }

    free ( buf );

    if ( file_read_error_flag )
    {
        p_protocol_gather_case->total_ptl_context[measure_type_data ].csv_ptl = NULL;
        printf ( " total_ptl_context get_tab_from_total_csv :%s  not exist  \n" , absolute_measure_name );
        fprintf ( stderr," %s file content error read   \n" , absolute_measure_name );
        free ( tmp_protocol );
        return  -1;
    }


    if ( tmp_protocol->yx_flag == 0 )
    {

        tmp_protocol->protocol_data =  protocol_data;

        uint16_t  ptl_begin_addr = 0;
        uint16_t ptl_end_addr = 0;
        int32_t ret_getaddr = 0;


        if ( line_num > 0 )
        {
            ptl_begin_addr =  protocol_data[0].address;
            ptl_end_addr = protocol_data[line_num-1].address;
        }

        tmp_protocol->address =   ptl_begin_addr;

        tmp_protocol->len =   ptl_end_addr -  ptl_begin_addr+1;

        tmp_protocol->ptl_data_num = line_num;

        printf ( "tmp_protocol->ptl_data_num   %d  ptl_begin_addr    %d  ptl_end_addr %d tmp_protocol->len  %d \n" ,tmp_protocol->ptl_data_num,    ptl_begin_addr  ,  ptl_end_addr, tmp_protocol->len );

        ret_getaddr = getRemoteBeginEndAddr ( p_protocol_gather_case,  ptl_begin_addr,ptl_end_addr,measure_type_data );
        if ( ret_getaddr < 0 )
        {
            printf ( "error config background protocol config \n" );
        }

        p_protocol_gather_case->total_ptl_context[measure_type_data ].csv_ptl = tmp_protocol;

    }
    else
    {
        printf ( " protocol_data_yx[0].address %d \n", protocol_data_yx[0].address );
        uint16_t  ptl_yx_begin_addr = 0;
        uint16_t ptl_yx_end_addr = 0;
        int32_t ret_get_yx_addr = 0;

        printf ( "ptl_yx_ line_num  %d \n" ,line_num );
        if ( line_num > 0 )
        {
            ptl_yx_begin_addr =  protocol_data_yx[0].address;
            ptl_yx_end_addr = protocol_data_yx[line_num-1].address;
        }

        tmp_protocol->protocol_data_yx =  protocol_data_yx;

        tmp_protocol->address =   ptl_yx_begin_addr;

        tmp_protocol->len =  ptl_yx_end_addr -  ptl_yx_begin_addr+1;

        tmp_protocol->ptl_data_num = line_num;

        printf ( "tmp_protocol->ptl_data_num   %d   tmp_protocol->len  %d ptl_yx_end_addr %d ptl_yx_begin_addr  %d \n" ,tmp_protocol->ptl_data_num,  tmp_protocol->len,ptl_yx_end_addr , ptl_yx_begin_addr );


        ret_get_yx_addr = getRemoteBeginEndAddr ( p_protocol_gather_case,  ptl_yx_begin_addr,ptl_yx_end_addr,measure_type_data );
        if ( ret_get_yx_addr < 0 )
        {
            printf ( "error config background protocol config \n" );
        }


        p_protocol_gather_case->total_ptl_context[measure_type_data ].csv_ptl = tmp_protocol;

    }



    return line_num;

}


/*******************************************************************************
 * Function       : DirExist
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : 文件是否存在,
 * Calls              :access 测试文件存在与否
 * Input             : char* configfilePATH xml文件路径
 * Output          :
 * Return           :存在返回真  不存在返回假
 ********************************************************************************/
int DirExist ( char *FilePath )
{
    return ( access ( FilePath,F_OK ) ==0 );
}



/*******************************************************************************
 * Function       : mkdir_ex
 * Author          : shiran
 * Date              : 2016.05.17
 * Description  : 创建目录（创建文件的上一层的目录路径)
 * Calls              :
 * Input             : char *FilePath  文件路径
 * Output          :
 * Return           :成功返回0  失败返回-1
 ********************************************************************************/
int mkdir_ex ( char *FilePath )
{
    char DirPath[256];
    strcpy ( DirPath,FilePath );

    int i,len=strlen ( DirPath );

    if ( DirPath[len-1]!='/' )
    {
        DirPath[len]='/';
        DirPath[len+1]='\0';
    }

    len=strlen ( DirPath );

    for ( i=1; i<len; i++ )
    {
        if ( DirPath[i]=='/' )
        {
            DirPath[i]='\0';

            if ( access ( DirPath,0 ) !=0 )
            {
                if ( mkdir ( DirPath,0777 ) ==-1 )
                {
                    printf ( "mkdir error: %s\n",DirPath );
                    return -1;
                }
            }
            DirPath[i]='/';
        }
    }
    return 0;
}




