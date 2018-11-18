/*************************************************
FileName:     protocol_operate.h
Copyright (C), 2010-2015, SUNGROW POWER SUPPLY CO., LTD.
Link:         protocol.c
History:
**************************************************/
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/************************************************************************
*                  头文件+宏定义+静态局部定义
*************************************************************************/
#include "io.h"

#define PTL_NUM 10
#define MAXEVENTS 64
#define     MB_FUNC_MAX_NUM (int32_t)100

#define MAXDEVNUM  (int32_t)50

#define BUFLEN     (int32_t)1024*10+1

#define PROTOCOLNUM     (int32_t )50

// #define CLIENTID   (uint32_t)0x2  //当前进程客户端id

extern int32_t  modbus_client_id  ;  //当前进程客户端id


// 最大客户端链接数量
#define MAX_CLINET_LINK_NUM   (int32_t)10



#define  MODBUS_OPERATE_RIGHT        0x0          //正确操作返回  
#define  MODBUS_CMD_NO_SUPPORT   0x01          //不支持的命令  
#define  CMD_DATA_NUM_BEYOND   	  0x03          //寄存器数量越界   0x0001≤输出数量≤0x07D0   (2000)
#define  CMD_ADDRESS_BEYOND            0x02          //起始地址 != OK       起始地址+输出数量 !＝OK
#define  CMD_READ_WRITE_ERR               0x04          //读取离散输出!＝OK  设置失败

/************************************************************************
*                结构体声明
*************************************************************************/
typedef struct
{
    uint16_t id[256];
    uint16_t type[256];
    uint16_t ptr;
} pro_data_s;


//modbus slave 接收数据协议解析
typedef struct
{
    uint8_t id; //站号
    uint8_t cmd; //命令类型
    uint16_t address;//寄存器地址
    uint16_t len; //数据长度
    uint8_t *data; //数据指针
    uint8_t valid; //有效性
    uint8_t err;//错误码
} modbus_protocol;


#define CALL_FUN_MAX_LEN  64
//通信协议点表
typedef struct protocol_data_st
{

    char data_name[30];  //数据名称
    uint16_t data_id;  //数据id
// uint8_t data_unit;  //设备类型数据单元号  大功率存在  单元1  单元2  单元 3 。。。
    uint16_t data_address;//寄存器地址
    uint8_t data_byte_num;//数据类型
    char data_info[20];
    uint8_t data_type;//数据类型     i     F
    uint8_t data_sign;  // 数据正负  U   S
    float dec_p;//小数点位数
    float scale_coefficient;//比例系数
    int32_t low_limit1; //下限
    int32_t high_limit1;//上限

    int32_t low_limit2; //下限
    int32_t high_limit2;//上限


    uint32_t low_limit1_uint; // 无符号下限
    uint32_t high_limit1_uint;;//无符号上限

    uint32_t low_limit2_uint;; //无符号下限
    uint32_t high_limit2_uint;;//无符号上限



    char func_Rev[CALL_FUN_MAX_LEN];//数据处理函数
    char func_Send[CALL_FUN_MAX_LEN];//数据处理函数
    char func_Fifo[CALL_FUN_MAX_LEN];//数据处理函数

} protocol_data_s;

//网口接口属性
typedef struct
{
    char allow_ip[30];  //允许链接ip地址
    uint16_t netport;//端口号
    uint16_t allow_all_flag;  //允许链接所有ip地址请求标记
    uint16_t max_allow_link_num; //允许最大的链接数量
} net_port_attr;


//端口属性
typedef struct
{
    int32_t commtype;//端口类型
    int32_t commdef;  //LCD 设置标记  兼容大功率标记
    int32_t uartid;		//串口号
    int32_t uartbaudrate;  //波特率
    int32_t uartdatabit;//串口数据位
    char uartstopbit[4];  //停止位
    char uartparity;          //校验位

    uint16_t net_id;//网口号
    uint8_t net_port_num;//端口号
    net_port_attr port_attr[5];//端口号
} comport_s;


//点表属性
typedef struct
{

    uint16_t ptl_dev_code;//协议唯一编号 ,类型的设备编号
    uint32_t max_ptl_data_id; //点表最大数据id
    uint8_t cmd;		//寄存器类型
    uint16_t address;//起始地址
    uint16_t len;//长度
    protocol_data_s *protocol_data;//点表起始地址
    uint16_t ptl_data_num;//
    uint32_t *map;//点表映射表	协议寄存器地址-点表序号
    uint8_t protocol_byte;  //字节序
    uint8_t protocol_word;  // 字序

    int32_t ptl_min_addr;
    int32_t  ptl_max_addr;

} protocol_s;

typedef struct SLAVE_CORRESPOND_INDEX  //modbus 地址 对应包含的l类型设备序号
{
    int32_t modbus_id ;  //modbus地址
    int32_t slaveIndex ;   //对应设备索引

} slaver_corr_index;


typedef struct SLAVER_CONFIG_CONTENT
{
    char dev_type_name[30];  //中功率 或者 大功率设备总类型名称  中功率设备SG_STRING_INV
    char dev_name[30];   //具体设备名称
    uint8_t  slave_flag ;            //设备对应关系初始化 完成标志
    int32_t max_data_id ;         //同类设备中的最大设备id节点
    uint16_t dev_code; 		//设备类型
    int32_t slave_id_cor_num;   //同类型设备数量
    slaver_corr_index    *p_slaver_corr_index_arr;  //modbus 地址 对应包含的l类型设备序号

} slave_device_config_content; 		//设备配置对应内容


