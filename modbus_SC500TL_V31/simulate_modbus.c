#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

#include <termios.h>
#include <time.h>
#include "feeddog.h"
#include "simulate_modbus.h"
#include <errno.h>



//#define soft_ver  "1.0.0.0_2018-11-03" //  更新调试打印信息 管道发送数据内容

#define soft_ver  "1.0.0.1_2018-11-19" //  更新打开串口为 ttyO1

void print_time_lable_show ( char *p_rec_str )
{
    struct tm ptm = { 0 };
    time_t tNow =time ( NULL );
    localtime_r ( &tNow, &ptm );
    printf ( "time: %d.%d.%d-%d:%d:%d  --: %s \n",ptm.tm_year+1900,ptm.tm_mon+1,ptm.tm_mday,ptm.tm_hour,ptm.tm_min,ptm.tm_sec,p_rec_str );

}

#define DEBUG_TIME_OUTPUT(format, args...)  print_time_lable_show("TIME_OUTPUT:");\
printf(format,##args);



#define DEBUG_OPEN  1






static uint8_t g_serialPortReadBuf[SERIAL_PORT_READ_BUF_LENGTH] = {0};
static uint8_t g_pipeReadBuf[PIPE_READ_BUF_LENGTH] = {0};

static YX_PACKET_SEND_BY_PIPE_T g_yxPacketSendByPipe ;
static YC_PACKET_SEND_BY_PIPE_T g_ycPacketSendByPipe ;

static YK_PACKET_SEND_BY_PIPE_T g_ykPacketSendByPipe ;

static YT_PACKET_SEND_BY_PIPE_T g_ytPacketSendByPipe ;


static uint8_t g_modbusLostPktsCounter[MAX_DEV_ADDR] = {0};
static uint8_t g_isDevLost[MAX_DEV_ADDR] = {0};

static LOG_INFO_T g_logInfo = { 0 , "", "", { 0 } };

//----------------------------------------------------------------------------------------------------------------------------
static void init_modbus_lost_pkts_counter ( void )
{
    int32_t i = 0;
    for ( i=0; i<MAX_DEV_ADDR; i++ )
    {
        g_modbusLostPktsCounter[i] = MODBUS_LOST_PKTS_THRESHOLD;
    }
}

/** STATIC/LOCAL FUNCTIONS: ***************************************************/
//----------------------------------------------------------------------------------------------------------------------------
#if ALLOW_TO_DISPLAY_DEBUG_INFO
static void SHOW_DEBUG_INFO ( const char *fmt, ... )
{
    va_list args = {0};

    va_start ( args, fmt );
    fprintf ( stderr, "DEBUG: " );
    vfprintf ( stderr, fmt, args );
    va_end ( args );
    return;
}
#else
#define SHOW_DEBUG_INFO
#endif
//----------------------------------------------------------------------------------------------------------------------------
static void WRITE_TO_LOG ( const char *fmt, ... )
{
    va_list args = {0};

    va_start ( args, fmt );

    vfprintf ( stderr, fmt, args );
    va_end ( args );
    return;
}
//----------------------------------------------------------------------------------------------------------------------------
static void log_init ( void )
{
    const char* fileName = "syslog/sg61850Modbus.log";

    strcpy ( g_logInfo.fileName, fileName );
    sprintf ( g_logInfo.backUpFileName, "%s.bak", g_logInfo.fileName );


    g_logInfo.fileHandle = open ( fileName, O_CREAT | O_RDWR | O_APPEND, 0666 );
    if ( g_logInfo.fileHandle <= 0 )
    {
        perror ( "ERROR @ open 'sg61850Modbus.log'!\n" );
        exit ( EXIT_FAILURE );
    }


    if ( dup2 ( g_logInfo.fileHandle, STDERR_FILENO ) < 0 )
    {
        perror ( "ERROR @ dup2 STDERR to 'sg61850Modbus.log'!\n" );
        exit ( EXIT_FAILURE );
    }
}

//----------------------------------------------------------------------------------------------------------------------------
static void backup_log_if_need ( void )
{
#define LOG_MAX_SIZE (16*1024)

    do
    {
        if ( g_logInfo.fileHandle <= 0 )
        {
            break;
        }

        g_logInfo.attr.st_size = 0;
        fstat ( g_logInfo.fileHandle, &g_logInfo.attr );
        if ( g_logInfo.attr.st_size < LOG_MAX_SIZE )
        {
            break;
        }

        int ret = close ( g_logInfo.fileHandle );
        if ( 0 != ret )
        {
            break;
        }

        //----------------------------------------------------------------------------------------------------------------------------
        ret = rename ( g_logInfo.fileName, g_logInfo.backUpFileName );
        /*  */
        if ( 0 != ret )
        {
            break;
        }

        g_logInfo.fileHandle = open ( g_logInfo.fileName, O_CREAT | O_RDWR | O_APPEND, 0666 );
        if ( g_logInfo.fileHandle <= 0 )
        {
            perror ( "ERROR @ open 'sg61850Modbus.log'!\n" );
            exit ( EXIT_FAILURE );
        }


        if ( dup2 ( g_logInfo.fileHandle, STDERR_FILENO ) < 0 )
        {
            perror ( "ERROR @ dup2 STDERR to 'sg61850Modbus.log'!\n" );
            exit ( EXIT_FAILURE );
        }
    }
    while ( 0 );
}


//----------------------------------------------------------------------------------------------------------------------------

#define SEND_CMD_TO_MODBUS(handle, buf, buflen) \
do{                                             \
    write(handle, buf, (buflen));               \
    sw_watchdog_feed(SIGUSER1);                 \
}while(0)

static int32_t read_data_from_modbus ( int32_t handle, uint8_t* buf, uint32_t buflen )
{
    int32_t ret = 0;

    memset ( buf,0x00,buflen );
    ret = read ( handle, buf, buflen );
#if DEBUG_OPEN
    printf ( "\n RECV_CMD_TO_MODBUS  :  read_len_num: %d   ret %d \n",buflen ,ret );

    int32_t data_id  = 0;
    for ( data_id = 0 ; data_id < ret; data_id ++ )
    {
        if ( ( data_id %10 )  == 0 )
        {
            printf ( "\n" );
        }
        printf ( "0x%02x ",buf[data_id ] );
    }
    printf ( "\n" );

#endif

    return ret;
}

//----------------------------------------------------------------------------------------------------------------------------
static char* get_current_time_str ( void )
{
    time_t rawTime = time ( NULL );
    struct tm* tmPtr = localtime ( &rawTime );
    static char timeInfo[24] = {0};

    sprintf ( timeInfo, "%04d-%02d-%02d %02d:%02d:%02d",\
              tmPtr->tm_year+1900, tmPtr->tm_mon+1, tmPtr->tm_mday,\
              tmPtr->tm_hour,      tmPtr->tm_min,   tmPtr->tm_sec );

    return timeInfo;
}

//----------------------------------------------------------------------------------------------------------------------------
static int32_t check_and_get_header_info ( FifoHeader* whereToSave, const FifoHeader* headerPointer )
{
    int32_t ret = 0;

    if ( ( 0 == strcmp ( headerPointer->dataType, "yt" ) ) ||
            ( 0 == strcmp ( headerPointer->dataType, "ds" ) ) ||
            ( 0 == strcmp ( headerPointer->dataType, "yk" ) ) )
    {
        memcpy ( whereToSave, headerPointer, sizeof ( FifoHeader ) );
    }
    else
    {
        WRITE_TO_LOG ( "ERROR: headerPointer.dataType = %s @ %s.\n",
                       headerPointer->dataType, get_current_time_str() );
        ret = -1;
    }

    return ret;
}
//----------------------------------------------------------------------------------------------------------------------------
static uint16_t get_crc16 ( volatile uint8_t *ptr, uint16_t len )
{
    uint8_t i;
    uint16_t crc=0xFFFF;
    if ( len==0 )
    {
        len=1;
    }
    while ( len-- )
    {
        crc ^= *ptr;
        for ( i=0; i<8; i++ )
        {
            if ( crc&1 )
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        ptr++;
    }
    return crc;
}

//----------------------------------------------------------------------------------------------------------------------------
static int32_t open_and_set_a_serial_port ( const char* portName )
{
    struct termios uart_attr;
    int serialPortHandle = open ( portName, O_RDWR | O_NDELAY );
    if ( serialPortHandle < 0 )
    {
        perror ( "open serial port fail!\n" );
        exit ( EXIT_FAILURE );
    }

    // configure the uart
    tcgetattr ( serialPortHandle, &uart_attr );
    cfmakeraw ( &uart_attr );
    cfsetispeed ( &uart_attr, B9600 );
    cfsetospeed ( &uart_attr, B9600 );
    uart_attr.c_cflag &= ~PARENB;
    uart_attr.c_cflag &= ~CSIZE;
    uart_attr.c_cflag |= CS8;      // 8-bit data
    uart_attr.c_cflag &= ~CRTSCTS;
    uart_attr.c_lflag &= ~ ( ICANON | ECHO | ECHOE | ISIG );
    tcsetattr ( serialPortHandle, TCSANOW, &uart_attr ); // TCSANOW make the setting valid immediately

    return serialPortHandle;
}


//----------------------------------------------------------------------------------------------------------------------------
static int32_t get_a_to_modbus_pipe_at_read_mode ( const char* pipeName )
{
    if ( 0 == access ( pipeName, F_OK ) )
    {
    }
    else
    {
        mkfifo ( pipeName, 0777 );
    }

    int ykYtPipeHander = 0;

    while ( 1 )
    {

        ykYtPipeHander = open ( pipeName, O_RDONLY | O_NONBLOCK );

        if ( ykYtPipeHander <= 0 )
        {
            WRITE_TO_LOG ( "ERROR: Failed to open pipe<%s> @ read mode @ ret = %d @ %s\n",
                           pipeName, ykYtPipeHander, get_current_time_str() );
            DELAY_MICRO_SECOND ( 1000 );
        }
        else
        {
            //actually, pipe can be open @ read mode @ any time.
            SHOW_DEBUG_INFO ( "Open pipe<%s> @ read mode successfully! @ %s\n",
                              pipeName, get_current_time_str() );
            break;

        }
    }

    return ykYtPipeHander;
}


//----------------------------------------------------------------------------------------------------------------------------
static int32_t get_a_to61850_pipe_at_write_mode ( const char* pipeName )
{
    int pipeHander;

    if ( 0 == access ( pipeName, F_OK ) )
    {
    }
    else
    {
        mkfifo ( pipeName, 0777 );
    }

    while ( 1 )
    {
        pipeHander = open ( pipeName, O_WRONLY | O_NONBLOCK | O_APPEND );

        if ( pipeHander <= 0 )
        {
            WRITE_TO_LOG ( "ERROR: Failed to open pipe<%s> @ write mode @ ret = %d @ %s\n",
                           pipeName, pipeHander, get_current_time_str() );
            DELAY_MICRO_SECOND ( 1000 );
        }
        else
        {
            SHOW_DEBUG_INFO ( "Open pipe<%s> @ write mode successfully! @ %s\n",
                              pipeName, get_current_time_str() );
            break;
        }
    }
    return pipeHander;
}

//----------------------------------------------------------------------------------------------------------------------------
static int32_t send_cmd_and_wait_for_return_data ( int32_t  serialPortHandle,
        uint8_t* sendBuf,
        uint32_t bufLen,
        uint32_t delayMs )
{
    SEND_CMD_TO_MODBUS ( serialPortHandle, sendBuf, bufLen );

#if DEBUG_OPEN
    printf ( "\n  SEND_CMD_TO_MODBUS  :     buflen %d\n",bufLen );

    int32_t data_id  = 0;
    for ( data_id = 0 ; data_id < bufLen; data_id ++ )
    {
        printf ( "0x%0x ",sendBuf[data_id ] );
    }
    printf ( "\n" );

#endif


    DELAY_MICRO_SECOND ( delayMs ); // >= 2 * (8 + 2 * sendBuf[5] + 5) @ 9600bps.

    int32_t returnBytes = read_data_from_modbus ( serialPortHandle,
                          g_serialPortReadBuf,
                          SERIAL_PORT_READ_BUF_LENGTH );
    return returnBytes;
}

//----------------------------------------------------------------------------------------------------------------------------
static int32_t set_a_value_to_modbus ( int32_t serialPortHandle, uint16_t regaddr, int16_t regValue, int16_t devNo )
{
    uint16_t modbusAddr = regaddr-1;

#define  MODBUS_CMD_LEN 8
    uint8_t  modBusCmd[MODBUS_CMD_LEN] =
    {
        devNo,0x06, // addr & cmd
        modbusAddr >> 8,        // modbus addr
        modbusAddr & 0xFF,      // modbus addr
        regValue >> 8, regValue & 0xFF,   // value to set.
        0,0
    };   //crc

    uint16_t crc = get_crc16 ( modBusCmd, 6 );
    modBusCmd[6] = crc & 0xFF;
    modBusCmd[7] = ( crc >> 8 ) & 0xFF;

    int32_t sendTimes = 0;
    int i = 0;

    SHOW_DEBUG_INFO ( "set_a_value_to_modbus  set reg%d @ value:%d to modbus @ dev(%d).\n", regaddr, regValue, devNo );

    for ( sendTimes=0;  sendTimes<3; sendTimes++ )
    {
        send_cmd_and_wait_for_return_data ( serialPortHandle, modBusCmd, MODBUS_CMD_LEN, 150 );
        UPDATE_LOST_PKTS_COUNTER ( devNo );

        if ( 0 == memcmp ( modBusCmd, g_serialPortReadBuf, MODBUS_CMD_LEN ) )
        {
            CLEAR_LOST_PKTS_COUNTER ( devNo );
            break;
        }
        else
        {
            WRITE_TO_LOG ( "ERROR: Failed to set reg%d @ dev(%d) @ time %d:\n",regaddr,devNo, sendTimes+1 );

            WRITE_TO_LOG ( "Set:    " );
            WRITE_TO_LOG_WHILE_ERROR ( i,modBusCmd,MODBUS_CMD_LEN );

            WRITE_TO_LOG ( "Return: " );
            WRITE_TO_LOG_WHILE_ERROR ( i, g_serialPortReadBuf, MODBUS_CMD_LEN );
        }
    }

    return 0;
}

#define MAX_LINE  128
static uint16_t lastReg[ MAX_LINE ][ MAX_DEV_ADDR ] ;//= { 0 };


static int32_t yt_data_is_changed ( uint8_t devNo, uint16_t reg_rev,int16_t reg_id )
{
    int32_t ret = 0;

    if ( lastReg[reg_id][ARRAY_ID ( devNo )] != reg_rev )
    {
        ret = 1;
        lastReg[reg_id][ARRAY_ID ( devNo )] = reg_rev;
    }
    else
    {
        ret = 0;
    }

    return ret;
}




/*61850¹*/
static int32_t parse_yc_data_and_send_to_pipe ( int32_t to61850PipeHandle,
        const uint8_t * modbusBuf,
        uint32_t devNo,
        uint8_t segNum )
{
    YC_FIFO_DATA_T * dataPointer = g_ycPacketSendByPipe.data;
    int dataIndex = 0;
    int totalSize = 0;
    int ret = 0;

    FifoHeader*     headPointer = &g_ycPacketSendByPipe.header;
    headPointer->dataType[0] = 'y';
    headPointer->dataType[1] = 'c';
    headPointer->dataType[2] = '\0';
    headPointer->devType = DEV_TYPE;
    headPointer->devNo = devNo;
    headPointer->resv = 0;

    // 3310~3362

    /*3310 devType is in header info.*/
    dataIndex = 0;
    dataPointer[dataIndex].dataID       = 3310;  //直流电压
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[0] << 8 ) + modbusBuf[1] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;

//3310-3310*2

    dataPointer[dataIndex].dataID       = 3311;  //直流电流
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[2] << 8 ) + modbusBuf[3] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;



    dataPointer[dataIndex].dataID       = 3312;  //直流功率
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[4] << 8 ) + modbusBuf[5] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;



    dataPointer[dataIndex].dataID       = 3313;  //电网电压VAB

    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[6] << 8 ) + modbusBuf[7] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3314;  //电网电压VBC
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[8] << 8 ) + modbusBuf[9] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3315;  //电网电压VCA
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[10] << 8 ) + modbusBuf[11] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3316;  //A相电流
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[12] << 8 ) + modbusBuf[13] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3317;  //B相电流
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[14] << 8 ) + modbusBuf[15] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3318;  //C相电流
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[16] << 8 ) + modbusBuf[17] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3319;//有功功率
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[18] << 8 ) + modbusBuf[19] +
                                          ( ( uint32_t ) modbusBuf[20] << 24 ) + ( ( uint32_t ) modbusBuf[21]<<16 ) ) ) /10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3321;//无功功率
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[22] << 8 ) + modbusBuf[23] +
                                          ( ( uint32_t ) modbusBuf[24] << 24 ) + ( ( uint32_t ) modbusBuf[25]<<16 ) ) ) /10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3323;//正级对地阻抗
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[26] << 8 ) + modbusBuf[27] +
                                          ( ( uint32_t ) modbusBuf[28] << 24 ) + ( ( uint32_t ) modbusBuf[29]<<16 ) ) ) /100;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;

    dataPointer[dataIndex].dataID       = 3325;//负级对地阻抗
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[30] << 8 ) + modbusBuf[31] +
                                          ( ( uint32_t ) modbusBuf[32] << 24 ) + ( ( uint32_t ) modbusBuf[33]<<16 ) ) ) /100;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


