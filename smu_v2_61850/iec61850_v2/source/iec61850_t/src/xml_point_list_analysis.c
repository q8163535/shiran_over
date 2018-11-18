#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <stdio.h>
#include <stdlib.h>


#include "../../io.h"
#include <string.h>

#include "debugSwitch.h"
#include "xml_csv.h"


uint32_t get_tab_from_xml ( uint16_t dev_code,char *p_protocol_name )
{
    protocol_data_s *p_ptl_data = NULL;

    int32_t file_max_data_id = 0;

    int8_t tmp_data_sign = 0;
    
    
        int8_t tmp_data_type = 0;

    xmlDocPtr doc;
    xmlNodePtr curNode;
    xmlNodePtr Node1;
    xmlNodePtr Node2;


    xmlXPathContextPtr context= NULL;


    char *szDocName = NULL;
    uint32_t ptl_code = 0;
    char ptl_ver[100 ] = { 0 };

    int32_t index_id = 0;

    int32_t proCmd = 0;


    szDocName = p_protocol_name;
    doc = xmlReadFile ( szDocName,"UTF-8",XML_PARSE_RECOVER );
    if ( NULL == doc )
    {
        DEBUG_FAULT ( "string_xml empty !\n" );
        return -1;
    }


    context = xmlXPathNewContext ( doc );
    if ( context == NULL )
    {
        DEBUG_FAULT ( "context is NULL\n" );
        return 0;
    }

    //读取root节点
    curNode = xmlDocGetRootElement ( doc );
    if ( NULL == curNode )
    {
        fprintf ( stderr,"string_xml no root !\n" );
	DEBUG_FAULT("string_xml no root !\n" );
        xmlFreeDoc ( doc );
        return -1;
    }


    //解析root下子节点
    if ( xmlStrcmp ( curNode->name, BAD_CAST "root" ) )
    {
        fprintf ( stderr,"document of the wrong type, root node != root" );
	DEBUG_FAULT("document of the wrong type, root node != root" );
        xmlFreeDoc ( doc );
        return -1;
    }

    curNode = curNode->xmlChildrenNode;
    xmlNodePtr propNodePtr = curNode;
    Node1 = curNode;

    while ( Node1 != NULL )
    {
        DEBUG_CONFIG ( " Node1->name %s \n ",  Node1->name );

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
                    ptl_code = strtoul ( ( const char * ) szAttr, NULL, 0 );
                    DEBUG_CONFIG ( "ptl_code = 0x%0x\n", ptl_code );
                    xmlFree ( szAttr );
                }
                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "ProtocolVer" ) )
                {
                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "ProtocolVer" );
                    strcpy ( ptl_ver, ( const char * ) szAttr );
                    DEBUG_CONFIG ( "ptl_ver = %s\n", ptl_ver );
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
                    //查找每个协议的属性
                    xmlAttrPtr attrPtr = propNodePtr->properties;

                    int32_t tab_id  = 0;

                    while ( attrPtr != NULL )
                    {
                        if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "id" ) )
                        {
                            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "id" );
                            tab_id = strtoul ( ( const char * ) szAttr, NULL, 0 );
                            xmlFree ( szAttr );
#if 1
                            DEBUG_CSV_PRINT ( "id %d \n",tab_id );
#endif
                        }

                        else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "cmd" ) )
                        {
                            xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "cmd" );

                            proCmd = strtoul ( ( char * ) szAttr, NULL, 0 );
#if 1
                            DEBUG_CSV_PRINT ( "proCmd %d \n",proCmd );
#endif

                            xmlFree ( szAttr );
                        }

                        attrPtr = attrPtr->next;

                    }

                    char xpath_point[100 ]   =  { 0 };

                    char xpath_tab[100 ]      =   { 0 };

                   // char xpath_enumeration[100 ]  =  { 0 };


                    sprintf ( xpath_tab ,"/root/protocol/table[@id='%d' and @cmd='%d'  ]",tab_id,proCmd );

                    sprintf ( xpath_point ,"%s/register/point",xpath_tab );


                    xmlNodePtr tmp_node=NULL;
                    xmlXPathObjectPtr result = NULL;
                    xmlNodeSetPtr nodeset = NULL;


                    xmlChar *xpath_tmp = ( xmlChar * ) xpath_point;
                    result = xmlXPathEvalExpression ( xpath_tmp, context );

                    if ( result == NULL )
                    {
                        DEBUG_FAULT ( "xmlXPathEvalExpression   NULL\n" );
                        return 0;
                    }

                    if ( xmlXPathNodeSetIsEmpty ( result->nodesetval ) )
                    {

                        xmlXPathFreeObject ( result );

                        return 0;
                    }




                    int32_t  addr_min =  0 ;

                    int32_t addr_max = 0;

                    int32_t tmp_data_id =  0 ;

                    int32_t max_ptl_data_id = 0;


                    int32_t  node_id = 0;
                    nodeset = result->nodesetval  ;

                    int32_t line_num =  nodeset->nodeNr  ;
		    
