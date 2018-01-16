
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#define MY_ENCODING "UTF-8"


/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :xml_create_export_csv.c
* Author :shiran
* Date :2017-05-06
* Description : 解析XML文件创建CSV 文件
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
*                    宏定义-常量定-静态全局变量
*************************************************************************/

/************************************************************************
*                   全局变量
*************************************************************************/
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
int32_t mkdir_ex ( char *FilePath )
{

  char DirPath[256];
  int32_t i= 0 ;
  int32_t  len  = 0;
  int32_t ret_mk = 0;


  strcpy ( DirPath,FilePath );

  len=strlen ( DirPath );





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
                  ret_mk = -1;
                  break;

                }
            }
          DirPath[i]='/';
        }
    }

  return ret_mk;
}



/************************************************************************
*                   局部函数原型（声明）
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
unsigned int  get_element_from_str ( char *buf,char *element,char separator )
{
  unsigned int i = 0;
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

#define YC_104 0
#define YK_104 1
#define YT_104 2
#define YX_104 3
#define YM_104 4

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
 * Function       : transform_import_csv
 * Author          : shiran
 * Date              : 2016.02.17
 * Description  : 转换own为export 的csv输出
 * Calls              :access 测试文件存在与否
 * Input             :  char *p_protocol_name, char *measure_csv_name, int32_t measure_type_id
 * Output          :
 * Return           :
 ********************************************************************************/



char data_head_name[100][100] ;


int test(  char *p_protocol_name)
{
  


#if 0

  char csv_data_name[100]= { 0 };

  int n = 0;

  while ( 1 )
    {
      if ( p_protocol_name[n]  !=  '.' )
        {
          // uri++;
          csv_data_name[ n ] = p_protocol_name[ n ];
          n ++;
        }
      else
        {
          break;
        }
    }

  csv_data_name[n]='.';
  csv_data_name[n+1]='x';
  csv_data_name[n+2]='m';
  csv_data_name[n+3]='l';


  int rc;




  /* Create a new XmlWriter for uri, with no compression. */
    xmlTextWriterPtr writer = xmlNewTextWriterFilename ( csv_data_name, 0 );


  rc = xmlTextWriterStartDocument ( writer, NULL, MY_ENCODING, NULL );


  /* Add an attribute with name "version" and value "1.0" to ORDER. */
  rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST "version",
                                     BAD_CAST "1.0" );



  /* Start an element named "EXAMPLE". Since thist is the first
   * element, this will be the root element of the document. */
  //1
  rc = xmlTextWriterStartElement ( writer, BAD_CAST "root" );



  //2
  /* Start an element named "EXAMPLE". Since thist is the first
  * element, this will be the root element of the document. */
  rc = xmlTextWriterStartElement ( writer, BAD_CAST "protocol" );



  /* Add an attribute with name "xml:lang" and value "de" to ORDER. */
  rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST "ProtocolCode",
                                     BAD_CAST "0x01000000" );



  /* Add an attribute with name "xml:lang" and value "de" to ORDER. */
  rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST "ProtocolVer",
                                     BAD_CAST "1.0.0.0" );



  //<protocol ProtocolCode="0x01000000" ProtocolVer="1.0.0.0">
  //	<table id="1" cmd="yc">
  //		<register type="R">

  /* Start an element named "ORDER" as child of EXAMPLE. */

  //3
  rc = xmlTextWriterStartElement ( writer, BAD_CAST "register" );


  /* Add an attribute with name "xml:lang" and value "de" to ORDER. */
  rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST "type",
                                     BAD_CAST "R" );