#if 1
    dataPointer[dataIndex].dataID       = 3333;  //充放电状态
    dataPointer[dataIndex].val.value_32  = ( uint32_t ) ( ( uint32_t ) ( ( ( uint16_t ) modbusBuf[46] << 8 ) + modbusBuf[47] ) ) ;
    dataPointer[dataIndex].val_type     = TYPE_UINT32;
    dataIndex++;
#endif


    dataPointer[dataIndex].dataID       = 3334;// 日充电量
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[48] << 8 ) + modbusBuf[49] +
                                          ( ( uint32_t ) modbusBuf[50] << 24 ) + ( ( uint32_t ) modbusBuf[51]<<16 ) ) ) /10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;



    dataPointer[dataIndex].dataID       = 3336;//日放电量
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[52] << 8 ) + modbusBuf[53] +
                                          ( ( uint32_t ) modbusBuf[54] << 24 ) + ( ( uint32_t ) modbusBuf[55]<<16 ) ) ) /10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;



    dataPointer[dataIndex].dataID       = 3338;//总充电量
    dataPointer[dataIndex].val.value_32  = ( uint32_t ) ( ( uint32_t ) ( ( ( uint32_t ) modbusBuf[56] << 8 ) + modbusBuf[57] +
                                           ( ( uint32_t ) modbusBuf[58] << 24 ) + ( ( uint32_t ) modbusBuf[59]<<16 ) ) );
    dataPointer[dataIndex].val_type     = TYPE_UINT32;
    dataIndex++;



    dataPointer[dataIndex].dataID       = 3340;//总放电量
    dataPointer[dataIndex].val.value_32  = ( uint32_t ) ( ( uint32_t ) ( ( ( uint32_t ) modbusBuf[60] << 8 ) + modbusBuf[61] +
                                           ( ( uint32_t ) modbusBuf[62] << 24 ) + ( ( uint32_t ) modbusBuf[63]<<16 ) ) );
    dataPointer[dataIndex].val_type     = TYPE_UINT32;
    dataIndex++;




    dataPointer[dataIndex].dataID       = 3348;  //额定输出功率
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[76] << 8 ) + modbusBuf[77] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3349;  //额定输出无功功率
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[78] << 8 ) + modbusBuf[79] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3352;  //电网频率
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[84] << 8 ) + modbusBuf[85] ) ) / 100;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3353;  //功率因素
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[86] << 8 ) + modbusBuf[87] ) ) / 1000;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;

    dataPointer[dataIndex].dataID       = 3354;  //环境温度
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[88] << 8 ) + modbusBuf[89] ) ) / 10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;




    dataPointer[dataIndex].dataID       = 3357;  //日充电时数
    dataPointer[dataIndex].val.value_32  = ( uint32_t ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[94] << 8 ) + modbusBuf[95] ) );
    dataPointer[dataIndex].val_type     = TYPE_UINT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3358;  //日放电时数
    dataPointer[dataIndex].val.value_32  = ( uint32_t ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[96] << 8 ) + modbusBuf[97] ) );
    dataPointer[dataIndex].val_type     = TYPE_UINT32;
    dataIndex++;




    dataPointer[dataIndex].dataID       = 3359;//总充电时数
    dataPointer[dataIndex].val.value_32  = ( uint32_t ) ( ( uint32_t ) ( ( ( uint32_t ) modbusBuf[98] << 8 ) + modbusBuf[99] +
                                           ( ( uint32_t ) modbusBuf[100] << 24 ) + ( ( uint32_t ) modbusBuf[101]<<16 ) ) );
    dataPointer[dataIndex].val_type     = TYPE_UINT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3361;//总放电时数
    dataPointer[dataIndex].val.value_32  = ( uint32_t ) ( ( uint32_t ) ( ( ( uint32_t ) modbusBuf[102] << 8 ) + modbusBuf[103] +
                                           ( ( uint32_t ) modbusBuf[104] << 24 ) + ( ( uint32_t ) modbusBuf[105]<<16 ) ) );
    dataPointer[dataIndex].val_type     = TYPE_UINT32;
    dataIndex++;





    dataPointer[dataIndex].dataID       = 3363;//可用充电功率
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[106] << 8 ) + modbusBuf[107] +
                                          ( ( uint32_t ) modbusBuf[108] << 24 ) + ( ( uint32_t ) modbusBuf[109]<<16 ) ) ) /10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3365;//可用放电功率
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[110] << 8 ) + modbusBuf[111] +
                                          ( ( uint32_t ) modbusBuf[112] << 24 ) + ( ( uint32_t ) modbusBuf[113]<<16 ) ) ) /10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;


    dataPointer[dataIndex].dataID       = 3367;//可用感性无功
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[114] << 8 ) + modbusBuf[115] +
                                          ( ( uint32_t ) modbusBuf[116] << 24 ) + ( ( uint32_t ) modbusBuf[117]<<16 ) ) ) /10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;

    dataPointer[dataIndex].dataID       = 3369;//可用容性无功
    dataPointer[dataIndex].val.value_f  = ( float ) ( ( int32_t ) ( ( ( uint32_t ) modbusBuf[118] << 8 ) + modbusBuf[119] +
                                          ( ( uint32_t ) modbusBuf[120] << 24 ) + ( ( uint32_t ) modbusBuf[121]<<16 ) ) ) /10;
    dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
    dataIndex++;



    dataPointer[dataIndex].dataID       = 3371;//工作状态
    dataPointer[dataIndex].val.value_32  = ( uint32_t ) ( ( int32_t ) ( ( ( uint16_t ) modbusBuf[122] << 8 ) + modbusBuf[123] ) );
    dataPointer[dataIndex].val_type     = TYPE_UINT32;
    dataIndex++;




    headPointer->dataNum = dataIndex;
    headPointer->bodySize= headPointer->dataNum * sizeof ( YC_FIFO_DATA_T ); //data bytes

    totalSize = headPointer->bodySize + sizeof ( FifoHeader );
    if ( totalSize > sizeof ( FifoHeader ) )
    {
        ret = write ( to61850PipeHandle, &g_ycPacketSendByPipe, totalSize );
#if DEBUG_OPEN
        printf ( "yc ret %d to61850PipeHandle %d  totalSize %d  \n",ret,to61850PipeHandle, totalSize );
#endif
        if ( totalSize != ret )
        {
            WRITE_TO_LOG ( "ERROR: Failed to write %d bytes yc to pipe<%s> @ ret=%d @ %s\n",
                           totalSize, "to61850Pipe", ret, get_current_time_str() );
        }
    }