#if 1
                    DEBUG_CONFIG ( "--------------------line_num %d \n",line_num );
#endif

                    p_ptl_data = ( protocol_data_s * ) malloc ( line_num*sizeof ( protocol_data_s ) ); //分配协议点表内存

                    memset ( p_ptl_data,0,line_num*sizeof ( protocol_data_s ) );

                    for ( node_id = 0; node_id < line_num ; node_id++ )
                    {
                        index_id = node_id;
                        tmp_node =  nodeset->nodeTab[  node_id ];

                        propNodePtr = tmp_node ;

                        if ( ( !xmlStrcmp ( tmp_node->name, ( const xmlChar * ) "point" ) ) ) //mbmmaster 结点内容解析
                        {
                            xmlAttrPtr attrPtr = propNodePtr->properties;
			    
			 //初始化比例系数为0
			    p_ptl_data[index_id ].scale_coefficient = 1;


                            while ( attrPtr != NULL )
                            {
                                if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "id" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "id" );
                                   
#if  1
				DEBUG_CSV_PRINT ( "point_id: %s     ", szAttr );
#endif
                                    xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataName" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataName" );

                                    strcpy ( p_ptl_data[index_id ].data_name, ( const char * ) szAttr );
				    
#if 1
                                    DEBUG_CSV_PRINT ( "DataName %s     ",  p_ptl_data[index_id ].data_name );
#endif

                                    xmlFree ( szAttr );

                                }

                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataId" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataId" );

                                    p_ptl_data[ index_id ].data_id = strtoul ( ( const char * ) szAttr, NULL, 0 );

                                    xmlFree ( szAttr );

                                    tmp_data_id =p_ptl_data[index_id ].data_id ;


                                    if ( max_ptl_data_id < tmp_data_id )
                                    {
                                        max_ptl_data_id   = tmp_data_id;
                                    }
#if 1
                                    DEBUG_CSV_PRINT ( "tmp_data_id %d max_ptl_data_id  %d    ", tmp_data_id,max_ptl_data_id );
#endif

                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataAddress" ) )   //单位 名称
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataAddress" );  //单位名称

                                    p_ptl_data[ index_id ].data_address = strtoul ( ( const char * ) szAttr, NULL, 0 );

                                    int32_t  tmp_addr =   p_ptl_data[ index_id ].data_address;



                                    if ( addr_max < tmp_addr )
                                    {
                                        addr_max   = tmp_addr;
                                    }
                                    if ( ( addr_min <= 0 ) || ( addr_min >  tmp_addr ) )
                                    {
                                        addr_min  = tmp_addr;
                                    }


#if 1
                                    DEBUG_CSV_PRINT ( "data_address: %d     ", p_ptl_data[index_id ].data_address );
#endif
                                    xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataLen" ) )   //单位 名称
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataLen" );  //单位名称

                                    p_ptl_data[ index_id ].data_byte_num = strtoul ( ( const char * ) szAttr, NULL, 0 );

#if 1
                                    DEBUG_CSV_PRINT ( " data_type:%d     ",p_ptl_data[ index_id ].data_type );
#endif
                                    xmlFree ( szAttr );
                                }
                               else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataType" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataType" );

                                    tmp_data_type = get_xml_point_data_type_config ( ( char * ) szAttr ) ;

#if 1
                                     DEBUG_CSV_PRINT ( "data_type:%d     ", p_ptl_data[index_id ].data_type );
#endif

                                    xmlFree ( szAttr );
                                }
                                
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataSign" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataSign" );
                               //     p_ptl_data[ index_id ].data_sign = strtoul ( ( const char * ) szAttr, NULL, 0 );

                                    tmp_data_sign = get_xml_point_data_sign_config ( ( char * ) szAttr ) ;
				    
                                    if ( tmp_data_sign >= 0 )
                                    {
                                      p_ptl_data[ index_id ].data_sign = tmp_data_sign;
                                    }

