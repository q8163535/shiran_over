#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "io.h"

#include "protocol.h"
#include "protocol_cfg.h"

#include "debugSwitch.h"
#include "xml_csv.h"
#include "dataProcess.h"


int32_t get_tab_from_xml ( dev_protocol *p_dev_ptl,char *p_protocol_name );

int32_t  list_data_read ( xmlChar * list_path,xmlXPathContextPtr context );



/*******************************************************************************
 * Function       : analysis_xml_yx_ptl_points
 * Author          : shiran
 * Date              : 2017-04-18
 * Description  : 解析遥信xml测点,
 * Calls              :
 * Input             : protocol_s    *xml_protocol ,xmlNodeSetPtr nodeset
 * Output          :
 * Return           :文件不为空 存在返回真  不存在返回假
 ********************************************************************************/
static int32_t analysis_xml_yx_ptl_points ( protocol_s    *xml_protocol ,xmlNodeSetPtr nodeset )
{

    protocol_data_s_yx *protocol_data_yx  =  NULL;

    xmlNodePtr tmp_node=NULL;

    int32_t line_num = 0;

    int32_t index_id = 0;

    int32_t max_ptl_data_id = 0;

    int32_t tmp_data_id =  0 ;

    int32_t  node_id = 0;

    xmlNodePtr propNodePtr = NULL;


    if ( !xmlXPathNodeSetIsEmpty ( nodeset ) )
    {

        line_num =  nodeset->nodeNr  ;

        protocol_data_yx = ( protocol_data_s_yx * ) data_safe_distribution ( line_num*sizeof ( protocol_data_s_yx ) , "protocol_data_yx-malloc-line_num" );

        memset ( protocol_data_yx,0,line_num*sizeof ( protocol_data_s_yx ) );

        index_id = 0;

        for ( node_id = 0; node_id < line_num ; node_id++ )
        {

            tmp_node =  nodeset->nodeTab[  node_id ];

            propNodePtr = tmp_node;
            if ( ( !xmlStrcmp ( tmp_node->name, ( const xmlChar * ) "point" ) ) ) //mbmmaster 结点内容解析
            {
                xmlAttrPtr attrPtr = propNodePtr->properties;

                while ( attrPtr != NULL )
                {

                    if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "BitName" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "BitName" );
                        strcpy ( protocol_data_yx[index_id ].yx_name, ( const char * ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "BitName:%s   ", protocol_data_yx[index_id ].yx_name );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataName" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataName" );
                        strcpy ( protocol_data_yx[index_id ].data_name, ( const char * ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataName:%s   ", protocol_data_yx[index_id ].data_name );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataId" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataId" );
                        strcpy ( protocol_data_yx[index_id ].yx_data_id_name, ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "state_data_name:%s   ", protocol_data_yx[index_id ].yx_data_id_name );
#endif

                        tmp_data_id =   strtoul ( ( char * ) protocol_data_yx[index_id ].yx_data_id_name,NULL, 0 );

                        if ( max_ptl_data_id < tmp_data_id )
                        {
                            max_ptl_data_id   = tmp_data_id;
                        }

                        xmlFree ( szAttr );
                    }

                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataPost" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataPost" );
                        strcpy ( protocol_data_yx[index_id ].data_post_name, ( const char * ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "data_post_name %s   ", protocol_data_yx[index_id ].data_post_name );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "FixedFaultValue" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "FixedFaultValue" );
                        strcpy ( protocol_data_yx[index_id ].fixed_fault_value_name, ( const char* ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "fixed_fault_value_name %s     ", protocol_data_yx[index_id ].fixed_fault_value_name );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMin" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMin" );
                        strcpy ( protocol_data_yx[index_id ].low_limit , ( const char * ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataMin %s     ", protocol_data_yx[index_id ].low_limit );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMax" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMax" );
                        strcpy ( protocol_data_yx[index_id ].high_limit , ( const char * ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataMax %s     ", protocol_data_yx[index_id ].high_limit );
#endif
                        xmlFree ( szAttr );
                    }

                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "RevFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "RevFunc" );
                        strcpy ( protocol_data_yx[index_id ].func_Rev , ( const char * ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "RevFunc %s     ", protocol_data_yx[index_id ].func_Rev );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "SendFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "SendFunc" );
                        strcpy ( protocol_data_yx[index_id ].func_Send , ( const char * ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "SendFunc %s    ", protocol_data_yx[index_id ].func_Send );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "FifoFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "FifoFunc" );
                        strcpy ( protocol_data_yx[index_id ].func_Fifo , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( " func_Fifo %s     ", protocol_data_yx[index_id ].func_Fifo );
                        printf ( " \n " );
#endif
                        xmlFree ( szAttr );
                    }


                    attrPtr = attrPtr->next;
                }
                strcpy ( protocol_data_yx[index_id ].data_sign_name , "U" );

                index_id ++;
            }


        }


    }

    xml_protocol->protocol_data_yx =  protocol_data_yx;

    xml_protocol->ptl_data_num = line_num;


    return max_ptl_data_id;

}






/*******************************************************************************
 * Function       : analysis_xml_ycmk_ptl_points
 * Author          : shiran
 * Date              : 2017-04-18
 * Description  : 解析遥测 遥脉 遥控 xml测点,
 * Calls              :
 * Input             : cprotocol_s    *xml_protocol,   xmlNodeSetPtr nodeset
 * Output          :
 * Return           :文件存在返回真  不存在返回假
 ********************************************************************************/
static int32_t analysis_xml_ycmk_ptl_points ( protocol_s    *xml_protocol,   xmlNodeSetPtr nodeset )
{

    protocol_data_s *protocol_data  = NULL;
    xmlNodePtr tmp_node=NULL;
//     xmlXPathObjectPtr result = NULL;

    int32_t line_num = 0;

    int32_t index_id = 0;

    int32_t  node_id = 0;

    int32_t max_ptl_data_id = 0;

    int32_t tmp_data_id =  0 ;

    xmlNodePtr propNodePtr = NULL;


    if ( !xmlXPathNodeSetIsEmpty ( nodeset ) )
    {

        line_num =  nodeset->nodeNr  ;

        //   protocol_data = ( protocol_data_s * ) malloc ( line_num*sizeof ( protocol_data_s ) ); //分配协议点表内存

        protocol_data = ( protocol_data_s * ) data_safe_distribution ( line_num*sizeof ( protocol_data_s )  , "protocol_data-malloc-line_num" );

        memset ( protocol_data,0,line_num*sizeof ( protocol_data_s ) );

        index_id = 0;

        for ( node_id = 0; node_id < line_num ; node_id++ )
        {

            tmp_node =  nodeset->nodeTab[  node_id ];
            propNodePtr = tmp_node ;

            if ( ( !xmlStrcmp ( tmp_node->name, ( const xmlChar * ) "point" ) ) ) //mbmmaster 结点内容解析
            {
                xmlAttrPtr attrPtr = propNodePtr->properties;


                while ( attrPtr != NULL )
                {

                    if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataName" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataName" );
                        strcpy ( protocol_data[index_id ].data_name, ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "data_name:  %s   ", protocol_data[index_id ].data_name );
#endif
                        xmlFree ( szAttr );

                    }

                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataId" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataId" );

                        strcpy ( protocol_data[index_id ].data_id , ( const char * )  szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "data_id:  %s     ", protocol_data[index_id ].data_id );
#endif
                        xmlFree ( szAttr );

                        tmp_data_id = atoi ( protocol_data[index_id ].data_id );

                        if ( max_ptl_data_id < tmp_data_id )
                        {
                            max_ptl_data_id   = tmp_data_id;
                        }
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataUnit" ) )   //单位 名称
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataUnit" );  //单位名称
                        strcpy ( protocol_data[index_id ].data_unit, ( const char * ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                        printf ( "data_unit:%s     ", protocol_data[index_id ].data_unit );
#endif

                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataInfo" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataInfo" );
                        strcpy ( protocol_data[index_id ].data_info , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "data_info:%s     ", protocol_data[index_id ].data_info );
#endif
                        xmlFree ( szAttr );
                    }

                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataSign" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataSign" );
                        strcpy ( protocol_data[index_id ].data_sign , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataSign %s     ", protocol_data[index_id ].data_sign );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataDecimal" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataDecimal" );
                        strcpy ( protocol_data[index_id ].dec_p_arr, ( const char* ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataDecimal %s     ", protocol_data[index_id ].dec_p_arr );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataScaleCoefficient" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataScaleCoefficient" );
                        strcpy ( protocol_data[index_id ].scale_coefficient_arr, ( const char* ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataScaleCoefficient %s     ", protocol_data[index_id ].scale_coefficient_arr );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMin" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMin" );
                        strcpy ( protocol_data[index_id ].low_limit , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataMin %s     ", protocol_data[index_id ].low_limit );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMax" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMax" );
                        strcpy ( protocol_data[index_id ].high_limit , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataMax %s     ", protocol_data[index_id ].high_limit );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMin1" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMin1" );
                        strcpy ( protocol_data[index_id ].low_limit_1 , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataMin1 %s     ", protocol_data[index_id ].low_limit_1 );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMax1" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMax1" );
                        strcpy ( protocol_data[index_id ].high_limit_1 , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "DataMax1 %s     ", protocol_data[index_id ].high_limit_1 );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "RevFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "RevFunc" );
                        strcpy ( protocol_data[index_id ].func_Rev , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "RevFunc %s     ", protocol_data[index_id ].func_Rev );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "SendFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "SendFunc" );
                        strcpy ( protocol_data[index_id ].func_Send , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "SendFunc %s     ", protocol_data[index_id ].func_Send );
#endif
                        xmlFree ( szAttr );
                    }
                    else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "FifoFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "FifoFunc" );
                        strcpy ( protocol_data[index_id ].func_Fifo , ( const char * ) szAttr );

#if DEBUG_XML_PRINT_OPEN
                        printf ( "FifoFunc %s     \n", protocol_data[index_id ].func_Fifo );
#endif
                        xmlFree ( szAttr );
                    }

                    attrPtr = attrPtr->next;

                }

                index_id ++;
            }


        }

    }


    xml_protocol->protocol_data =  protocol_data;

    xml_protocol->ptl_data_num = line_num;


    return max_ptl_data_id;


}








/*******************************************************************************
 * Function       : get_tab_from_xml
 * Author          : shiran
 * Date              : 2017.04.18
 * Description  :
 * Calls              :access 测试文件存在与否
 * Input             : char* configfilePATH xml文件路径
 * Output          :
 * Return           :存在返回真  不存在返回假
 ********************************************************************************/
int32_t get_tab_from_xml ( dev_protocol *p_dev_ptl,char *p_protocol_name )
{


    xmlDocPtr doc;
    xmlNodePtr curNode;
    xmlNodePtr Node1;
    xmlNodePtr Node2;

    xmlXPathContextPtr context= NULL;

    int32_t max_ptl_data_id = 0;

    int32_t tmp_data_id =  0 ;

    int32_t measure_type_data = 0;

    protocol_s    *tmp_protocol  =  NULL;

    char *szDocName = NULL;

    char ptl_ver[100 ] = { 0 };


    szDocName = p_protocol_name;
    printf ( "get_tab_from_xml szDocName:  %s \n",szDocName );

    doc = xmlReadFile ( szDocName,"UTF-8",XML_PARSE_RECOVER );
    if ( NULL == doc )
    {
        printf ( "string_xml empty !\n" );
        return -1;
    }


    context = xmlXPathNewContext ( doc );
    if ( context == NULL )
    {
        printf ( "context is NULL\n" );
        return 0;
    }

    //读取root节点
    curNode = xmlDocGetRootElement ( doc );
    if ( NULL == curNode )
    {
        fprintf ( stderr,"string_xml no root !\n" );
        xmlFreeDoc ( doc );
        return -1;
    }


    //解析root下子节点
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

        if ( ( !xmlStrcmp ( Node1->name, ( const xmlChar * ) "protocol" ) ) ) //mbmmaster 结点内容解析
        {
            //查找属性
            propNodePtr = Node1;
            xmlAttrPtr attrPtr = propNodePtr->properties;
            while ( attrPtr != NULL )
            {
                if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "ProtocolCode" ) )
                {
                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "ProtocolCode" );

#if DEBUG_XML_PRINT_OPEN
                    uint32_t ptl_code = 0;
                    ptl_code = strtoul ( ( const char * ) szAttr, NULL, 0 );
                    printf ( "ptl_code = 0x%0x, ", ptl_code );
#endif

                    xmlFree ( szAttr );
                }
                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "ProtocolVer" ) )
                {
                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "ProtocolVer" );
                    strcpy ( ptl_ver, ( const char * ) szAttr );
#if DEBUG_XML_PRINT_OPEN
                    printf ( "ptl_ver = %s\n", ptl_ver );
#endif
                    xmlFree ( szAttr );
                }
                attrPtr = attrPtr->next;
            }

            //node2节点 是node1的子节点
            Node2 = Node1->xmlChildrenNode;
            while ( Node2 != NULL )
            {

                propNodePtr = Node2;
                if ( ( !xmlStrcmp ( Node2->name, ( const xmlChar * ) "table" ) ) ) //mbmmaster 结点内容解析
                {
                    char cmd_name[10] = { 0 };
                    int32_t tab_id  = 0;


                    char xpath_point[100 ]   =  { 0 };
                    char xpath_tab[100 ]      =   { 0 };

                    //查找每个协议的属性
                    xmlAttrPtr attrPtr = propNodePtr->properties;

                    while ( attrPtr != NULL )
                    {
                        if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "id" ) )
                        {
                            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "id" );
                            tab_id = strtoul ( ( const char * ) szAttr, NULL, 0 );
                            xmlFree ( szAttr );
                        }
                        else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "cmd" ) )
                        {
                            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "cmd" );

                            strcpy ( cmd_name , ( const char * ) szAttr );

                            measure_type_data =  get_measure_type ( cmd_name );
                            if ( measure_type_data < 0 )
                            {
                                break;
                            }

                            // 分配协议点表内存
                            tmp_protocol =  data_safe_distribution ( sizeof ( protocol_s ) , "cmd-malloc-line_num" );

                            memset ( tmp_protocol,0,sizeof ( protocol_s ) );

                            xmlFree ( szAttr );

                        }

                        attrPtr = attrPtr->next;

                    }

                    sprintf ( xpath_tab ,"/root/protocol/table[@id='%d' and @cmd='%s'  ]",tab_id,cmd_name );

                    sprintf ( xpath_point ,"%s/register/point",xpath_tab );

                    printf ( "xpath_point %s \n",xpath_point );


                    xmlXPathObjectPtr result = NULL;

                    xmlChar *xpath_tmp = ( xmlChar * ) xpath_point;
                    result = xmlXPathEvalExpression ( xpath_tmp, context );

                    if ( result == NULL )
                    {
                        printf ( "xmlXPathEvalExpression measure_standard  NULL\n" );
                        xmlXPathFreeObject ( result );
                        return 0;
                    }

                    if ( measure_type_data == YX_104 )
                    {
                        tmp_protocol->yx_flag = 1;
                        tmp_data_id =   analysis_xml_yx_ptl_points ( tmp_protocol,   result->nodesetval );
                    }
                    else
                    {
                        tmp_protocol->yx_flag = 0 ;
                        tmp_data_id=    analysis_xml_ycmk_ptl_points ( tmp_protocol,   result->nodesetval );
                    }
                    xmlXPathFreeObject ( result );

                    if ( max_ptl_data_id < tmp_data_id )
                    {
                        max_ptl_data_id   = tmp_data_id;
                    }

                    p_dev_ptl[measure_type_data].csv_ptl = tmp_protocol;


                    strcpy ( p_dev_ptl[measure_type_data].ProtocolName,p_protocol_name );

                }

                Node2 =  Node2->next;
            }

        }

        Node1 =  Node1->next;

    }


    return max_ptl_data_id;

}