#if  DEBUG_OPEN
    printf ( "\n------- parse_yc_data_and_send_to_pipe--------\n" );
    int32_t  send_id = 0;
    for ( send_id  = 0  ; send_id < g_ycPacketSendByPipe.header.dataNum ; send_id ++ )
    {

        if ( g_ycPacketSendByPipe.data[ send_id ].val_type == TYPE_FLOAT32 )
        {
            printf ( "data_addr:%d  value_f: %f  \n",g_ycPacketSendByPipe.data[ send_id ].dataID,g_ycPacketSendByPipe.data[ send_id ].val.value_f );
        }
        else
        {
            printf ( "data_addr:%d  value_u: 0x%0x  \n",g_ycPacketSendByPipe.data[ send_id ].dataID,g_ycPacketSendByPipe.data[ send_id ].val.value_32 );
        }


    }
    printf ( "\n" );

#endif


    return 0;

}


#if 0
//---------------------------------------------------------------------------------------------------------------------------- 3310~3344   35
uint8_t cmd_3x_01[] = {0x01, 0x04, 0x0c, 0xed, 0x00, 0x23, 0x22  , 0xB6};

uint16_t cmd_3x_01_len =  35;



//---------------------------------------------------------------------------------------------------------------------------- 3359~3362  4
uint8_t cmd_3x_02[] = {0x01, 0x04, 0x0d, 0x1e, 0x00, 0x4, 0x93  , 0x63};
uint16_t cmd_3x_02_len =  4;


//---------------------------------------------------------------------------------------------------------------------------- 3256~3306  50
uint8_t cmd_4x_01[] = {0x01, 0x03, 0x0c, 0xb7, 0x00, 0x32, 0x77  , 0x69};
uint16_t cmd_4x_01_len =  50;
//---------------------------------------------------------------------------------------------------------------------------- 3319~3320  2
uint8_t cmd_4x_02[] = {0x01, 0x03, 0x0c, 0xf6, 0x00, 0x02, 0x27  , 0x69};

uint16_t cmd_4x_02_len =  2;

#endif
//---------------------------------------------------------------------------------------------------------------------------- 3310~3371   访问地址 3309 个数  62
uint8_t cmd_3x_01[ ] = { 0x01, 0x04, 0x0c, 0xed, 0x00, 0x3e, 0xE2 ,0xBF };

uint16_t cmd_3x_01_len =  62;

//---------------------------------------------------------------------------------------------------------------------------- 3256~3320 65
uint8_t cmd_4x_01[ ] = { 0x01, 0x03, 0x0c, 0xb7, 0x00, 0x41, 0x36 ,0x8C};
uint16_t cmd_4x_01_len =  65;


//---------------------------------------------------------------------------------------------------------------------------- 3330~3412 83
uint8_t cmd_4x_02[ ] = { 0x01, 0x03, 0x0d, 0x01, 0x00, 0x53, 0x56 ,0x9B};
uint16_t cmd_4x_02_len =  83;




static int32_t get_yx_yc_data ( int32_t serialPortHandle, int32_t to61850PipeHandle,uint8_t devNo )
{
    int32_t ret = -1;
    int32_t expectation = 0;
    int k=0;
    int32_t readOutBytes =0;

    //--------------------------------------------------------------------------------------------------------------------------------------------------


    readOutBytes = send_cmd_and_wait_for_return_data ( serialPortHandle, cmd_3x_01, 8, ( 4 * ( 8 + 2 * cmd_3x_01[5] + 5 ) ) );
    UPDATE_LOST_PKTS_COUNTER ( devNo );

    do
    {
        if ( readOutBytes <= 0 )
        {
            if ( DEV_IS_LOST==g_isDevLost[ARRAY_ID ( devNo )] )
            {
            }
            else
            {

                WRITE_TO_LOG ( "ERROR: Nothing come back @ 'YX YC' read CMD  , readOutBytes = %d @ %s.\n",
                               readOutBytes,get_current_time_str() );
                k = 0;
                WRITE_TO_LOG_WHILE_ERROR ( k, cmd_3x_01,8 );
            }
            break;
        }

        expectation = cmd_3x_01[5]*2 + 5;
        if ( readOutBytes != expectation )
        {
            break;
        }

        if ( 0 != get_crc16 ( g_serialPortReadBuf, readOutBytes ) )
        {
            WRITE_TO_LOG ( "ERROR: get_yx_yc_data() CRC Error! @ dev(%d) @ %s\n",
                           devNo,get_current_time_str() );
            WRITE_TO_LOG_WHILE_ERROR ( k,g_serialPortReadBuf,readOutBytes );
            break;
        }

        CLEAR_LOST_PKTS_COUNTER ( devNo );
        //
        ret = parse_yc_data_and_send_to_pipe ( to61850PipeHandle, &g_serialPortReadBuf[3], devNo , 1 );
    }
    while ( 0 );

#if 0
    //----------------------------------------------------------------------------------------------------------------------------
    readOutBytes = send_cmd_and_wait_for_return_data ( serialPortHandle, cmd_3x_02, 8, ( 4 * ( 8 + 2 * cmd_3x_02[5] + 5 ) ) );

    UPDATE_LOST_PKTS_COUNTER ( devNo );

    do
    {
        if ( readOutBytes <= 0 )
        {
            if ( DEV_IS_LOST==g_isDevLost[ARRAY_ID ( devNo )] )
            {
            }
            else
            {
                WRITE_TO_LOG ( "ERROR: Nothing come back from dev(%d) under 'YX YC' read CMD @ %s.\n",devNo,
                               get_current_time_str() );
                k = 0;
                WRITE_TO_LOG_WHILE_ERROR ( k, cmd_3x_02,8 );
            }
            break;
        }

        expectation = cmd_3x_02[5]*2 + 5;
        if ( readOutBytes != expectation )
        {
            break;
        }

        if ( 0 != get_crc16 ( g_serialPortReadBuf, readOutBytes ) )
        {
            WRITE_TO_LOG ( "ERROR: get_yx_yc_data() CRC Error! @ dev(%d) @ %s\n",
                           devNo,get_current_time_str() );
            WRITE_TO_LOG_WHILE_ERROR ( k,g_serialPortReadBuf,readOutBytes );
            break;
        }

        CLEAR_LOST_PKTS_COUNTER ( devNo );
        //
        ret = parse_yc_data_and_send_to_pipe ( to61850PipeHandle, &g_serialPortReadBuf[3], devNo, 2 );
    }
    while ( 0 );
#endif

    return ret;
}