#endif


  FILE *owner_fp=NULL;



  unsigned int i = 0;
  int32_t line_num = 0;
  unsigned int line_len = 0;
  char element[128] = {0};


  owner_fp=fopen ( p_protocol_name,"r" );
  
  if ( NULL==owner_fp )
    {
      printf ( " transform_import_csv open  :%s  not exist  \n" , p_protocol_name );
      return 0;
    }

  fseek ( owner_fp, 0, SEEK_END );
  
  int size = ftell ( owner_fp );

  // printf ( " PATH--%s size = %d\n", PATH,size );
  char *buf = ( char * ) malloc ( ( size+10 ) );
  
  memset( buf,0, size+10);


  fseek ( owner_fp, 0, SEEK_SET );
  
  int read_num= fread ( buf,size,1,owner_fp );
  
  printf("size  %d  read_num %d \n",size,read_num);
  
  fclose ( owner_fp );


  int head_num = 0;


  memset ( data_head_name,0,100*100 );
  char temp_elem[1024] = { 0 };

  if ( size )
    {
      i=0;
      while ( buf[i] != '\n' )
        {
          temp_elem[i]  =  buf[i];
	  i++;
        }

      printf ( "temp_elem i %d %s \n", i,temp_elem );
      i=0;
      for ( ;  i<1024; i++ )
        {
          if ( ( temp_elem[i] == '\n' ) || ( temp_elem[i] == '\r' ) )
            {
              temp_elem[i] = '\0';
            }
        }

    }


  return 0;
}


 unsigned int transform_import_csv ( char *p_protocol_name )
{


#if 1

  char csv_data_name[100]= { 0 };

  int n = 0;

  while ( 1 )
    {
      if ( p_protocol_name[n]  !=  '.' )
        {
          csv_data_name[ n ] = p_protocol_name[ n ];
          n ++;
        }
      else
        {
          break;
        }
    }

    
  csv_data_name[n]='.';
  csv_data_name[n+1]='x';
  csv_data_name[n+2]='m';
  csv_data_name[n+3]='l';


  int rc;




  /* Create a new XmlWriter for uri, with no compression. */
    xmlTextWriterPtr writer = xmlNewTextWriterFilename ( csv_data_name, 0 );


  rc = xmlTextWriterStartDocument ( writer, NULL, MY_ENCODING, NULL );


  /* Add an attribute with name "version" and value "1.0" to ORDER. */
  rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST "version",
                                     BAD_CAST "1.0" );



  /* Start an element named "EXAMPLE". Since thist is the first
   * element, this will be the root element of the document. */
  //1
  rc = xmlTextWriterStartElement ( writer, BAD_CAST "root" );



  //2
  /* Start an element named "EXAMPLE". Since thist is the first
  * element, this will be the root element of the document. */
  rc = xmlTextWriterStartElement ( writer, BAD_CAST "protocol" );



  /* Add an attribute with name "xml:lang" and value "de" to ORDER. */
  rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST "ProtocolCode",
                                     BAD_CAST "0x01000000" );



  /* Add an attribute with name "xml:lang" and value "de" to ORDER. */
  rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST "ProtocolVer",
                                     BAD_CAST "1.0.0.0" );



  //<protocol ProtocolCode="0x01000000" ProtocolVer="1.0.0.0">
  //	<table id="1" cmd="yc">
  //		<register type="R">

  /* Start an element named "ORDER" as child of EXAMPLE. */

  //3
  rc = xmlTextWriterStartElement ( writer, BAD_CAST "register" );


  /* Add an attribute with name "xml:lang" and value "de" to ORDER. */
  rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST "type",
                                     BAD_CAST "R" );