/*******************************************************************************
 * Function       : list_data_read
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : list_data_read
 * Calls              :
 * Input             :
 * Output          :
 * Return           :void
 ********************************************************************************/

int32_t  list_data_read ( xmlChar * list_path,xmlXPathContextPtr context )
{
    xmlNodePtr tmp_node=NULL;
    xmlXPathObjectPtr result = NULL;
    xmlNodeSetPtr nodeset = NULL;

    int32_t  node_id = 0;

    result = xmlXPathEvalExpression ( list_path, context );

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


    nodeset = result->nodesetval  ;

    int32_t node_num =  nodeset->nodeNr    ;

    for ( node_id = 0; node_id < node_num ; node_id++ )
    {

        tmp_node =  nodeset->nodeTab[  node_id ];

        xmlNodePtr children_nodes = NULL;

        xmlNodePtr  propNodePtr =NULL;

        int32_t list_var = 0;

        int32_t bit_post = 0;


        if ( ( !xmlStrcmp ( tmp_node->name,BAD_CAST "enumeration" ) ) )   //commport 结点内容解析
        {

            children_nodes = tmp_node->xmlChildrenNode;

        }

        while ( children_nodes != NULL )
        {

            if ( ( !xmlStrcmp ( children_nodes->name , BAD_CAST "list" ) ) )   //commport 结点内容解析
            {
                propNodePtr = children_nodes ;

                xmlAttrPtr node_attr_ptr = propNodePtr->properties;

                while ( node_attr_ptr != NULL )
                {

                    if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "var" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "var" );

                        list_var = strtoul ( ( char * ) szAttr,NULL, 0 );

                        printf ( "  --  list_var   %d      \n   ", list_var );

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "DataInfo" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataInfo" );

                        printf ( "  --    DataInfo %s   \n   ",szAttr );

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "RevFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "RevFunc" );

                        printf ( "  --    RevFunc %s   \n   ",szAttr );

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "SendFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "SendFunc" );

                        printf ( "  --    SendFunc %s   \n   ",szAttr );

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "FifoFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "FifoFunc" );

                        printf ( "  --    FifoFunc %s   \n   ",szAttr );

                        xmlFree ( szAttr );
                    }

                    node_attr_ptr = node_attr_ptr->next;

                }

                xmlChar* sz_content_tmp  =  NULL;

                sz_content_tmp = xmlNodeGetContent ( children_nodes );

                char list_name[30] = { 0 };

                strcpy ( list_name, ( char * ) sz_content_tmp );

                printf ( "  --    list_name %s   \n   ",list_name );

            }
            else   if ( ( !xmlStrcmp ( children_nodes->name,BAD_CAST "BitName" ) ) )   //commport 结点内容解析
            {

                propNodePtr = children_nodes;

                xmlAttrPtr node_attr_ptr = propNodePtr->properties;

                while ( node_attr_ptr != NULL )
                {

                    if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "BitPost" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "BitPost" );

                        bit_post = strtoul ( ( char * ) szAttr,NULL, 0 );

                        printf ( "  --  BitPost   %d      \n   ", bit_post );

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "DataInfo" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataInfo" );

                        printf ( "  --    DataInfo %s   \n   ",szAttr );

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "RevFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "RevFunc" );

                        printf ( "  --    RevFunc %s   \n   ",szAttr );

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "SendFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "SendFunc" );

                        printf ( "  --    SendFunc %s   \n   ",szAttr );

                        xmlFree ( szAttr );
                    }
                    else   if ( !xmlStrcmp ( node_attr_ptr->name, BAD_CAST "FifoFunc" ) )
                    {
                        xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "FifoFunc" );

                        printf ( "  --    FifoFunc %s   \n   ",szAttr );

                        xmlFree ( szAttr );
                    }

                    node_attr_ptr = node_attr_ptr->next;

                }

                xmlChar*  sz_content_tmp  =  NULL;

                sz_content_tmp = xmlNodeGetContent ( children_nodes );

                char bit_name[30] = { 0 };

                strcpy ( bit_name, ( char * ) sz_content_tmp );

                printf ( "  --    bit_name %s   \n   ",bit_name );

            }
            children_nodes =children_nodes->next ;
        }
    }

    xmlXPathFreeObject ( result );

    return 1;



}