//----------------------------------------------------------------------------------------------------------------------------
static int32_t parse_yx_data_and_packet_it ( uint8_t * modbusBuf, int devNo )
{
    YX_FIFO_DATA_T* dataPointer = g_yxPacketSendByPipe.data;
    int dataIndex = 0;
    int i =0;

    g_yxPacketSendByPipe.header.dataType[0] = 'y';
    g_yxPacketSendByPipe.header.dataType[1] = 'x';
    g_yxPacketSendByPipe.header.dataType[2] = '\0';
    g_yxPacketSendByPipe.header.devType = DEV_TYPE; //500K
    g_yxPacketSendByPipe.header.devNo = devNo;
    g_yxPacketSendByPipe.header.resv = 0;

    static uint32_t lastFltState1 = 0, lastFltState1Bit[32] = {0};
    static uint32_t lastFltState2 = 0, lastFltState2Bit[32] = {0};
    static uint32_t lastalarm_run_state =0, lastalarm_run_stateBit[32] = {0};
    static uint32_t lastalarm_run_state2 = 0, lastalarm_run_state2Bit[32] = {0};
    static uint32_t last_work_state =0, last_work_stateBit[32] = {0};
    static uint32_t last_work_mode = 0,  last_work_modeBit[32] = {0};

    static uint32_t last_charging = 0,  last_chargingBit[32] = {0};

    static uint32_t newFltState1 = 0, newFltState1Bit[32] = {0};
    static uint32_t newFltState2 = 0,  newFltState2Bit[32] = {0};
    static uint32_t newalarm_run_state =0, newalarm_run_stateBit[32] = {0};
    static uint32_t newalarm_run_state2 = 0,newalarm_run_state2Bit[32] = {0};
    static uint32_t new_work_state =0,  new_work_stateBit[32] = {0};
    static uint32_t new_work_mode = 0,new_work_modeBit[32] = {0};

    static uint32_t new_charging = 0,  new_chargingBit[32] = {0};


    static uint32_t last_pcs_node = 0,  last_pcs_nodeBit[32] = {0};

    static uint32_t new_pcs_node = 0, new_pcs_nodeBit[32] = {0};


    // 3327 3328 3329 3331 3342  3344
    newalarm_run_state  = ( uint32_t ) ( ( uint32_t ) ( ( ( uint16_t ) modbusBuf[34] << 8 ) + modbusBuf[35] ) ) ;  //告警运行状态1

    newalarm_run_state2 = ( uint32_t ) ( ( uint32_t ) ( ( ( uint16_t ) modbusBuf[36] << 8 ) + modbusBuf[37] ) ) ;  //告警运行状态2

    newFltState1  = ( uint32_t ) ( ( uint32_t ) ( ( ( uint32_t ) modbusBuf[38] << 8 ) + modbusBuf[39] +
                                   ( ( uint32_t ) modbusBuf[40] << 24 ) + ( ( uint32_t ) modbusBuf[41]<<16 ) ) );  //故障状态1

    newFltState2 = ( uint32_t ) ( ( uint32_t ) ( ( ( uint32_t ) modbusBuf[42] << 8 ) + modbusBuf[43] +
                                  ( ( uint32_t ) modbusBuf[44] << 24 ) + ( ( uint32_t ) modbusBuf[45]<<16 ) ) );  //故障状态2

#if 0
    new_work_state = ( uint32_t ) ( ( uint32_t ) ( ( ( uint32_t ) modbusBuf[64] << 8 ) + modbusBuf[65] +
                                    ( ( uint32_t ) modbusBuf[66] << 24 ) + ( ( uint32_t ) modbusBuf[67]<<16 ) ) );  //工作状态
#endif

    new_work_mode = ( uint32_t ) ( ( uint32_t ) ( ( ( uint16_t ) modbusBuf[68] << 8 ) + modbusBuf[69] ) ) ;  //工作模式

#if 0
    new_charging = ( uint32_t ) ( ( uint32_t ) ( ( ( uint16_t ) modbusBuf[46] << 8 ) + modbusBuf[47] ) ) ;  //充放电状态

#endif

    new_pcs_node = ( uint32_t ) ( ( uint32_t ) ( ( ( uint32_t ) modbusBuf[90] << 8 ) + modbusBuf[91] +
                                  ( ( uint32_t ) modbusBuf[92] << 24 ) + ( ( uint32_t ) modbusBuf[93]<<16 ) ) );  //pcs节点状态


    if ( newFltState1 == lastFltState1 )
    {

    }
    else
    {
        for ( i=0; i<32; i++ )
        {
            newFltState1Bit[i] = ( newFltState1 >> i ) & 0x01;
            if ( newFltState1Bit[i] == lastFltState1Bit[i] )
            {

            }
            else
            {
                dataPointer[dataIndex].dataID = i+1 ;
                dataPointer[dataIndex].val    = newFltState1Bit[i];
                dataIndex++;
                lastFltState1Bit[i] = newFltState1Bit[i];
            }
        }
        lastFltState1 = newFltState1;
    }

    //
    // newFltState2 = ( ( ( uint32_t ) modbusBuf[4] << 8 ) + modbusBuf[5] ) + ( ( ( uint32_t ) modbusBuf[6] << 24 ) + ( ( uint32_t ) modbusBuf[7]<<16 ) );
    if ( newFltState2 == lastFltState2 ) //
    {

    }
    else
    {
        for ( i=0; i<32; i++ )
        {
            newFltState2Bit[i] = ( newFltState2 >> i ) & 0x01;
            if ( newFltState2Bit[i] == lastFltState2Bit[i] )
            {

            }
            else
            {
                dataPointer[dataIndex].dataID = i+1+32;
                dataPointer[dataIndex].val    = newFltState2Bit[i];
                dataIndex++;
                lastFltState2Bit[i] = newFltState2Bit[i];
            }
        }
        lastFltState2 = newFltState2;
    }

    //
    //  newalarm_run_state = ( ( ( uint32_t ) modbusBuf[16] << 8 ) + modbusBuf[17] ) + ( ( ( uint32_t ) modbusBuf[18] << 24 ) + ( ( uint32_t ) modbusBuf[19]<<16 ) );
    if ( newalarm_run_state == lastalarm_run_state ) //
    {

    }
    else
    {
        for ( i=0; i<16; i++ )
        {
            newalarm_run_stateBit[i] = ( newalarm_run_state >> i ) & 0x01;
            if ( newalarm_run_stateBit[i] == lastalarm_run_stateBit[i] )
            {

            }
            else
            {
                dataPointer[dataIndex].dataID = i+1 +64;
                dataPointer[dataIndex].val    = newalarm_run_stateBit[i];
                dataIndex++;
                lastalarm_run_stateBit[i] = newalarm_run_stateBit[i];
            }
        }
        lastalarm_run_state = newalarm_run_state;
    }

    //
    //  newalarm_run_state2 = ( ( ( uint32_t ) modbusBuf[20] << 8 ) + modbusBuf[21] ) + ( ( ( uint32_t ) modbusBuf[22] << 24 ) + ( ( uint32_t ) modbusBuf[23]<<16 ) );
    if ( newalarm_run_state2 == lastalarm_run_state2 ) //
    {

    }
    else
    {
        for ( i=0; i<16; i++ )
        {
            newalarm_run_state2Bit[i] = ( newalarm_run_state2 >> i ) & 0x01;
            if ( newalarm_run_state2Bit[i] == lastalarm_run_state2Bit[i] )
            {

            }
            else
            {
                dataPointer[dataIndex].dataID = i+1+80;
                dataPointer[dataIndex].val    = newalarm_run_state2Bit[i];
                dataIndex++;
                lastalarm_run_state2Bit[i] = newalarm_run_state2Bit[i];
            }
        }
        lastalarm_run_state2 = newalarm_run_state2;
    }

    //
    // new_work_state = ( ( ( uint32_t ) modbusBuf[82] << 8 ) + modbusBuf[83] ) + ( ( ( uint32_t ) modbusBuf[84] << 24 ) + ( ( uint32_t ) modbusBuf[85]<<16 ) );

    if ( new_work_state == last_work_state )
    {

    }
    else
    {

        for ( i=0; i<32; i++ )
        {
            new_work_stateBit[i] = ( new_work_state >> i ) & 0x01;
            if ( new_work_stateBit[i] == last_work_stateBit[i] )
            {

            }
            else
            {
                dataPointer[dataIndex].dataID = i+1+96;
                dataPointer[dataIndex].val    = new_work_stateBit[i];
                //WRITE_TO_LOG("alarm id = %d , val = %d\n", dataPointer[dataIndex].dataID , dataPointer[dataIndex].val );
                dataIndex++;
                last_work_stateBit[i] = new_work_stateBit[i];
            }
        }
        last_work_state = new_work_state;
    }

    //
    // new_work_mode = ( ( ( uint32_t ) modbusBuf[62] << 8 ) + modbusBuf[63] ) + ( ( ( uint32_t ) modbusBuf[64] << 24 ) + ( ( uint32_t ) modbusBuf[65]<<16 ) );
    if ( new_work_mode == last_work_mode )
    {

    }
    else
    {
        for ( i=0; i<16; i++ )
        {
            new_work_modeBit[i] = ( new_work_mode >> i ) & 0x01;
            if ( new_work_modeBit[i] == last_work_modeBit[i] )
            {

            }
            else
            {

                dataPointer[dataIndex].dataID = i+1+128;
                dataPointer[dataIndex].val    = new_work_modeBit[i];
                dataIndex++;
                last_work_modeBit[i] = new_work_modeBit[i];
            }
        }
        last_work_mode = new_work_mode;
    }

    if ( new_charging == last_charging )
    {

    }
    else
    {
        for ( i=0; i<16; i++ )
        {
            new_chargingBit[i] = ( new_charging >> i ) & 0x01;
            if ( new_chargingBit[i] == last_chargingBit[i] )
            {

            }
            else
            {

                dataPointer[dataIndex].dataID = i+1+144;
                dataPointer[dataIndex].val    = new_chargingBit[i];
                dataIndex++;
                last_work_modeBit[i] = new_chargingBit[i];
            }
        }
        last_charging = new_charging  ;
    }


    if ( new_pcs_node == last_pcs_node )
    {

    }
    else
    {
        for ( i=0; i<32; i++ )
        {
            new_pcs_nodeBit[i] = ( new_pcs_node >> i ) & 0x01;
            if ( new_pcs_nodeBit[i] == last_pcs_nodeBit[i] )
            {

            }
            else
            {

                dataPointer[dataIndex].dataID = i+1+193;
                dataPointer[dataIndex].val    = new_pcs_nodeBit[i];
                dataIndex++;
                last_pcs_nodeBit[i] = new_pcs_nodeBit[i];
            }
        }
        last_pcs_node = new_pcs_node  ;
    }




    g_yxPacketSendByPipe.header.dataNum  = dataIndex;
    g_yxPacketSendByPipe.header.bodySize = g_yxPacketSendByPipe.header.dataNum * sizeof ( YX_FIFO_DATA_T );

    int totalSize = g_yxPacketSendByPipe.header.bodySize + sizeof ( FifoHeader );

#if  DEBUG_OPEN

    printf ( "\n-- get_yx_data_and_send_to_pipe--\n" );

    int32_t  send_id = 0;
    for ( send_id  = 0  ; send_id < g_yxPacketSendByPipe.header.dataNum ; send_id ++ )
    {

        printf ( "data_addr:%d  value_u: 0x%0x  \n",g_yxPacketSendByPipe.data[ send_id ].dataID,g_yxPacketSendByPipe.data[ send_id ].val );

    }
    printf ( "\n" );

#endif


    return totalSize;
}


/*

*/
static int32_t get_yx_yc_data_and_send_to_pipe ( uint8_t devNo,
        int32_t serialPortHandle,
        int32_t to61850PipeHandle,
        const char* to61850PipeName )
{
    int32_t bytesToWrite = 0;
    int32_t ret = 0;

    do
    {
        if ( 0 != get_yx_yc_data ( serialPortHandle, to61850PipeHandle, devNo ) ) //
        {
            break;
        }

        bytesToWrite = parse_yx_data_and_packet_it ( &g_serialPortReadBuf[3], g_serialPortReadBuf[0] ); //


        if ( bytesToWrite > sizeof ( FifoHeader ) )
        {
            ret = write ( to61850PipeHandle, &g_yxPacketSendByPipe, bytesToWrite );
#if DEBUG_OPEN
            printf ( "yx ret %d to61850PipeHandle %d  bytesToWrite %d  \n",ret,to61850PipeHandle, bytesToWrite );
#endif
            if ( bytesToWrite != ret )
            {
                WRITE_TO_LOG ( "ERROR: Failed to write %d bytes yx to pipe<%s> @ ret=%d @ %s\n",
                               bytesToWrite, to61850PipeName, ret, get_current_time_str() );
            }
        }

    }
    while ( 0 );

    return 0;
}


/*03*/
static int32_t get_yk_yt_data_from_modbus ( int32_t serialPortHandle,uint8_t devNo, uint8_t sendBuf[ ] )
{
    //  uint8_t  sendBuf[8] = {devNo,0x03,0x13,0x87,0x00,0x32,0,0};

    sendBuf[0] = devNo;
    uint16_t crc = get_crc16 ( sendBuf, 6 );
    sendBuf[6] = crc & 0xFF;
    sendBuf[7] = ( crc >> 8 ) & 0xFF;
    int i =0;


    int32_t count = send_cmd_and_wait_for_return_data ( serialPortHandle, sendBuf, 8, ( 4 * ( 8 + 2 * sendBuf[5] + 5 ) ) );

    UPDATE_LOST_PKTS_COUNTER ( devNo );

    do
    {
        if ( count <= 0 )
        {
            if ( DEV_IS_LOST==g_isDevLost[ARRAY_ID ( devNo )] )
            {
            }
            else
            {
                WRITE_TO_LOG ( "ERROR: Nothing come back @ 'YT' read CMD  , count = %d @ %s.\n",
                               count,get_current_time_str() );
                WRITE_TO_LOG_WHILE_ERROR ( i, sendBuf,8 );
                i = 0;
            }
            break;
        }

        if ( 0 != get_crc16 ( g_serialPortReadBuf, count ) )
        {
            WRITE_TO_LOG ( "ERROR: CRC ERROR  @ 'YT' read CMD:\n" );
            i = 0;
            WRITE_TO_LOG_WHILE_ERROR ( i, g_serialPortReadBuf, count );

            count = 0; //
            break;
        }

        CLEAR_LOST_PKTS_COUNTER ( devNo );

        int expectation = sendBuf[5]*2 + 5;
        if ( expectation == count ) //
        {
        }
        else
        {
            count = 0;//
        }

    }
    while ( 0 );

    return count;
}