#endif


  FILE *owner_fp=NULL;



  unsigned int i = 0;
  int32_t line_num = 0;
  unsigned int line_len = 0;
  char element[128] = {0};


  owner_fp=fopen ( p_protocol_name,"r" );
  
  if ( NULL==owner_fp )
    {
      printf ( " transform_import_csv open  :%s  not exist  \n" , p_protocol_name );
      return 0;
    }

  fseek ( owner_fp, 0, SEEK_END );
  
  int size = ftell ( owner_fp );

  // printf ( " PATH--%s size = %d\n", PATH,size );
  char *buf = ( char * ) malloc ( ( size+10 ) );


  fseek ( owner_fp, 0, SEEK_SET );
  fread ( buf,size,1,owner_fp );
  
  fclose ( owner_fp );


  int head_num = 0;


  memset ( data_head_name,0,100*100 );
  
  
  char temp_elem[1024] = { 0 };

  if ( size )
    {
        
      
      for ( i = 0; i < size; ++i )
        {
          if ( ( buf[i] == ',' ) || ( buf[i] == '\n' ) )
            {
              head_num ++;

              if ( buf[i] == '\n' )
                {
                  break;
                }
            }
        }
        
        printf("head_num %d \n",head_num);
        
        
      
      for ( i = 0; i < size; ++i )
        {


          if ( buf[i] == '\n' )
            {
              buf[i] = '\0';
              line_num++;
            }
        }

        
        
      char tmp_element[128] =  { 0 };

      int32_t line_id = 0;
      int32_t list_id = 0;

      for ( line_id = 0; line_id < 2 ; line_id++ )
        {
//         printf ( " headLine %d:  \n ", line_id );
          for ( list_id = 0; list_id < head_num ; list_id ++ )
            {
              memset ( tmp_element,0,sizeof ( tmp_element ) );

              line_len +=  get_element_from_str ( &buf[line_len],tmp_element,',' );
              printf ( " tmp_element %s \n ",tmp_element );
              if ( line_id ==1 )
                {
		  
                  memset ( data_head_name[list_id],0,100 );

                  memcpy ( data_head_name[ list_id ],tmp_element,strlen ( tmp_element ) );
		  
		 // printf("data_head_name[ %d ] %s \n",list_id,data_head_name[ list_id ]);
		  
                }
            }
        }


      int line_m = 0;
      int row_n = 0;
      
      for ( line_m =0 ; line_m < line_num-2 ; line_m ++ )
        {

              rc = xmlTextWriterStartElement ( writer, BAD_CAST "point" );


          for ( row_n = 0; row_n < head_num; row_n++ )
            {

              line_len += get_element_from_str ( &buf[line_len],element,',' );

             printf ( " data_head_name[ %d ] %s   BAD_CAST  element %s   \n",row_n,data_head_name[ row_n ],   element );

               rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST data_head_name[ row_n ],  BAD_CAST  element );

            }
            
              rc = xmlTextWriterEndElement ( writer );

        }

    }

  free ( buf );



  /* Close the element named HEADER. */
  // 停止写入HEADER结点
  rc = xmlTextWriterEndElement ( writer );


  /* Close the element named FOOTER. */
  rc = xmlTextWriterEndElement ( writer );


  /* Close the element named FOOTER. */
  rc = xmlTextWriterEndElement ( writer );

//xmlTextWriterFullEndElement(writer);

  /* Here we could close the elements ORDER and EXAMPLE using the
   * function xmlTextWriterEndElement, but since we do not want to
   * write any other elements, we simply call xmlTextWriterEndDocument,
   * which will do all the work. */
 rc = xmlTextWriterEndDocument ( writer );
  

  
  xmlFreeTextWriter ( writer );
  


     /*
     * Cleanup function for the XML library 
     */  
   xmlCleanupParser();  
    /* 
     * this is to debug memory for regression tests 
     */  
//    xmlMemoryDump();  


  return 0;
}




void search_all_file ( char *path )
{
  
  
  DIR              *pDir ;
  
  struct dirent    *ent ;

  char              childpath[512];

  pDir=opendir ( path );
  
  memset ( childpath  ,  0 ,  sizeof ( childpath )   );


  while ( ( ent=readdir ( pDir ) ) !=NULL )
    {

      if ( ent->d_type & DT_DIR )
        {

          if ( strcmp ( ent->d_name,"." ) ==0 || strcmp ( ent->d_name,".." ) ==0 )
            continue;

          sprintf ( childpath,"%s/%s",path,ent->d_name );
          printf ( "path:%s/n",childpath );

          search_all_file ( childpath );

        }
      else
        {
          printf ( "ent->d_name %s\n",ent->d_name );

          transform_import_csv ( ent->d_name );


        }
    }

}





int main ( int argc, char **argv )
{

 
    LIBXML_TEST_VERSION  
  
//   transform_import_csv ( argv[1] );
  
  search_all_file (argv[1]  );
  
    /* 
     * Cleanup function for the XML library 
     */  
    xmlCleanupParser();  
    /* 
     * this is to debug memory for regression tests 
     */  
    xmlMemoryDump(); 
  
//     xmlCleanupParser();  
    /* 
     * this is to debug memory for regression tests 
     */  
  //  xmlMemoryDump();  
              printf ( "444444444444   \n" );
  
 
  return ( 0 );
}