#if 1
                                     DEBUG_CSV_PRINT ( "data_sign:%d     ", p_ptl_data[index_id ].data_sign );
#endif

                                    xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataInfo" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataInfo" );

                                    strcpy ( p_ptl_data[index_id ].data_info , ( const char * ) szAttr );
#if 1
                                    DEBUG_CSV_PRINT ( "%s     \n", p_ptl_data[index_id ].data_info );
#endif

                                    xmlFree ( szAttr );
                                }


                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataDecimal" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataDecimal" );
                                    p_ptl_data[ index_id ].dec_p =atof ( ( const char * ) szAttr );

#if 1
                                    DEBUG_CSV_PRINT ( "dec_p:%f    ", p_ptl_data[index_id ].dec_p );
#endif

                                    xmlFree ( szAttr );
                                }
                              else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataScaleCoefficient" ) )
                                {


                                  xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataScaleCoefficient" );
				  
                                  p_ptl_data[ index_id ].scale_coefficient =atof ( ( const char * ) szAttr );
				  
				  if(    (  p_ptl_data[ index_id ].scale_coefficient <0.0000001 )  &&  (   p_ptl_data[ index_id ].scale_coefficient > -0.0000001 )  )
				  {
				     p_ptl_data[ index_id ].scale_coefficient =1;
				  }


#if 1
                                  DEBUG_CSV_PRINT ( "scale_coefficient:%f    ", p_ptl_data[index_id ].scale_coefficient );
#endif

                                  xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMin" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMin" );
				  if( p_ptl_data[ index_id ].data_sign == SIGN_U )
				  {
				       p_ptl_data[ index_id ].low_limit1_uint = strtoul ( ( const char * ) szAttr, NULL, 0 );
				  }
				  else 
				  {
                                    p_ptl_data[ index_id ].low_limit1 = strtoul ( ( const char * ) szAttr, NULL, 0 );
				  }

#if 1
                                    DEBUG_CSV_PRINT ( "low_limit1:%d     ", p_ptl_data[index_id ].low_limit1 );
#endif

                                    xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMax" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMax" );
//                                     p_ptl_data[ index_id ].high_limit1 = strtoul ( ( const char * ) szAttr, NULL, 0 );
				    
				  if( p_ptl_data[ index_id ].data_sign == SIGN_U )
				  {
				        p_ptl_data[ index_id ].high_limit1_uint = strtoul ( ( const char * ) szAttr, NULL, 0 );
				  }
				  else 
				  {
				      p_ptl_data[ index_id ].high_limit1 = strtoul ( ( const char * ) szAttr, NULL, 0 );
				  }


#if 1
                                    DEBUG_CSV_PRINT ( "high_limit1:%d     ", p_ptl_data[index_id ].high_limit1 );
#endif

                                    xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMin1" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMin1" );
//                                     p_ptl_data[ index_id ].low_limit2 = strtoul ( ( const char * ) szAttr, NULL, 0 );
				    
				       
				  if( p_ptl_data[ index_id ].data_sign == SIGN_U )
				  {
				        p_ptl_data[ index_id ].low_limit2_uint = strtoul ( ( const char * ) szAttr, NULL, 0 );
				  }
				  else 
				  {
                                p_ptl_data[ index_id ].low_limit2 = strtoul ( ( const char * ) szAttr, NULL, 0 );
				  }

#if 1
                                    DEBUG_CSV_PRINT ( "low_limit2: %d    ", p_ptl_data[index_id ].low_limit2 );
#endif

                                    xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "DataMax1" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "DataMax1" );
//                                     p_ptl_data[ index_id ].high_limit2 = strtoul ( ( const char * ) szAttr, NULL, 0 );
				    
				       
				  if( p_ptl_data[ index_id ].data_sign == SIGN_U )
				  {
				        p_ptl_data[ index_id ].high_limit2_uint = strtoul ( ( const char * ) szAttr, NULL, 0 );
				  }
				  else 
				  {
                                  p_ptl_data[ index_id ].high_limit2 = strtoul ( ( const char * ) szAttr, NULL, 0 );
				  }

#if 1
                                    DEBUG_CSV_PRINT ( "high_limit2 :%d     ", p_ptl_data[index_id ].high_limit2 );