/**/
static int32_t parse_yt_data_and_packet_it_4x01 ( const uint8_t * modbusBuf, uint32_t devNo )
{
    YT_FIFO_DATA_T* dataPointer = g_ytPacketSendByPipe.data;
    int32_t dataIndex = 0;
    uint16_t modbusReg = 0;

    g_ytPacketSendByPipe.header.dataType[0] = 'y';
    g_ytPacketSendByPipe.header.dataType[1] = 't';
    g_ytPacketSendByPipe.header.dataType[2] = '\0';
    g_ytPacketSendByPipe.header.devType = DEV_TYPE;//500K
    g_ytPacketSendByPipe.header.devNo = devNo;
    g_ytPacketSendByPipe.header.resv = 0;





    //3256  -3256
    modbusReg = ( ( ( uint16_t ) modbusBuf[0] << 8 ) + modbusBuf[1] );
    if ( yt_data_is_changed ( devNo,modbusReg ,0 ) )
    {
        dataPointer[dataIndex].dataID       = 3256;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }

    //3257  -3256
    modbusReg = ( ( ( uint16_t ) modbusBuf[2] << 8 ) + modbusBuf[3] );
    if ( yt_data_is_changed ( devNo,modbusReg ,1 ) )
    {
        dataPointer[dataIndex].dataID       = 3257;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }

    //3258
    modbusReg = ( ( ( uint16_t ) modbusBuf[4] << 8 ) + modbusBuf[5] );
    if ( yt_data_is_changed ( devNo,modbusReg ,2 ) )
    {
        dataPointer[dataIndex].dataID       = 3258;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }

    //3259
    modbusReg = ( ( ( uint16_t ) modbusBuf[6] << 8 ) + modbusBuf[7] );
    if ( yt_data_is_changed ( devNo,modbusReg ,3 ) )
    {
        dataPointer[dataIndex].dataID       = 3259;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }
    //3260
    modbusReg = ( ( ( uint16_t ) modbusBuf[8] << 8 ) + modbusBuf[9] );
    if ( yt_data_is_changed ( devNo,modbusReg ,4 ) )
    {
        dataPointer[dataIndex].dataID       = 3260;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }

    //3261
    modbusReg = ( ( ( uint16_t ) modbusBuf[10] << 8 ) + modbusBuf[11] );
    if ( yt_data_is_changed ( devNo,modbusReg ,5 ) )
    {
        dataPointer[dataIndex].dataID       = 3261;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }

    //3300
    modbusReg = ( ( ( uint16_t ) modbusBuf[88] << 8 ) + modbusBuf[89] );
    if ( yt_data_is_changed ( devNo,modbusReg ,6 ) )
    {
        dataPointer[dataIndex].dataID       = 3300;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }
    //3301-3412
    modbusReg = ( ( ( uint16_t ) modbusBuf[90] << 8 ) + modbusBuf[91] );
    if ( yt_data_is_changed ( devNo,modbusReg ,7 ) )
    {
        dataPointer[dataIndex].dataID       = 3301;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( int16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }


    //3302-3412
    modbusReg = ( ( ( uint16_t ) modbusBuf[92] << 8 ) + modbusBuf[93] ) ;
    if ( yt_data_is_changed ( devNo,modbusReg ,8 ) )
    {
        dataPointer[dataIndex].dataID       = 3302;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3303--3412
    modbusReg = ( ( ( uint16_t ) modbusBuf[94] << 8 ) + modbusBuf[95] );
    if ( yt_data_is_changed ( devNo,modbusReg ,9 ) )
    {
        dataPointer[dataIndex].dataID       = 3303;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }


    //3305--3412
    modbusReg = ( ( ( uint16_t ) modbusBuf[98] << 8 ) + modbusBuf[99] );
    if ( yt_data_is_changed ( devNo,modbusReg ,11 ) )
    {
        dataPointer[dataIndex].dataID       = 3305;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3306--3412
    modbusReg = ( ( ( uint16_t ) modbusBuf[100] << 8 ) + modbusBuf[101] );
    if ( yt_data_is_changed ( devNo,modbusReg ,12 ) )
    {
        dataPointer[dataIndex].dataID       = 3306;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3307-3412
    modbusReg = ( ( ( uint16_t ) modbusBuf[102] << 8 ) + modbusBuf[103] );
    if ( yt_data_is_changed ( devNo,modbusReg ,13 ) )
    {
        dataPointer[dataIndex].dataID       = 3307;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /100;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3310-3412
    modbusReg = ( ( ( uint16_t ) modbusBuf[108] << 8 ) + modbusBuf[109] );
    if ( yt_data_is_changed ( devNo,modbusReg ,15 ) )
    {
        dataPointer[dataIndex].dataID       = 3310;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3311-3412
    modbusReg = ( ( ( uint16_t ) modbusBuf[110] << 8 ) + modbusBuf[111] );
    if ( yt_data_is_changed ( devNo,modbusReg ,16 ) )
    {
        dataPointer[dataIndex].dataID       = 3311;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }


    //3312
    modbusReg = ( ( ( uint16_t ) modbusBuf[112] << 8 ) + modbusBuf[113] );
    if ( yt_data_is_changed ( devNo,modbusReg ,17 ) )
    {
        dataPointer[dataIndex].dataID       = 3312;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }

    //3313
    modbusReg = ( ( ( uint16_t ) modbusBuf[114] << 8 ) + modbusBuf[115] );
    if ( yt_data_is_changed ( devNo,modbusReg ,18 ) )
    {
        dataPointer[dataIndex].dataID       = 3313;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }
    //3314
    modbusReg = ( ( ( uint16_t ) modbusBuf[116] << 8 ) + modbusBuf[117] );
    if ( yt_data_is_changed ( devNo,modbusReg ,19 ) )
    {
        dataPointer[dataIndex].dataID       = 3314;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }



    //3315
    modbusReg = ( ( ( uint16_t ) modbusBuf[118] << 8 ) + modbusBuf[119] );
    if ( yt_data_is_changed ( devNo,modbusReg ,20 ) )
    {
        dataPointer[dataIndex].dataID       = 3315;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3320
    modbusReg = ( ( ( uint16_t ) modbusBuf[128] << 8 ) + modbusBuf[129] );
    if ( yt_data_is_changed ( devNo,modbusReg ,21 ) )
    {
        dataPointer[dataIndex].dataID       = 3320;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /1000;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    g_ytPacketSendByPipe.header.dataNum = dataIndex;
    g_ytPacketSendByPipe.header.bodySize= g_ytPacketSendByPipe.header.dataNum *
                                          sizeof ( YT_FIFO_DATA_T ); //data bytes

    int totalSize = g_ytPacketSendByPipe.header.bodySize + sizeof ( FifoHeader );


#if  DEBUG_OPEN

    printf ( "\n-- get_yk_yt_data_and_send_to_pipe--\n" );

    int32_t  send_id = 0;
    for ( send_id  = 0  ; send_id < g_ytPacketSendByPipe.header.dataNum ; send_id ++ )
    {

        if ( g_ytPacketSendByPipe.data[ send_id ].val_type == TYPE_FLOAT32 )
        {
            printf ( "data_addr:%d  value_f: %f  \n",g_ytPacketSendByPipe.data[ send_id ].dataID,g_ytPacketSendByPipe.data[ send_id ].val.value_f );
        }
        else
        {
            printf ( "data_addr:%d  value_u: 0x%0x  \n",g_ytPacketSendByPipe.data[ send_id ].dataID,g_ytPacketSendByPipe.data[ send_id ].val.value_32 );
        }

    }
    printf ( "\n" );

#endif



    return totalSize;
}



/**/
static int32_t parse_yt_data_and_packet_it_4x02 ( const uint8_t * modbusBuf, uint32_t devNo )
{
    YT_FIFO_DATA_T* dataPointer = g_ytPacketSendByPipe.data;
    int32_t dataIndex = 0;
    uint16_t modbusReg = 0;

    g_ytPacketSendByPipe.header.dataType[0] = 'y';
    g_ytPacketSendByPipe.header.dataType[1] = 't';
    g_ytPacketSendByPipe.header.dataType[2] = '\0';
    g_ytPacketSendByPipe.header.devType = DEV_TYPE;//500K
    g_ytPacketSendByPipe.header.devNo = devNo;
    g_ytPacketSendByPipe.header.resv = 0;


    //3300-3412

    //3330
    modbusReg = ( ( ( uint16_t ) modbusBuf[0] << 8 ) + modbusBuf[1] );
    if ( yt_data_is_changed ( devNo,modbusReg ,22 ) )
    {
        dataPointer[dataIndex].dataID       = 3330;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3331
    modbusReg = ( ( ( uint16_t ) modbusBuf[2] << 8 ) + modbusBuf[3] );
    if ( yt_data_is_changed ( devNo,modbusReg ,23 ) )
    {
        dataPointer[dataIndex].dataID       = 3331;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /100;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3332
    modbusReg = ( ( ( uint16_t ) modbusBuf[4] << 8 ) + modbusBuf[5] );
    if ( yt_data_is_changed ( devNo,modbusReg ,24 ) )
    {
        dataPointer[dataIndex].dataID       = 3332;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3333
    modbusReg = ( ( ( uint16_t ) modbusBuf[6] << 8 ) + modbusBuf[7] );
    if ( yt_data_is_changed ( devNo,modbusReg ,25 ) )
    {
        dataPointer[dataIndex].dataID       = 3333;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3334
    modbusReg = ( ( ( uint16_t ) modbusBuf[8] << 8 ) + modbusBuf[9] );
    if ( yt_data_is_changed ( devNo,modbusReg ,26 ) )
    {
        dataPointer[dataIndex].dataID       = 3334;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3335
    modbusReg = ( ( ( uint16_t ) modbusBuf[10] << 8 ) + modbusBuf[11] );
    if ( yt_data_is_changed ( devNo,modbusReg ,27 ) )
    {
        dataPointer[dataIndex].dataID       = 3335;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3387
    modbusReg = ( ( ( uint16_t ) modbusBuf[114] << 8 ) + modbusBuf[115] );
    if ( yt_data_is_changed ( devNo,modbusReg ,28 ) )
    {
        dataPointer[dataIndex].dataID       = 3387;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }


    //3388
    modbusReg = ( ( ( uint16_t ) modbusBuf[116] << 8 ) + modbusBuf[117] );
    if ( yt_data_is_changed ( devNo,modbusReg ,29 ) )
    {
        dataPointer[dataIndex].dataID       = 3388;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3390
    modbusReg = ( ( ( uint16_t ) modbusBuf[120] << 8 ) + modbusBuf[121] );
    if ( yt_data_is_changed ( devNo,modbusReg ,30 ) )
    {
        dataPointer[dataIndex].dataID       = 3390;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3391
    modbusReg = ( ( ( uint16_t ) modbusBuf[122] << 8 ) + modbusBuf[123] );
    if ( yt_data_is_changed ( devNo,modbusReg ,31 ) )
    {
        dataPointer[dataIndex].dataID       = 3391;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3392
    modbusReg = ( ( ( uint16_t ) modbusBuf[124] << 8 ) + modbusBuf[125] );
    if ( yt_data_is_changed ( devNo,modbusReg ,32 ) )
    {
        dataPointer[dataIndex].dataID       = 3392;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3393
    modbusReg = ( ( ( uint16_t ) modbusBuf[126] << 8 ) + modbusBuf[127] );
    if ( yt_data_is_changed ( devNo,modbusReg ,33 ) )
    {
        dataPointer[dataIndex].dataID       = 3393;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3394
    modbusReg = ( ( ( uint16_t ) modbusBuf[128] << 8 ) + modbusBuf[129] );
    if ( yt_data_is_changed ( devNo,modbusReg ,34 ) )
    {
        dataPointer[dataIndex].dataID       = 3394;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3395
    modbusReg = ( ( ( uint16_t ) modbusBuf[130] << 8 ) + modbusBuf[131] );
    if ( yt_data_is_changed ( devNo,modbusReg ,35 ) )
    {
        dataPointer[dataIndex].dataID       = 3395;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }


    //3396
    modbusReg = ( ( ( uint16_t ) modbusBuf[132] << 8 ) + modbusBuf[133] );
    if ( yt_data_is_changed ( devNo,modbusReg ,36 ) )
    {
        dataPointer[dataIndex].dataID       = 3396;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }
    //3397
    modbusReg = ( ( ( uint16_t ) modbusBuf[134] << 8 ) + modbusBuf[135] );
    if ( yt_data_is_changed ( devNo,modbusReg ,37 ) )
    {
        dataPointer[dataIndex].dataID       = 3397;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }

    //3398
    modbusReg = ( ( ( uint16_t ) modbusBuf[136] << 8 ) + modbusBuf[137] );
    if ( yt_data_is_changed ( devNo,modbusReg ,38 ) )
    {
        dataPointer[dataIndex].dataID       = 3398;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /1000;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }
    //3399
    modbusReg = ( ( ( uint16_t ) modbusBuf[138] << 8 ) + modbusBuf[139] );
    if ( yt_data_is_changed ( devNo,modbusReg ,39 ) )
    {
        dataPointer[dataIndex].dataID       = 3399;//power limit 0.1%
        dataPointer[dataIndex].val.value_f  = ( float ) ( ( int16_t ) modbusReg ) /10;
        dataPointer[dataIndex].val_type     = TYPE_FLOAT32;
        dataIndex++;
    }


    //3404
    modbusReg = ( ( ( uint16_t ) modbusBuf[148] << 8 ) + modbusBuf[149] );
    if ( yt_data_is_changed ( devNo,modbusReg ,40 ) )
    {
        dataPointer[dataIndex].dataID       = 3404;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }

    //3405
    modbusReg = ( ( ( uint16_t ) modbusBuf[150] << 8 ) + modbusBuf[151] );
    if ( yt_data_is_changed ( devNo,modbusReg ,41 ) )
    {
        dataPointer[dataIndex].dataID       = 3405;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }

    //3411
    modbusReg = ( ( ( uint16_t ) modbusBuf[152] << 8 ) + modbusBuf[153] );
    if ( yt_data_is_changed ( devNo,modbusReg ,42 ) )
    {
        dataPointer[dataIndex].dataID       = 3411;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }

    //3412
    modbusReg = ( ( ( uint16_t ) modbusBuf[154] << 8 ) + modbusBuf[155] );
    if ( yt_data_is_changed ( devNo,modbusReg ,43 ) )
    {
        dataPointer[dataIndex].dataID       = 3412;//power limit 0.1%
        dataPointer[dataIndex].val.value_32  = ( ( uint16_t ) modbusReg )  ;
        dataPointer[dataIndex].val_type     = TYPE_UINT32;
        dataIndex++;
    }



    g_ytPacketSendByPipe.header.dataNum = dataIndex;
    g_ytPacketSendByPipe.header.bodySize= g_ytPacketSendByPipe.header.dataNum *
                                          sizeof ( YT_FIFO_DATA_T ); //data bytes

    int totalSize = g_ytPacketSendByPipe.header.bodySize + sizeof ( FifoHeader );


#if  DEBUG_OPEN

    printf ( "\n-- get_yk_yt_data_and_send_to_pipe--\n" );

    int32_t  send_id = 0;
    for ( send_id  = 0  ; send_id < g_ytPacketSendByPipe.header.dataNum ; send_id ++ )
    {

        if ( g_ytPacketSendByPipe.data[ send_id ].val_type == TYPE_FLOAT32 )
        {
            printf ( "data_addr:%d  value_f: %f  \n",g_ytPacketSendByPipe.data[ send_id ].dataID,g_ytPacketSendByPipe.data[ send_id ].val.value_f );
        }
        else
        {
            printf ( "data_addr:%d  value_u: 0x%0x  \n",g_ytPacketSendByPipe.data[ send_id ].dataID,g_ytPacketSendByPipe.data[ send_id ].val.value_32 );
        }

    }
    printf ( "\n" );

#endif



    return totalSize;
}

//if dev is lost, device state is immert
static  int32_t send_yk_to_pipe_while_dev_lost ( uint32_t devNo, int32_t to61850PipeHandle,  const char* to61850PipeName )
{
    YK_FIFO_DATA_T* dataPointer = g_ykPacketSendByPipe.data;
    int32_t dataIndex = 0;
    static uint8_t flag = 0;

    if ( DEV_IS_LOST == g_isDevLost[ARRAY_ID ( devNo )] )
    {
        g_ykPacketSendByPipe.header.dataType[0] = 'y';
        g_ykPacketSendByPipe.header.dataType[1] = 'k';
        g_ykPacketSendByPipe.header.dataType[2] = 'D';
        g_ykPacketSendByPipe.header.dataType[3] = '\0';
        g_ykPacketSendByPipe.header.devType = DEV_TYPE;
        g_ykPacketSendByPipe.header.devNo   = devNo;
        g_ykPacketSendByPipe.header.resv = 0;

        dataPointer[dataIndex].dataID = 5006;
        dataPointer[dataIndex].val = 0;
        dataIndex++;

        if ( 0 == flag )
        {
            WRITE_TO_LOG ( "dev is lost, send ykD @ 0\n" );
            g_ykPacketSendByPipe.header.dataNum = dataIndex;
            g_ykPacketSendByPipe.header.bodySize= g_ykPacketSendByPipe.header.dataNum *
                                                  sizeof ( YK_FIFO_DATA_T ); //data bytes

            int totalSize = g_ykPacketSendByPipe.header.bodySize + sizeof ( FifoHeader );
            int32_t ret = write ( to61850PipeHandle, &g_ykPacketSendByPipe, totalSize );
            if ( totalSize != ret )
            {
                WRITE_TO_LOG ( "ERROR: Failed to write %d bytes yk to pipe<%s> @ ret=%d @ %s\n",\
                               totalSize, to61850PipeName, ret, get_current_time_str() );
            }
            else
            {
                flag = 1;
            }
        }
    }
    else
    {
        flag = 0;
    }

    return 0;
}


#if 1
static int32_t parse_yk_data_and_packet_it ( const uint8_t * modbusBuf, uint32_t devNo )
{
    YK_FIFO_DATA_T* dataPointer = g_ykPacketSendByPipe.data;
    int32_t dataIndex = 0;
    uint16_t modbusReg =0;
    static uint16_t lastmodbusReg = 0;
    int totalSize = 0;

    g_ykPacketSendByPipe.header.dataType[0] = 'y';
    g_ykPacketSendByPipe.header.dataType[1] = 'k';
    g_ykPacketSendByPipe.header.dataType[2] = 'D';
    g_ykPacketSendByPipe.header.dataType[3] = '\0';
    g_ykPacketSendByPipe.header.devType = DEV_TYPE;
    g_ykPacketSendByPipe.header.devNo   = devNo;
    g_ykPacketSendByPipe.header.resv = 0;

    dataPointer[dataIndex].dataID = 5006;

    modbusReg = ( ( ( uint16_t ) modbusBuf[12] << 8 ) + modbusBuf[13] );
    if ( modbusReg != lastmodbusReg )
    {
        lastmodbusReg = modbusReg;
        if ( 0xCE == modbusReg )
        {
            dataPointer[dataIndex].val = 1;      //off
        }
        else if ( 0xCF == modbusReg )
        {
            dataPointer[dataIndex].val = 2;      //on
        }
        dataIndex++;
        g_ykPacketSendByPipe.header.dataNum = dataIndex;
        g_ykPacketSendByPipe.header.bodySize= g_ykPacketSendByPipe.header.dataNum *
                                              sizeof ( YK_FIFO_DATA_T ); //data bytes

        totalSize = g_ykPacketSendByPipe.header.bodySize + sizeof ( FifoHeader );
        return totalSize;
    }

    return 0;
}
#endif


static int32_t get_yk_yt_data_and_send_to_pipe ( uint8_t devNo,
        int32_t serialPortHandle,
        int32_t to61850PipeHandle,
        const char* to61850PipeName )
{
    int32_t bytesToWrite = 0;
    int32_t ret = 0;

    memset ( g_serialPortReadBuf ,0, sizeof ( g_serialPortReadBuf ) );
    ret = get_yk_yt_data_from_modbus ( serialPortHandle,devNo,cmd_4x_01 );

    do
    {
        if ( ret <= 0 )
        {
            break;  //
        }

        bytesToWrite = parse_yt_data_and_packet_it_4x01 ( &g_serialPortReadBuf[3],devNo );
        if ( bytesToWrite <= sizeof ( FifoHeader ) )
        {
            //
        }
        else
        {
            ret = write ( to61850PipeHandle, &g_ytPacketSendByPipe, bytesToWrite );
#if DEBUG_OPEN
            printf ( "yt ret %d to61850PipeHandle %d  bytesToWrite %d  \n",ret,to61850PipeHandle, bytesToWrite );
#endif
            if ( bytesToWrite != ret )
            {
                WRITE_TO_LOG ( "cmd_4x_01--1  %s \n   ERROR: Failed to write %d bytes yt to pipe<%s> @ ret=%d @ %s\n",
                               cmd_4x_01,bytesToWrite, to61850PipeName, ret, get_current_time_str() );
            }




        }

#if 1

//屏蔽遥控
        bytesToWrite = parse_yk_data_and_packet_it ( &g_serialPortReadBuf[3],devNo );
        if ( bytesToWrite <= sizeof ( FifoHeader ) )
        {
            break;  //
        }

        ret = write ( to61850PipeHandle, &g_ykPacketSendByPipe, bytesToWrite );
        if ( bytesToWrite != ret )
        {
            WRITE_TO_LOG ( "cmd_4x_01--2 %s \n  ERROR: Failed to write %d bytes yk to pipe<%s> @ ret=%d @ %s\n",
                           cmd_4x_01,bytesToWrite, to61850PipeName, ret, get_current_time_str() );
        }
        else
        {
        }
#endif

    }
    while ( 0 );



#if 1
    memset ( g_serialPortReadBuf ,0, sizeof ( g_serialPortReadBuf ) );
    ret = get_yk_yt_data_from_modbus ( serialPortHandle,devNo,cmd_4x_02 );

    do
    {
        if ( ret <= 0 )
        {
            break;  //
        }

        bytesToWrite = parse_yt_data_and_packet_it_4x02 ( &g_serialPortReadBuf[3],devNo );
        if ( bytesToWrite <= sizeof ( FifoHeader ) )
        {
            //
        }
        else
        {
            ret = write ( to61850PipeHandle, &g_ytPacketSendByPipe, bytesToWrite );
            if ( bytesToWrite != ret )
            {
                WRITE_TO_LOG ( "cmd_4x_02--1 %s \n  ERROR: Failed to write %d bytes yt to pipe<%s> @ ret=%d @ %s\n",
                               cmd_4x_02, bytesToWrite, to61850PipeName, ret, get_current_time_str() );
            }
        }

        bytesToWrite = parse_yk_data_and_packet_it ( &g_serialPortReadBuf[3],devNo );
        if ( bytesToWrite <= sizeof ( FifoHeader ) )
        {
            break;  //
        }

        ret = write ( to61850PipeHandle, &g_ykPacketSendByPipe, bytesToWrite );
        if ( bytesToWrite != ret )
        {
            WRITE_TO_LOG ( "cmd_4x_02--2 %s \n ERROR: Failed to write %d bytes yk to pipe<%s> @ ret=%d @ %s\n",
                           cmd_4x_02,bytesToWrite, to61850PipeName, ret, get_current_time_str() );
        }
        else
        {
        }

    }
    while ( 0 );
#endif



    return 0;
}


#if 1
//遥控 屏蔽
/* modbus*/
static int32_t deal_with_yk_info ( int serialPortHandle, const FifoHeader* headerInfo, const YK_FIFO_DATA_T* p_YkDataReadFromPipe )
{
    int16_t value = 0;

    SHOW_DEBUG_INFO ( "'YK' value is %d @ dev(%d).\n", p_YkDataReadFromPipe->val,headerInfo->devNo );

    if ( 3256 != p_YkDataReadFromPipe->dataID )
    {
        WRITE_TO_LOG ( "ERROR: 'YK' dataID(%04d != 5006) is WRONG @ dev(%d) @ %s.\n",
                       p_YkDataReadFromPipe->dataID, headerInfo->devNo, get_current_time_str() );
        return -1;
    }
    else
    {
        if ( 0==p_YkDataReadFromPipe->val )
        {
            value = 0xCE;
        }
        else
        {
            value = 0xCF;
        }
        set_a_value_to_modbus ( serialPortHandle , 5006,  value, headerInfo->devNo );
    }

    return 0;
}

#endif


//设置时间
static int32_t set_ds_value_to_modbus ( int32_t serialPortHandle, uint16_t *buf )
{
    uint8_t modBusCmd[21] = {0};
    //uint8_t readBuf[8] = {0x01,0x10,0x13,0x87,0x00,0x06,0xF4,0xA6};

    uint8_t readBuf[8] = {0x01,0x10,0x0c,0xb1,0x00,0x06,0x13,0x7C };

    uint8_t i=0;
    uint16_t crc = 0xffff;
    uint16_t tot_num = 0;
    int32_t sendTimes = 0;

    modBusCmd[0] = 0x01;
    modBusCmd[1] = 0x10;

    modBusCmd[2] = 0x0c;
    modBusCmd[3] = 0xb1;

    modBusCmd[4] = 0x00;
    modBusCmd[5] = 0x06;
    modBusCmd[6] = 0x0c;
    for ( i=0; i<6; i++ )
    {
        modBusCmd[7 + 2*i] = buf[i] >>8;
        modBusCmd[8 + 2*i] = buf[i] & 0xFF;
    }
    tot_num = 19;
    crc = get_crc16 ( modBusCmd, tot_num );
    modBusCmd[tot_num++] = crc & 0xFF;
    modBusCmd[tot_num++] = ( crc >> 8 ) & 0xFF;

    for ( sendTimes=0; sendTimes<3; sendTimes++ )
    {
        send_cmd_and_wait_for_return_data ( serialPortHandle, modBusCmd, tot_num, 250 );
        UPDATE_LOST_PKTS_COUNTER ( 0x01 );

        if ( 0 == memcmp ( readBuf, g_serialPortReadBuf, 8 ) )
        {
            CLEAR_LOST_PKTS_COUNTER ( 0x01 );

            WRITE_TO_LOG ( "ds time set ok, @ %s \n", get_current_time_str() );

            break;
        }
        else
        {
            WRITE_TO_LOG ( "ds error@ modbus reply is wrong  @ time %d:\n", sendTimes+1 );

            WRITE_TO_LOG ( "Set:    " );
            i=0;
            WRITE_TO_LOG_WHILE_ERROR ( i,modBusCmd,tot_num );

            WRITE_TO_LOG ( "Return: " );
            WRITE_TO_LOG_WHILE_ERROR ( i, g_serialPortReadBuf, 8 );
        }
    }
    return 0;

}


static int32_t deal_with_ds_info ( int32_t serialPortHandle )
{
    uint16_t timeBuf[6] = {0};
    time_t rawTime = time ( NULL );
    struct tm* tmPtr = localtime ( &rawTime );

    timeBuf[0] = tmPtr->tm_year + 1900;
    timeBuf[1] = tmPtr->tm_mon +1;
    timeBuf[2] = tmPtr->tm_mday;
    timeBuf[3] = tmPtr->tm_hour;
    timeBuf[4] = tmPtr->tm_min;
    timeBuf[5] = tmPtr->tm_sec;


    set_ds_value_to_modbus ( serialPortHandle,timeBuf );

    return 0;
}



/* */
static int32_t deal_with_yt_info ( int32_t serialPortHandle,
                                   const FifoHeader* headerInfo,
                                   const YT_FIFO_DATA_T* p_YtDataReadFromPipe )
{
    int16_t value = 0;
    uint16_t dataID = p_YtDataReadFromPipe->dataID;
    uint32_t devNo = headerInfo->devNo;

    if ( TYPE_FLOAT32 == p_YtDataReadFromPipe->val_type )
    {
        SHOW_DEBUG_INFO ( "dev(%d)'s 'YT' %d value is float %0.3f.\n",devNo,dataID,
                          ( float ) p_YtDataReadFromPipe->val.value_f );
    }
    else if ( TYPE_UINT32 == p_YtDataReadFromPipe->val_type )
    {
        SHOW_DEBUG_INFO ( "dev(%d)'s 'YT' %d value is uint32 %d.\n",devNo,dataID,
                          ( uint32_t ) p_YtDataReadFromPipe->val.value_32 );
    }
    else
    {
        WRITE_TO_LOG ( "ERROR: dev(%d)'s 'YT' data type(%d) is WRONG @ %s.\n",devNo,
                       p_YtDataReadFromPipe->val_type, get_current_time_str() );
    }

    if ( 3256 == dataID ) //开关机
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3256,  value, devNo );
    }
    else   if ( 3257 == dataID ) //并离网模式设置
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3257,  value, devNo );
    }
    else   if ( 3258 == dataID ) //无功调节选择开关
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3258,  value, devNo );
    }
    else   if ( 3259 == dataID ) //远程/本地
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3259,  value, devNo );
    }
    else   if ( 3260 == dataID ) //有功/无功优先
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3260,  value, devNo );
    }
    else   if ( 3261 == dataID ) //功率缓启使能选择
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3261,  value, devNo );
    }

    else   if ( 3300 == dataID ) //并网充放电模式
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3300,  value, devNo );
    }
    else   if ( 3301 == dataID ) //并网恒流电流值
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3300,  value, devNo );
    }
    else   if ( 3302 == dataID ) //并网恒压电压值
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3302,  value, devNo );
    }
    else   if ( 3303 == dataID ) //并网恒压限制电流值
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3303,  value, devNo );
    }
    else   if ( 3305 == dataID ) //并网恒功率值(DC)
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3305,  value, devNo );
    }
    else   if ( 3306 == dataID ) //并网恒功率值(DC)
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3306,  value, devNo );
    }
    else   if ( 3307 == dataID ) //并网恒功率值(DC)
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 100 );
        set_a_value_to_modbus ( serialPortHandle , 3307,  value, devNo );
    }
    else   if ( 3310 == dataID ) //并网恒功率值(DC)
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3310,  value, devNo );
    }
    else   if ( 3311 == dataID ) //并网恒功率值(DC)
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3311,  value, devNo );
    }
    else   if ( 3312 == dataID ) //并网充放电模式
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3312,  value, devNo );
    }
    else   if ( 3313 == dataID ) //并网充放电模式
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3313,  value, devNo );
    }
    else   if ( 3314 == dataID ) //并网充放电模式
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3314,  value, devNo );
    }
    else   if ( 3315 == dataID ) //并网恒功率值(DC)
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3315,  value, devNo );
    }
    else   if ( 3315 == dataID ) //并网恒功率值(DC)
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3315,  value, devNo );
    }
    else   if ( 3320 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 1000 );
        set_a_value_to_modbus ( serialPortHandle , 3320,  value, devNo );
    }
    else   if ( 3330 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3330,  value, devNo );
    }
    else   if ( 3331 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 100 );
        set_a_value_to_modbus ( serialPortHandle , 3331,  value, devNo );
    }
    else   if ( 3332 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3332,  value, devNo );
    }
    else   if ( 3333 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3333,  value, devNo );
    }
    else   if ( 3334 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3334,  value, devNo );
    }
    else   if ( 3335 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3335,  value, devNo );
    }
    else   if ( 3387 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3387,  value, devNo );
    }
    else   if ( 3388 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3388,  value, devNo );
    }
    else   if ( 3390 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3390,  value, devNo );
    }
    else   if ( 3391== dataID )  //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3391,  value, devNo );
    }
    else   if ( 3392 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3392,  value, devNo );
    }
    else   if ( 3393 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3393,  value, devNo );
    }
    else   if ( 3394 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3394,  value, devNo );
    }
    else   if ( 3395 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3395,  value, devNo );
    }
    else   if ( 3396 == dataID ) //并网充放电模式
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3396,  value, devNo );
    }
    else   if ( 3397 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3397,  value, devNo );
    }
    else   if ( 3398 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3398,  value, devNo );
    }
    else   if ( 3399 == dataID ) //
    {
        value = ( int16_t ) ( ( float ) p_YtDataReadFromPipe->val.value_f * 10 );
        set_a_value_to_modbus ( serialPortHandle , 3399,  value, devNo );
    }
    else   if ( 3404 == dataID ) //并网充放电模式
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3404,  value, devNo );
    }
    else   if ( 3405 == dataID ) //并网充放电模式
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3405,  value, devNo );
    }
    else   if ( 3411 == dataID ) //并网充放电模式
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3411,  value, devNo );
    }
    else   if ( 3412 == dataID ) //并网充放电模式
    {
        value = ( int16_t ) ( p_YtDataReadFromPipe->val.value_32 );
        set_a_value_to_modbus ( serialPortHandle , 3412,  value, devNo );
    }
    else
    {
        WRITE_TO_LOG ( "ERROR: dev(%d)'s 'YT' dataID(%04d) is out of range @ %s.\n",devNo,
                       p_YtDataReadFromPipe->dataID, get_current_time_str() );
    }

    return 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
