#ifndef __PIPE_DATA_TYPE_H__
#define __PIPE_DATA_TYPE_H__

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short int uint16_t;
typedef short int          int16_t;
typedef unsigned int       uint32_t;
typedef int                int32_t;
typedef float              float32_t;

typedef enum {FALSE = 0, TRUE = !FALSE} bool;

/**/
typedef union
{
    float32_t    value_f;     
    uint32_t     value_32;  
    bool         value_bool; 
}FifoValue; /* 4 bytes */
    
typedef enum
{
    TYPE_FLOAT32,     
    TYPE_UINT32,       
    TYPE_BOOL,      
    TYPE_NUM,
}FifoValueType;

typedef struct{
    time_t  opTime;
    uint8_t opType;//on:1; off:0.
    uint8_t gotReturn;//yes:1; not yet:0.
    uint8_t returnVal;//ok:1; fail:0.
    uint8_t needCheckReturn;//yes:1, no:0.
}YK_RETURN_STATE_READ_T;

    /* ?*/
typedef struct
{
    char     dataType[4];    /* yc/yx/yk/yt */
    uint32_t  devType;       /*devType*/
    uint32_t  devNo;        /*dev Addr*/
    uint32_t  dataNum;      /*data members*/
    uint32_t  bodySize;      /* total data bytes */
    uint32_t  resv;          /* reserved */
}FifoHeader;


typedef struct
{
    uint16_t    dataID;   
    uint16_t    val;      
}YX_FIFO_DATA_T;

typedef struct
{
    uint16_t        dataID;   /* modbusAddr */      
    FifoValueType   val_type:16;  /* uint32 or float */
    FifoValue       val;      /* value */
}YC_FIFO_DATA_T;

typedef struct
{
    uint16_t    dataID;   
    uint16_t    val;      
}YK_FIFO_DATA_T;

typedef struct
{
    uint16_t        dataID;   /* modbusAddr */      
    FifoValueType   val_type:16;  /* uint32 or float */
    FifoValue       val;      /* value */
}YT_FIFO_DATA_T;


#define YC_REGISTER_QUANTITY 150

typedef struct YC_PACKET_SEND_BY_PIPE_T
{
    FifoHeader      header;
    YC_FIFO_DATA_T  data[YC_REGISTER_QUANTITY];//5000-5072
}YC_PACKET_SEND_BY_PIPE_T;

#define YX_REGISTER_QUANTITY 200

typedef struct YX_PACKET_SEND_BY_PIPE_T
{
    FifoHeader      header;
    YX_FIFO_DATA_T  data[YX_REGISTER_QUANTITY];
}YX_PACKET_SEND_BY_PIPE_T;


#define YK_REGISTER_QUANTITY 5

typedef struct YK_PACKET_SEND_BY_PIPE_T
{
    FifoHeader      header;
    YK_FIFO_DATA_T  data[YK_REGISTER_QUANTITY];//on(0xCF) off(0xCE) @ 5006
}YK_PACKET_SEND_BY_PIPE_T;


#define YT_REGISTER_QUANTITY 30

typedef struct YT_PACKET_SEND_BY_PIPE_T
{
    FifoHeader      header;
    YT_FIFO_DATA_T  data[YT_REGISTER_QUANTITY];//5008 5019 5036 5037
}YT_PACKET_SEND_BY_PIPE_T;



//-------my typedefines-------:
#define DEV_TYPE 2827
#define MAX_DEV_ADDR 3                 /**/
#define DELAY_IN_DEBUG_MODE         0   //delay actual MS or delay 1 second.
#define ALLOW_TO_DISPLAY_DEBUG_INFO 1   //open debug info print or close it.

#define SERIAL_PORT_READ_BUF_LENGTH 256 
#define PIPE_READ_BUF_LENGTH        512 
#define MODBUS_LOST_PKTS_THRESHOLD  20   //

#define YK_RETURN_STATE_READ_TIME_OUT 6 //seconds.


#define ARRAY_ID(devNo)  (((devNo)-1)%(MAX_DEV_ADDR))

#if DELAY_IN_DEBUG_MODE
    #define DELAY_MICRO_SECOND(DoNotCare)    sleep(1)
#else
    #define DELAY_MICRO_SECOND(microsecond)  usleep(1000*(microsecond))
#endif

#define CLEAR_LOST_PKTS_COUNTER(devNo)      \
do{                                         \
    g_modbusLostPktsCounter[ARRAY_ID(devNo)] = 0;   \
}while(0)
    
#define UPDATE_LOST_PKTS_COUNTER(devNo)                                 \
do{                                                                     \
    if(g_modbusLostPktsCounter[ARRAY_ID(devNo)] < MODBUS_LOST_PKTS_THRESHOLD)   \
    {                                                                   \
        g_modbusLostPktsCounter[ARRAY_ID(devNo)]++;            \
    }                                                                   \
}while(0)

#define GET_MODBUS_REG_VALUE(buf,baseIndex)\
        (((uint16_t)buf[(baseIndex)] << 8) + buf[((baseIndex)+1)])

#define WRITE_TO_LOG_WHILE_ERROR(i,buf,length)   \
do{                                               \
    for(i=0; i<(length); i++)                     \
    {                                             \
        fprintf(stderr, "%02x ",buf[i]);          \
    }                                             \
    fprintf(stderr, "\n");                        \
}while(0);

typedef enum
{
    DISENABLE = 0x55,
    PF_ENABLE = 0xA1,//power factor
    WR_ENABLE = 0xA2,//wattless ratio
    QP_ENABLE = 0xA3,
    QU_ENABLE = 0xA4
}SET_VALUE_ENABLE_OPTION_E;

typedef enum
{
    DEV_IS_LOST = 1,
    DEV_IS_ALIVE = 2,
}DEV_LOST_E;

typedef struct
{
    uint16_t lowerPower;    //5051
    uint16_t upperPower;    //5052
    uint16_t upperPf;       //5053
    uint16_t lowerPf;       //5054
}YT_QP_INFO_T;

typedef struct
{
    uint16_t lowerU;        //5081
    uint16_t upperU;        //5082
    uint16_t U1Limit;       //5083
    uint16_t U2Limit;       //5084
    uint16_t hysteresis;    //5085 ?
    uint16_t lowerQ;        //5086
    uint16_t upperQ;        //5087
    
    uint16_t reserved;
}YT_QU_INFO_T;

typedef struct
{
    YT_QP_INFO_T ytQpInfo;
    YT_QU_INFO_T ytQuInfo;
}YT_QP_QU_INFO_T;

typedef struct
{
    int32_t fileHandle;     // ?
    char fileName[30];      // ??
    char backUpFileName[34];// ??
    struct stat attr;       // ??
}LOG_INFO_T;

#endif /* of __PIPE_DATA_TYPE_H__ */
