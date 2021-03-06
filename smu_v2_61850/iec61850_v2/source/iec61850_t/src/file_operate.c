/******************************************************************************
* Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
* File name :file_operate.c
* Author :shiran
* Date :2015-12-08
* Description :
* Others :
*******************************************************************************
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
// 者及修改内容简述
*------------------------------------------------------------------------------
* //2015-12-08: 版本：V 0.1 作者: shiran
* Modification :
*------------------------------------------------------------------------------
******************************************************************************/





/************************************************************************
*                   头文件
*************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "io.h"
#include "debugSwitch.h"
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
                    DEBUG_FAULT ( "mkdir error: %s\n",DirPath );
                    ret_mk = -1;
                    break;

                }
            }
            DirPath[i]='/';
        }
    }

    return ret_mk;
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




/************************************************************************
*                   局部函数
*************************************************************************/