static int32_t get_yk_yt_data_and_send_to_modbus ( int32_t toModbusPipeHandle,
        int32_t serialPortHandle )
{
    int32_t ret = 0;
    static FifoHeader CurrentHeaderFromPipe;
    static uint32_t headerInfoErrorCount = 0;
    int32_t readTimes = 0;
    uint32_t devNo = 0;

    for ( readTimes=0; readTimes<MAX_DEV_ADDR; readTimes++ ) //
    {
        ret = read ( toModbusPipeHandle, g_pipeReadBuf, sizeof ( FifoHeader ) );

        if ( sizeof ( FifoHeader ) == ret )
        {
            ret = check_and_get_header_info ( &CurrentHeaderFromPipe,
                                              ( const FifoHeader* ) g_pipeReadBuf );
            devNo = ( ( const FifoHeader* ) g_pipeReadBuf )->devNo;

            if ( 0 == ret ) //
            {
                ret = read ( toModbusPipeHandle, g_pipeReadBuf,
                             CurrentHeaderFromPipe.bodySize );

                if ( 0 == strcmp ( &CurrentHeaderFromPipe.dataType[0],"yk" ) )
                {
                    deal_with_yk_info ( serialPortHandle, &CurrentHeaderFromPipe, ( YK_FIFO_DATA_T* ) g_pipeReadBuf );
                }
                else if ( 0 == strcmp ( &CurrentHeaderFromPipe.dataType[0],"yt" ) )
                {
                    deal_with_yt_info ( serialPortHandle, &CurrentHeaderFromPipe,
                                        ( YT_FIFO_DATA_T* ) g_pipeReadBuf );
                }
                else if ( 0 == strcmp ( &CurrentHeaderFromPipe.dataType[0],"ds" ) )
                {
                    deal_with_ds_info ( serialPortHandle );
                }
                else
                {
                    WRITE_TO_LOG ( "ERROR: Neither 'YK' nor 'YT' data! @ dev(%d) @ %s\n",devNo,
                                   get_current_time_str() );
                }
            }
            else//
            {
                headerInfoErrorCount++;
                WRITE_TO_LOG ( "ERROR: Header info ERROR(%d times)! @ dev(%d) @ %s\n",devNo,
                               headerInfoErrorCount,get_current_time_str() );
            }
        }
        else//
        {
            break;
        }
    }

    return 0;
}


