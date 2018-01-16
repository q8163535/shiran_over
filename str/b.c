#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#define DEBUG(format,...)  printf("FILE: "__FILE__", LINE: %d: "format"\n", __LINE__, ##__VA_ARGS__)

  //套接字通信数据结构
typedef struct {
  unsigned int  cmd;   //命令
  unsigned int para;   //参数
  unsigned int len;   //消息长度
  unsigned char data[1024];  //消息内容
  unsigned int crc;   //crc校验
}sg_msg_s;
void func_tion(sg_msg_s *p_msg)
{
p_msg[0].cmd = 1000;
p_msg[2].cmd =2000;
}
int main()
{
  
 sg_msg_s   tmp_msg[10];
 
 func_tion( &tmp_msg );
 
 printf("tmp_msg 0 cmd: %d tmp_msg  2  cmd: %d \n ",tmp_msg[0].cmd,  tmp_msg[2].cmd );
 
}