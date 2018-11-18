
#ifndef IO_H_
#define IO_H_

#include <unistd.h>
#include <fcntl.h>

typedef  unsigned char uint8_t;
typedef  signed char int8_t;
typedef  unsigned short int uint16_t;
typedef  signed short int int16_t;
typedef  unsigned int uint32_t;
typedef  signed int int32_t;
typedef  unsigned long long int uint64_t;
typedef float float32_t;

#define SERVER_FIFO_NAME "/tmp/fifo_server"
#define DATA_INPUT_FIFO_NAME "/tmp/data_input_fifo"   //数据处理进程的数据输入管道
#define DISPLAY_FIFO_NAME "/tmp/display_fifo"         //数据处理进程的诊断结果输出管道
#define MODBUS_M_W_FIFO "/tmp/modbus_m_fifo"
#define MODBUS_S_W_FIFO "/tmp/modbus_s_fifo"
#define IEC61850_W_FIFO "/tmp/iec61850_fifo"
#define IEC104_W_FIFO "/tmp/iec104_fifo"




#define UNIX_DOMAIN "/tmp/UNIX.domain" 

#define FLOAT_T 0  //浮点
#define U_INT_T 1  //无符号数
#define S_INT_T 2  //有符号数

//socket命令
typedef enum
{
  MSG_LOGIN = 1,
  MSG_REBOOT,
  MSG_FAULTWAVE,
  MSG_DEVCOMMERR,
  MSG_COMMSIGN,
  MSG_PTIMEOUT,
  MSG_PCOMPLETE,
  MSG_NETMODF,
  MSG_MODBUSID,
  MSG_COMBAUND,
  MSG_TIMER,
  MSG_FAULTWAVERET
}msg_cmd_s;


typedef union {
  uint32_t u32;
  int32_t  s32;
  float f32;
}data_u;

//管道通信数据结构
typedef struct {
  uint16_t dev_code; //设备类型编码
  uint16_t index;    //设备索引
  uint16_t data_id;       //测点编码
  struct
  {
    uint8_t data_type:4;//数据类型
    uint8_t wr_type:4;//读写类型 0-设置，1-回读
  };
  uint8_t clientID;  //测点上传客户端id
  data_u data;       //数据值
}dev_data_t;

//套接字通信数据结构
typedef struct {
  uint32_t cmd;   //命令
  uint32_t para;   //参数
  uint32_t len;   //消息长度
  uint8_t data[4096];  //消息内容
  uint16_t crc;   //crc校验
}sg_msg_s;



#endif/* IO_H_ */
