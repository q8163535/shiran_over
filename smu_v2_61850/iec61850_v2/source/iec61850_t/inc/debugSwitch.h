/*************************************************
FileName:     debugSwitch.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         debugSwitch.c
History:
**************************************************/
#ifndef DEBUGSWITCH_H
#define DEBUGSWITCH_H
#include <stdio.h>
#include "debug_config.h"



#define DEBUGOPEN 0   //调试开关

#define PRINTOPEN   0

#define XMLPRINTOPEN  0

#define DEBUG_FIFO_FILL    0

#define DEBUG_GLOBAL   0

#define FILE_ABSOLUTE_PATH 0

#define BUF_PRINT   0   //BUFFER打印开关
#define DEBUG_CALLBACK_PRINT   0   //回调函数打印开关

#define DEBUG_RECORD_PRINT   0   //调试日志打印开关

#define DEBUG_BACKGROUND_PRINT   0

//#define DEBUG(format,...)  printf("FILE: "__FILE__", LINE: %d: "format"/n", __LINE__, ##__VA_ARGS__)


#define DEBUG_ON_OFF
 
#if defined(DEBUG_ON_OFF)
  #define DEBUG(fmt,arg...) printf(fmt,##arg)
#else
  #define DEBUG(...)
#endif

#define PRINT_CONFIG_MARK (0x1 << 0)                       //0x01  debug 故障信息打印开关
#define PRINT_COM_RECV_MARK (0x1 << 1)              //0x02 debug  串口接收后台数据开关
#define PRINT_COM_SEND_MARK (0x1 << 2)             //0x04 debug  串口发送后台数据开关
#define PRINT_NET_RECV_MARK (0x1 << 3)              //0x08 debug 网络接受数据开关
#define PRINT_NET_SEND_MARK (0x1 << 4)             //0x010 debug 打印网络接收开关
#define PRINT_FIFO_RECV_MARK (0x1 << 5)            //0x020 debug 管道接受数据开关
#define PRINT_FIFO_SEND_MARK (0x1 << 6)           //0x040debug 管道发送数据开关
#define PRINT_CALLBACK_MARK (0x1 << 7)               //0x080  debug 回调信息打印开关
#define PRINT_MESSAGE_PRINT_MARK (0x1 << 8)     //0x0100  debug 本地信息打印开关
#define PRINT_CSV_PRINT_MARK (0x1 << 9)     //0x0200  debug 本地信息打印开关

//故障信息打印 带时间信息
#define DEBUG_FAULT(format, args...)  print_time_lable_show("fault_time:");\
printf("##%s:%s:%d##:"format,__FILE__,__func__,__LINE__,##args);
 
//初始化信息打印 带时间信息
#define DEBUG_INIT(format,arg...)  printf(format,##arg);
 
 
 //初始化信息打印 
#define DEBUG_TIME_INIT(format, args...)  print_time_lable_show("INIT_time:");\
printf(format,##args);
 
 
 
 
 
 #define DEBUG_CONFIG(format, args...) if((debug_module.debug_set&PRINT_CONFIG_MARK) == PRINT_CONFIG_MARK)\
{\
    printf(format,##args);\
}\
else\
{\
}



#define DEBUG_CONFIG_TIME(format, args...) if((debug_module.debug_set&PRINT_CONFIG_MARK) == PRINT_CONFIG_MARK)\
{\
  print_time_lable_show("CONFIG:");\
  printf("##%s:%s:%d##:"format,__FILE__,__func__,__LINE__,##args);\
}\
else\
{\
}


#define DEBUG_COM_RECV_INFO(format, args...) if((debug_module.debug_set&PRINT_COM_RECV_MARK) == PRINT_COM_RECV_MARK)\
{\
  printf(format,##args);\
}\
else\
{\
}

#define DEBUG_COM_RECV_TIME_INFO(format, args...) if((debug_module.debug_set&PRINT_COM_RECV_MARK) == PRINT_COM_RECV_MARK)\
{\
  print_time_lable_show("COM_RECV:");\
  printf(format,##args);\
}\
else\
{\
}



#define DEBUG_COM_SEND_INFO(format, args...) if((debug_module.debug_set&PRINT_COM_SEND_MARK) == PRINT_COM_SEND_MARK)\
{\
  printf(format,##args);\
}\
else\
{\
}

#define DEBUG_NET_RECV_INFO(format, args...) if((debug_module.debug_set&PRINT_NET_RECV_MARK) == PRINT_NET_RECV_MARK)\
{\
  printf(format,##args);\
}\
else\
{\
}


#define DEBUG_NET_RECV_TIME_INFO(format, args...) if((debug_module.debug_set&PRINT_NET_RECV_MARK) == PRINT_NET_RECV_MARK)\
{\
  print_time_lable_show("NET_RECV:");\
  printf(format,##args);\
}\
else\
{\
}

#define DEBUG_NET_SEND_INFO(format, args...) if((debug_module.debug_set&PRINT_NET_SEND_MARK) == PRINT_NET_SEND_MARK)\
{\
  printf(format,##args);\
}\
else\
{\
}

#define DEBUG_NET_SEND_TIME_INFO(format, args...) if((debug_module.debug_set&PRINT_NET_SEND_MARK) == PRINT_NET_SEND_MARK)\
{\
  print_time_lable_show("NET_SEND:");\
  printf(format,##args);\
}\
else\
{\
}




#define DEBUG_FIFO_RECV_INFO(format, args...) if((debug_module.debug_set&PRINT_FIFO_RECV_MARK) == PRINT_FIFO_RECV_MARK)\
{\
  printf(format,##args);\
}\
else\
{\
}


#define DEBUG_FIFO_RECV_TIME_INFO(format, args...) if((debug_module.debug_set&PRINT_FIFO_RECV_MARK) == PRINT_FIFO_RECV_MARK)\
{\
  print_time_lable_show("NET_RECV:");\
  printf(format,##args);\
}\
else\
{\
}


#define DEBUG_FIFO_SEND_INFO(format, args...) if((debug_module.debug_set&PRINT_FIFO_SEND_MARK) == PRINT_FIFO_SEND_MARK)\
{\
  printf(format,##args);\
}\
else\
{\
}

#define DEBUG_CALLBACK_INFO(format, args...) if((debug_module.debug_set&PRINT_CALLBACK_MARK) == PRINT_CALLBACK_MARK)\
{\
  printf(format,##args);\
}\
else\
{\
}


#define DEBUG_MESSAGE_PRINT_INFO(format, args...) if((debug_module.debug_set&PRINT_MESSAGE_PRINT_MARK) == PRINT_MESSAGE_PRINT_MARK)\
{\
  printf(format,##args);\
}\
else\
{\
}


#define DEBUG_CSV_PRINT(format, args...) if((debug_module.debug_set&PRINT_CSV_PRINT_MARK) == PRINT_CSV_PRINT_MARK)\
{\
    printf(format,##args);\
}\
else\
{\
}



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

void terminal_print_time_lable ( void );
void print_time_lable_show ( char *p_rec_str );
void fun_print_time(char * input_str);
void print_buf ( int32_t arr_len,char *in_buf,int32_t in_fd, int32_t int_out_direction );


#endif