//端口链接对象结构体
typedef struct   client_link_slaver
{
    int32_t comm_type;  //设备链接端口类型
    int32_t link_fd;	           //文件句柄标
    int32_t comm_conn_id;  //端口绑定 id

    uint32_t retnum;			       //处理结果数据个数
    uint8_t retbuf[BUFLEN];		//处理结果数据个数

    struct client_link_slaver *next;         //单链表指针

} client_link_slaver;

typedef int32_t ( *CommObjFunc ) ( struct client_link_slaver *, uint8_t *, uint16_t ,uint8_t * )  ;


//客户端对象
typedef struct CommObj_st
{
    int32_t  comm_conn_obj_id;         //端口物理接口对象id

    int32_t protocolNum;      		//协议个数

    protocol_s *p_comm_ptl[PROTOCOLNUM];       //通信协议指针

} CommObj_s;



//端口链接对象结构体
typedef struct CommConn_Obj
{

    uint16_t one_reg_dev_code;     //设备类型  特殊处理  当前只支持一种设备  2018-11-17 
    
    CommObj_s  comm_protocol_gather;  //设备类型对象协议合集



    int32_t mbm_slaver_num ;    //端口链接对象下的slaver 的设备总数

    slave_device_config_content   *p_mbm_slaver_config[10];  //最多下挂十个设备

    int32_t comm_obj_fd;	                //端口链接文件句柄标识    此处用于串口标识 网口存在多客户端情况

    int32_t cmm_case_id;                    //管道

    int32_t cmm_type ;    	                 //端口类型  com  net    (lcd   com_type -1 )

    CommObjFunc func;       		 //函数回调

    struct CommConnObj *next;         //单链表指针

} CommConnObj;



//数据处理客户端结构体
typedef struct
{
    client_link_slaver *comm_L;    //客户端数据处理链表头
    client_link_slaver *end;             //客户端数据处理链表尾
    int32_t num;				//客户端数量
} comm_reg_s;





//设备全局数据属性合集
typedef struct dev_gather_data
{

    uint16_t dev_code;   		//设备编号
    int32_t dev_max_data_id; 	//同种设备中最大的数据序号
    int32_t same_dev_num;  	//同类型设备数量
    data_u **data_p ;   		//全局数据集合
    uint8_t **func_id_p;  		//回调函数
    int32_t *dev_communicate_state;  //设备通讯状态

} dev_gather_data;



//搜索设备全局数据属性
typedef struct search_dev_gather
{
    int32_t dev_type_num ;  //设备数量
    uint32_t *search_map;  //设备号映射
    dev_gather_data *p_dev_gdata ;  //设备全集数据
} search_dev_gather;

extern search_dev_gather search_dev_gather_case;




/************************************************************************
*                  全局函数声明
*************************************************************************/
uint8_t crccheck ( uint8_t *pbuf, uint16_t buflen, uint8_t crctype );

int32_t protocol_init ( void );
int32_t modbus_rtu_slave ( client_link_slaver *comm, uint8_t *inbuf, uint16_t inlen,uint8_t *sendbuf ); //,CommObj_s *retcomm);
int32_t modbus_tcp_slave ( client_link_slaver *comm, uint8_t *inbuf, uint16_t inlen,uint8_t *sendbuf );
int32_t fifo_fill ( client_link_slaver *comm, uint8_t *inbuf, uint16_t inlen,uint8_t *sendbuf );
int32_t fifo_tran_fill ( client_link_slaver *comm, uint8_t *inbuf, uint16_t inlen,uint8_t *sendbuf );

int32_t create_comm ( void );
int32_t insert_comm ( int32_t fd,int32_t comm_conn_id );
int32_t delete_comm ( int32_t del_fd );


// int32_t PtAdjust ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t receive_TimeTick ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
// int32_t UartSet ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid,uint16_t search_id );
int32_t receive_NetSet ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid,uint16_t search_id );
int32_t receive_Net_IP_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid,uint16_t search_id );
int32_t receive_Net_Mask_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid,uint16_t search_id );
int32_t receive_Net_Getway_Set ( uint16_t name,data_u old_data,pro_data_s *databuf ,int32_t indexid,uint16_t search_id );
int32_t receive_Net_DNS1_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
int32_t receive_Net_DNS2_Set ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );
// int32_t errorStateSet ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid,uint16_t search_id );


int32_t     send_time_year ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_mon ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_day ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_hour ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_min ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_second ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );

int32_t	send_time_lcd_year ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_lcd_mon ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_lcd_day ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_lcd_hour ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_lcd_min ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	send_time_lcd_second ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );





int32_t     receive_time_year ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_mon ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_day ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_hour ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_min ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_second ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );

int32_t	receive_time_lcd_year ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_mon ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_day ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_hour ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_min ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );
int32_t	receive_time_lcd_second ( uint16_t name,data_u old_data,pro_data_s *databuf,int32_t indexid ,uint16_t search_id );



int32_t delete_comm ( int32_t del_fd );
/************************************************************************
*                  全局变量外部引用
*************************************************************************/

extern int32_t net_ip_reset_flag;                             //网口重新设置

extern uint8_t fifo_get_time_flag ;                         //管道获取时间标记  （说明有其他 的后台 eg 104 发送时间给数据处理进程，数据处理进程转发过来）



//枚举名称和函数数组下标对应
extern int32_t ( *comm_func[MB_FUNC_MAX_NUM] ) ( uint16_t name,data_u old_data,pro_data_s *,int32_t indexid,uint16_t search_id );


#define  DEV_COMM_FAULT    (int32_t)1
#define  DEV_COMM_NORMAL    (int32_t)0



#endif