#endif

                                    xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "RevFunc" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "RevFunc" );

                                    strcpy ( p_ptl_data[index_id ].func_Rev , ( const char * ) szAttr );

#if 1
                                    DEBUG_CSV_PRINT ( "func_Rev :%s     ", p_ptl_data[index_id ].func_Rev );
#endif

                                    xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "SendFunc" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "SendFunc" );

                                    strcpy ( p_ptl_data[index_id ].func_Send , ( const char * ) szAttr );

#if 1
                                    DEBUG_CSV_PRINT ( "func_Send :%s     ", p_ptl_data[index_id ].func_Send );
#endif

                                    xmlFree ( szAttr );
                                }
                                else if ( !xmlStrcmp ( attrPtr->name, BAD_CAST "FifoFunc" ) )
                                {
                                    xmlChar* szAttr = xmlGetProp ( propNodePtr,BAD_CAST "FifoFunc" );

                                    strcpy ( p_ptl_data[index_id ].func_Fifo , ( const char * ) szAttr );

#if 1
                                    DEBUG_CSV_PRINT ( "func_Fifo :%s     \n", p_ptl_data[index_id ].func_Fifo );
#endif

                                    xmlFree ( szAttr );
                                }

                                attrPtr = attrPtr->next;


                            }
                            
                          p_ptl_data[index_id ].data_type=    get_xml_point_data_type_context ( tmp_data_sign, tmp_data_type ) ;
                           
                        }

                    }



                    g_protocol[ g_protocl_num ].protocol_data = p_ptl_data;

                    g_protocol[ g_protocl_num ].address =   addr_min;



                    if ( proCmd == 0x01 )
                    {

                        if ( p_ptl_data[line_num-1].data_byte_num == 4 )
                        {
                            g_protocol[ g_protocl_num ].len =  addr_max -  addr_min+4*8 ;
                        }
                        else
                        {
                            g_protocol[ g_protocl_num ].len =   addr_max -  addr_min+2*8 ;
                        }
                    }
                    else
                    {

                        if ( p_ptl_data[line_num-1].data_byte_num == 4 )
                        {
                            g_protocol[ g_protocl_num ].len =   addr_max -  addr_min+2;
                        }
                        else
                        {
                            g_protocol[ g_protocl_num ].len =   addr_max -  addr_min+1;
                        }
                    }


                    g_protocol[ g_protocl_num ].ptl_min_addr = addr_min;

                    g_protocol[ g_protocl_num ].ptl_max_addr = addr_max;




                    g_protocol[ g_protocl_num ].max_ptl_data_id = max_ptl_data_id;

                    g_protocol[ g_protocl_num ].ptl_data_num = line_num;

                    g_protocol[ g_protocl_num ].cmd =  proCmd;

                    g_protocol[ g_protocl_num ].ptl_dev_code = dev_code;

                    g_protocl_num ++;

                    DEBUG_CONFIG ( "protocol_max_ptl_data_id %d \n",max_ptl_data_id );


                    if ( file_max_data_id < max_ptl_data_id )
                    {
                        file_max_data_id   = max_ptl_data_id;
                    }


                }
                Node2 = Node2->next;

            }



        }


        Node1 = Node1->next ;
    }


    DEBUG_CONFIG ( " file_max_data_id %d \n" ,file_max_data_id );

    return file_max_data_id;



}

#if 0

static uint32_t get_XML_tab_num ( char *ptl_name )
{
    xmlDocPtr doc;
    xmlNodePtr curNode;
    xmlNodeSetPtr nodeset;
    xmlXPathObjectPtr result;
    xmlXPathContextPtr context;
    char *szDocName = NULL;

    szDocName = ptl_name;
    doc = xmlReadFile ( szDocName,"UTF-8",XML_PARSE_RECOVER );
    if ( NULL == doc )
    {
        printf ( "string_xml empty !\n" );
        return -1;
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

    //获取该xml的协议个数
    context = xmlXPathNewContext ( doc );
    xmlChar *xpath = ( xmlChar * ) "/root/protocol/table";

    uint32_t protocol_num = 0;

    result = xmlXPathEvalExpression ( xpath, context );
    if ( result )
    {
        nodeset = result->nodesetval;
        if ( nodeset->nodeNr )
        {
            protocol_num = nodeset->nodeNr;
        }
    }

    xmlXPathFreeObject ( result );
    xmlXPathFreeContext ( context );
    xmlFreeDoc ( doc );

    return protocol_num;
}
#endif