static int32_t send_dev_lost_info_to_61850_by_pipe ( uint32_t devNo,
        uint16_t stateValue,
        const char* to61850PipeName,
        int32_t to61850PipeHandle )
{
    YX_FIFO_DATA_T* dataPointer = g_yxPacketSendByPipe.data;

    g_yxPacketSendByPipe.header.dataType[0] = 't';//
    g_yxPacketSendByPipe.header.dataType[1] = 'x';
    g_yxPacketSendByPipe.header.dataType[2] = '\0';
    g_yxPacketSendByPipe.header.devType = DEV_TYPE;//30K
    g_yxPacketSendByPipe.header.devNo = devNo;
    g_yxPacketSendByPipe.header.resv = 0;

    dataPointer[0].dataID = 193;      //last bit of yx
    dataPointer[0].val    = stateValue;

    g_yxPacketSendByPipe.header.dataNum  = 1;
    g_yxPacketSendByPipe.header.bodySize = g_yxPacketSendByPipe.header.dataNum * sizeof ( YX_FIFO_DATA_T );

    int32_t totalSize = g_yxPacketSendByPipe.header.bodySize + sizeof ( FifoHeader );

    int32_t ret = write ( to61850PipeHandle, &g_yxPacketSendByPipe, totalSize );
    if ( totalSize != ret )
    {
        WRITE_TO_LOG ( "ERROR: Failed to write %d bytes yx to pipe<%s> @ ret=%d @ %s\n",\
                       totalSize, to61850PipeName, ret, get_current_time_str() );
    }

    return 0;
}

/*

*/
static void deal_with_if_dev_is_lost ( uint8_t devNo,
                                       const char* to61850PipeName,
                                       int32_t to61850PipeHandle )
{
    static uint8_t lastDevLostFlag[MAX_DEV_ADDR] = {0};//

#if 0
    printf ( "g_modbusLostPktsCounter[ARRAY_ID(devNo %d )]  %d \n",devNo,g_modbusLostPktsCounter[ARRAY_ID ( devNo )] );
#endif

    if ( g_modbusLostPktsCounter[ARRAY_ID ( devNo )] >= MODBUS_LOST_PKTS_THRESHOLD )
    {
        g_isDevLost[ARRAY_ID ( devNo )] = DEV_IS_LOST;
    }
    else
    {
        g_isDevLost[ARRAY_ID ( devNo )] = DEV_IS_ALIVE;
    }

    if ( lastDevLostFlag[ARRAY_ID ( devNo )] != g_isDevLost[ARRAY_ID ( devNo )] )
    {
        send_dev_lost_info_to_61850_by_pipe ( devNo, ( DEV_IS_LOST==g_isDevLost[ARRAY_ID ( devNo )]?1:0 ),
                                              to61850PipeName,to61850PipeHandle );

        lastDevLostFlag[ARRAY_ID ( devNo )] = g_isDevLost[ARRAY_ID ( devNo )];

#if DEBUG_OPEN
        printf ( " send_dev_lost_info_to_61850_by_pipe  \n" );
#endif


        SHOW_DEBUG_INFO ( "dev(%d) is %s.\n",devNo, ( DEV_IS_LOST==g_isDevLost[ARRAY_ID ( devNo )]? "lost":"alive" ) );
    }
}

/*
»ù±¾Ë³Ðò:
1, ½«±¾½ø³ÌÇÐ»»ÎªºóÌ¨½ø³Ì,ÔÚºóÌ¨ÔËÐÐ;
2, ´ò¿ª´®¿Ú;
3, ÒÔ¶ÁÄ£Ê½´´½¨Ò»¸ö¹ÜµÀ,¹ÜµÀÒ»¶¨»á±»´´½¨³É¹¦;
4, ÒÔÐ´Ä£Ê½´´½¨¹ÜµÀ,º¯Êý»áÒ»Ö±µÈµ½ÁíÒ»¸ö½ø³ÌÒÔ¶ÁÄ£Ê½´´½¨ÁËÒ»¸öÍ¬ÃûµÄ¹ÜµÀºó²Å»á³É¹¦·µ»Ø;
5, ³õÊ¼»¯modbusÐ­ÒéµÄ¶ª°ü¼ÆÊýÆ÷Îª×î´óÖµ£¬ÒÔ±ãÉè±¸Æô¶¯Ê±¿ÉÒÔÖ±½Ó·¢ÏÖÄæ±äÆ÷ÊÇ·ñÄÜÁ¬½ÓÉÏ;
6, »ñÈ¡Ò£ÐÅÒ£²âÊý¾Ý²¢·¢ËÍµ½¹ÜµÀÖÐ,Êµ¼ÊÊÇ·¢ÏòÁË61850½ø³Ì;
7, »ñÈ¡61850½ø³Ì·¢¹ýÀ´µÄÒ£¿ØÒ£µ÷ÐÅÏ¢,²¢·¢ËÍµ½Äæ±äÆ÷ÖÐÈ¥;ÓÉÓÚ61850ÏÂÀ´µÄ¹¦ÂÊµ÷½ÚµÈ²ÎÊý¿ÉÄÜÊÇÊµ¼ÊÖµ,
   ÐèÒª×ª³ÉÏà¶ÔÖµ,ËùÒÔÐèÒªÉè±¸µÄ¶î¶¨¹¦ÂÊµÈÐÅÏ¢,¹Ê´Ëº¯ÊýÓ¦¸Ã·ÅÔÚ'»ñÈ¡Ò£ÐÅÒ£²âÊý¾Ý'µÄºóÃæ;
8, Èç¹ûÉèÖÃÁËÒ£µ÷ÐÅÏ¢,ÐèÒªÁ¢¼´»Ø¶Á,ËùÒÔ½ÓÏÂÀ´Á¢¼´¶ÁÈ¡Ò£µ÷ÐÅÏ¢²¢·¢Ïò61850½ø³Ì;
9, Ò£µ÷ÐÅÏ¢°üÀ¨ÁËQP QUÐÅÏ¢,ËùÒÔ½ô½Ó×ÅÐèÒª¶ÁÈ¡QP QUÐÅÏ¢²¢·¢Ïò61850½ø³Ì;
10,Í¨¹ýÃ¿Ì¨Éè±¸µÄ¶ª°ü¼ÆÊýÀ´ÅÐ¶ÏÉè±¸ÊÇ·ñÒÑ¾­Í¨Ñ¶ÖÐ¶Ï,Èç¹ûÊÇ,ÔòÍ¨Öª61850½ø³Ì;
11,ÅÐ¶Ïµ±Ç°±£´æµÄÉè±¸ÔËÐÐ´íÎóÐÅÏ¢ºÍµ÷ÊÔÐÅÏ¢ÊÇ·ñÒÑ¾­´ïµ½ÈÝÁ¿µÄÉÏÏÞ,ÊÇÔò±¸·Ýµ±Ç°µÄ,ÖØ½¨ÐÂµÄ;
12,¸ù¾ÝÊµ¼ÊÉè±¸µÄÊýÁ¿,ÐÞ¸Äµ±Ç°µÄÉè±¸±àºÅÒÔ´ïµ½±éÀúÉè±¸µÄÄ¿µÄ¡£
*/




int32_t main ( int argc, char* argv[] )
{

//打印程序版本号
    DEBUG_TIME_OUTPUT ( "\n -- init-- soft_ver:%s \n", soft_ver );

    if ( ( argc > 2 ) || ( argc < 2 ) )
    {
        printf ( " argc %d arguments  error right 2 !\n" ,argc );
        return -1;
    }
    else
    {
        if ( 0 == chdir ( argv[1] ) )
        {
            printf ( " change the work directory %s \n",argv[1] );
        }
        else
        {
            printf ( "Can not change the work directory.\n" );
            return 0;
        }
    }


    /*  */
    if ( 0 == daemon ( 1, 1 ) )
    {
        log_init();
        WRITE_TO_LOG ( "%s start @ %s.\n",__FILE__,get_current_time_str() );
    }
    else
    {
        perror ( "Make modbus daemon ERROR.\n" );
        exit ( EXIT_FAILURE );
    }


    int32_t serialPortHandle = open_and_set_a_serial_port ( "/dev/ttyO1" );

    const char* toModbusPipeName = "/tmp/fifo_ctrl"; //fifo_ctrl ykYtPipe
    int32_t toModbusPipeHandle = get_a_to_modbus_pipe_at_read_mode ( toModbusPipeName );

    const char* to61850PipeName = "/tmp/fifo_data"; //fifo_data yxYcPipe
    int32_t to61850PipeHandle = get_a_to61850_pipe_at_write_mode ( to61850PipeName );

    sw_watchdog_init();

    uint8_t devNo = 1;
    init_modbus_lost_pkts_counter();

    while ( 1 )
    {
        get_yx_yc_data_and_send_to_pipe ( devNo,serialPortHandle,to61850PipeHandle,to61850PipeName );

        get_yk_yt_data_and_send_to_modbus ( toModbusPipeHandle, serialPortHandle );

        get_yk_yt_data_and_send_to_pipe ( devNo,serialPortHandle,to61850PipeHandle,to61850PipeName );

        deal_with_if_dev_is_lost ( devNo, to61850PipeName,to61850PipeHandle );

        send_yk_to_pipe_while_dev_lost ( devNo,to61850PipeHandle,to61850PipeName );

        backup_log_if_need();

    }
}





