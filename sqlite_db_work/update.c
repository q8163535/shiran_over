/****************************************************************************** 
* Copyright (C), 1997-2016, SUNGROW POWER SUPPLY CO., LTD. 
* File name      :update.c
* Author         :luoj
* Date           :2016-9-1
* Description    :固件升级文件
* Others         :none
******************************************************************************/ 
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <errno.h>  
#include <netdb.h>  
#include <fcntl.h>   
#include <pthread.h>
#include <sys/time.h> 
#include <semaphore.h>
#include <sys/ioctl.h>
#include <math.h>    
#include <sqlite3.h> 
#include <sys/file.h>
#include "../inc/protocol.h"
#include "../inc/protocol_cfg.h"
#include "../inc/Script.h"
#include "../inc/db.h"
#include "../inc/init.h"
#include "../inc/can.h"
#include "../inc/debug.h"
#include "../inc/uart.h"
#include "../inc/update.h"
#include "../inc/aes.h"

sgu_module_s sgu_module;

sqlite3 *db_update;


//static int recover_baud(uint8_t index, update_module_s *update);

static const uint32_t g_crc32Table[256] = {
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};


uint16_t get_u16(uint8_t *val)
{
  uint16_t tmp16 ;
  tmp16 = ((uint16_t)val[0]<< 8) | val[1];
  return tmp16;
}

uint32_t get_u32(uint8_t *val)
{
  uint32_t tmp32;
  tmp32 = ((uint32_t)val[0] << 8) | val[1] | ((uint32_t)val[2] << 24) | ((uint32_t)val[3] << 16);
  return tmp32;
}  

/****************************************************************************** 
* Function       :crc32
* Author         :luoj
* Date           :2016-9-1
* Description    :crc校验，8位
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/ 
uint32_t crc32( uint8_t *buf, uint32_t len, uint32_t crcSeed)
{
    uint32_t ret = crcSeed; //first crcSeed is 0xFFFFFFFF;
    int i;

    for(i = 0; i < len;i++)
    {
    ret = (ret >> 8) ^ g_crc32Table[(ret ^ buf[i]) & 0xFF];
    }
    return ret;
}

/****************************************************************************** 
* Function       :CRC32_32
* Author         :luoj
* Date           :2016-9-1
* Description    :32位crc
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static uint32_t CRC32_32(uint32_t  *InputData, uint32_t len)
{
    uint32_t dwPolynomial = 0x04c11db7;
    uint32_t i;
    uint32_t xbit;
    uint32_t data;
    uint32_t crc_cnt = 0xFFFFFFFF; 
    int bits;
    for (i = 0; i < len; i++)
    {
        // xbit = 1 << 31;
        xbit = 0x80000000;
        data = InputData[i];
        for (bits = 0; bits < 32; bits++)
        {
            if ((crc_cnt & 0x80000000) > 0)
            {
                crc_cnt <<= 1;
                crc_cnt ^= dwPolynomial;
            }
            else
                crc_cnt <<= 1;
            if ((data & xbit) > 0)
                crc_cnt ^= dwPolynomial;
            xbit >>= 1;
        }
    }
    return crc_cnt;
}

/*
static unsigned long CalStmCrc32(unsigned long Crc32In,unsigned int *ptr, unsigned long size)
{
  unsigned long    xbit;
  unsigned long    dataH;
  unsigned long    dataL;
  unsigned long    data;
  unsigned long    len ;
  unsigned long    CRC = Crc32In;
  unsigned int     bits;
  unsigned long    dwPolynomial = 0x04c11db7;

  if((size & 0x01)==0)
  {
    len = size >> 1;
  }
  else
  {
    len = ( size + 1) >> 1;
  }

  if(size>0)
  {
    while (len--)
    {
       xbit = (unsigned long)1 << (unsigned long)31;
     if((size & 0x01)==0)
     {
       dataH = (unsigned long)(*ptr);
       dataL = (unsigned long)(*(ptr+1));
     }
     else
     {
       if(len==1)
       {
         dataH = (unsigned long)(*ptr);
         dataL = 0;
       }
       else
       {
         dataH = (unsigned long)(*ptr);
         dataL = (unsigned long)(*(ptr+1));
       }
     }
     ptr = ptr + 2;
     data = ((dataH << 16) & 0xFFFF0000)|(dataL & 0x0000FFFF);
     data = (((data<<24) & 0xFF000000)|\
           ((data<<8) & 0x00FF0000)|\
         ((data>>8) & 0x0000FF00)|\
         ((data>>24) & 0x000000FF));
     for (bits = 0;bits <32; bits++)
     {
        if (CRC & 0x80000000)
        {
          CRC <<= 1;
          CRC ^= dwPolynomial;
        }
        else
        {
          CRC <<= 1;
        }
        if (data & xbit)
        {
          CRC ^= dwPolynomial;
        }
        xbit >>= 1;
      }
    }
  }
  return CRC;
} */

/****************************************************************************** 
* Function       :update_log_open
* Author         :luoj
* Date           :2016-9-1
* Description    :创建升级日志
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void update_log_open(update_module_s *update)
{
  char log_name[100] = {0};

  sprintf(log_name, "%scom%d_update_log.txt", UPDATE_FILE_PATH, update->update_com);
  
  /* 追加方式打开日志文件，且删除之前文件内容 */
  if(update->update_type >= MCU_VER1_TYPE && update->update_type <= MCU_VER8_TYPE)
  {
    update->log_fd = open(log_name, O_CREAT | O_RDWR | O_APPEND| O_TRUNC, 0666);
  }
  else if(update->update_type == IAP_TYPE)
  {
    update->log_fd = open(log_name, O_CREAT | O_RDWR | O_APPEND, 0666);
  }
  
  if(update->log_fd <= 0)
  {
    printf("(com%d)open update log error!\n", update->update_com);
    exit(EXIT_FAILURE);
  }
}


/****************************************************************************** 
* Function       ://update_log_write
* Author         :luoj
* Date           :2016-9-1
* Description    :写入升级日志
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void update_log_write(int32_t fd,char *buf)
{  
  int len = write(fd, buf, strlen(buf));
  if(len <= 0)
  {
    printf("log write error !\n");
  }
}



void update_log_close(int32_t fd)
{
  close(fd);
  system("sync");
}


/****************************************************************************** 
* Function       :db_creat_table_update_param
* Author         :luoj
* Date           :2016-9-1
* Description    :创建升级参数表
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void db_creat_table_update_param(void)
{  
  char *sql = "CREATE TABLE IF NOT EXISTS UPDATEPARAM("  \
              "ID integer PRIMARY KEY autoincrement ," \
              "DATATIME       TEXT," \
              "COM            TEXT UNIQUE,"\
              "START_FLAG     BLOB," \
              "FINISH_FLAG    BLOB," \
              "UPDATE_TYPE    INT,"  \
              "TOT_NUM        INT,"  \
              "FILE_VER       TEXT," \
              "ADDR           TEXT," \
              "SGU_STATE      INT);";

  db_operate(UPDATE_DB,sql,db_update);
}

/****************************************************************************** 
* Function       :db_creat_table_update_info
* Author         :luoj
* Date           :2016-9-1
* Description    :创建升级信息表
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void db_creat_table_update_info(void)
{ 
  char *sql = "CREATE TABLE IF NOT EXISTS UPDATEINFO("  \
              "ID integer PRIMARY KEY autoincrement ," \
              "DATATIME       TEXT," \
              "POSITION       TEXT," \
              "COMM_STATE     BLOB," \
              "NAME           TEXT," \
              "ADDR           INT," \
              "COM            TEXT,"  \
              "CURVER         TEXT,"\
              "GOALVER        TEXT,"\
              "PROCESS        REAL,"\
              "STATE          INT," \
              "ISUP           INT," \
              "SN             TEXT," \
              "RESULT         TEXT);";

  db_operate(UPDATE_DB,sql,db_update);
}

/****************************************************************************** 
* Function       :db_creat_table_web_savefile
* Author         :luoj
* Date           :2016-9-1
* Description    :web使用
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void db_creat_table_web_savefile(void)
{ 
  char *sql = "CREATE TABLE IF NOT EXISTS SAVEFILE ("  \
              "SGUNAME        TEXT);";

  db_operate(UPDATE_DB,sql,db_update);
}


/****************************************************************************** 
* Function       :clear_update_db
* Author         :luoj
* Date           :2016-9-1
* Description    :每次进程重启删除表
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void clear_update_db(void)
{
  char strSQL1[256] = {0};

  // memset(strSQL1, 0 , 256);
  // sprintf(strSQL1, "DROP TABLE IF EXISTS UPDATEINFO;");
  // db_operate(UPDATE_DB,strSQL1,db_update);

  memset(strSQL1, 0 , 256);
  sprintf(strSQL1, "DROP TABLE IF EXISTS UPDATEPARAM;");
  db_operate(UPDATE_DB,strSQL1,db_update);

  memset(strSQL1, 0 , 256);
  sprintf(strSQL1, "DROP TABLE IF EXISTS SAVEFILE;");
  db_operate(UPDATE_DB,strSQL1,db_update);

}


/****************************************************************************** 
* Function       :sgu_init
* Author         :luoj
* Date           :2016-9-1
* Description    :sgu解析模块初始化
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
void sgu_init(void)
{
  int res =0;
  sgu_module.sgu_state = SGU_NONE;
  memset(sgu_module.file_ver, 0 , 30);
  memset(sgu_module.buf8, 0, BUF8_LEN);
  memset(sgu_module.buf32, 0, BUF32_LEN);

  sgu_module.tot_crc32 = 0xFFFFFFFF;
  sgu_module.tot_len = 0;
  sgu_module.tot_pkg_num = 0;
  sgu_module.pkg_len = 0;

  res = sem_init(&(sgu_module.sgu_sem), 0, 0);
  if (res != 0) 
  {
    perror("Semaphore sgu_seminitialization failed");
    exit(EXIT_FAILURE);
  }
}

void clear_sgu_memory(void)
{
  sgu_module.sgu_state = 0;
  memset(sgu_module.file_ver, 0 , 30);
  memset(sgu_module.buf8, 0, BUF8_LEN);
  memset(sgu_module.buf32, 0, BUF32_LEN);

  sgu_module.tot_crc32 = 0xFFFFFFFF;
  sgu_module.tot_len = 0;
  sgu_module.tot_pkg_num = 0;
  sgu_module.pkg_len = 0;
}

/****************************************************************************** 
* Function       :update_init
* Author         :luoj
* Date           :2016-9-1
* Description    :升级模块初始化
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
void update_init(void)
{
  int i = 0;
  mb_master_s *mb_master = mb_master_tab.head;

  for (i = 0; i < mb_master_tab.num; ++i)
  {
    mb_master->update = (update_module_s *)malloc(sizeof(update_module_s));
    mb_master->update->start_flag = 0;
    mb_master->update->finish_flag = 0;
    mb_master->update->update_com = mb_master->comport.uartid;
    mb_master->update->fd = mb_master->fd;
    mb_master->update->com_type = mb_master->comport.commtype;
    mb_master->update->update_type = 0;
    mb_master->update->tot_num = 0;
    memset(mb_master->update->file_ver, 0, 30);

    mb_master->update->begin_pkg_cnt = 1;
    mb_master->update->iapcmd = 0;
    mb_master->update->tot_crc32 = 0xFFFFFFFF;
    mb_master->update->tot_len = 0;
    mb_master->update->tot_pkg_num = 0;
    mb_master->update->pkg_len = 0;
    mb_master->update->pkg_cnt = 0;
    mb_master->update->already_pkg_cnt = 0;
    mb_master->update->dev_pkg_len = 0;
    memset(mb_master->update->data, 0, 208);

    mb_master->update->log_fd = 0;
    mb_master->update->success_num = 0;

    mb_master->update->src_baud = mb_master->comport.uartbaudrate;
    mb_master->update->adjust_flag = 0;
    mb_master->update->adjust_dirct = ADJUST_LOW_TO_HIGH;
    mb_master->update->plc_enable = mb_master->plc_enable;
    mb_master->update->start_t = 0;
    mb_master->update->end_t = 0;
    mb_master->update->mcu_ver_tot_num = 0;
    mb_master->update->mcu_ver_db_num = 0;
    mb_master->update->mcu_ver_finish_flag = 0;
    mb_master->update->aes_flag = 0;
    
    mb_master->update->iap = (iap_module_s *)malloc(sizeof(iap_module_s) *MAXSLAVERDEVS);
    memset(mb_master->update->iap, 0, sizeof(iap_module_s) * MAXSLAVERDEVS);
    int j=0;
    for(j=0; j<MAXSLAVERDEVS; j++)
    {
      mb_master->update->iap[j].log_ovr = 8;   //超时时间8分钟
      mb_master->update->iap[j].auto_update = 0x55;
      mb_master->update->iap[j].state = 0;
      mb_master->update->iap[j].process = 0.0f;
    }

    mb_master = mb_master->next;
  }

  //每次进程重启，先删除表后创建表，为web操作
  clear_update_db();

  //初始化升级数据库表
  db_creat_table_update_param();
  db_creat_table_update_info(); 
  db_creat_table_web_savefile(); 
}


/****************************************************************************** 
* Function       :clear_update_memory
* Author         :luoj
* Date           :2016-9-1
* Description    :清空升级时内存
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void clear_update_memory(mb_master_s *mb_master)
{
  int j = 0;
  update_module_s *update = mb_master->update;

  update->start_flag = 0;
  update->finish_flag = 0;
  update->update_com = mb_master->comport.uartid;
  update->fd = mb_master->fd;
  update->com_type = mb_master->comport.commtype;
  update->update_type = 0;
  update->tot_num = 0;
  // memset(update->file_ver, 0, 30);

  update->begin_pkg_cnt = 1;
  update->iapcmd = 0;
  update->tot_crc32 = 0xFFFFFFFF;
  update->tot_len = 0;
  update->tot_pkg_num = 0;
  update->pkg_len = 0;
  update->pkg_cnt = 0;
  update->already_pkg_cnt = 0;
  update->dev_pkg_len = 0;
  memset(update->data, 0, 208);

  update->log_fd = 0;
  update->success_num = 0;

  update->src_baud = mb_master->comport.uartbaudrate;
  update->adjust_flag = 0;
  update->adjust_dirct = ADJUST_LOW_TO_HIGH;
  update->plc_enable = mb_master->plc_enable;
  update->start_t = 0;
  update->end_t = 0;
  update->mcu_ver_tot_num = 0;
  update->mcu_ver_db_num = 0;
  update->mcu_ver_finish_flag = 0;
  update->aes_flag = 0;
  
  memset(update->iap, 0, sizeof(iap_module_s) * MAXSLAVERDEVS);
  for(j=0; j<MAXSLAVERDEVS; j++)
  {
    update->iap[j].log_ovr = 5;   //超时时间5分钟
    update->iap[j].auto_update = 0x55;
    update->iap[j].state = 0;
    update->iap[j].process = 0.0f;
  }
}

/****************************************************************************** 
* Function       :db_update_param_callback
* Author         :luoj
* Date           :2016-8-1
* Description    :升级参数表数据库回调函数，select读取时会调用该函数，获取数据库行信息,已有端口启动的情况下
* Calls          :none
* Table Accessed :db_update_param 
* Table Updated  :none 
* Input          :*data ：传入参数
                  argc： 数据库表格列数
                  argv : 列数据
                  azColName ：列名称
* Output         :int 
* Return         :none
******************************************************************************/
static int db_update_param_callback(void *data, int argc, char **argv, char **azColName)
{
  uint8_t *err_flag = (uint8_t *)data;
  uint8_t i=0;
  uint8_t temp_type = 0;

  for(i=0; i< argc;i++)
  {
    printf("%s = %s\n", azColName[i], argv[i]);
  }

  temp_type = atoi(argv[5]);
  if(temp_type > MAX_UPDATE_TYPE)
  {
    printf("升级类型错误\n");
    err_flag[0] = 1;
    return 0;
  }

  if(temp_type == SGU_TYPE)   //sgu解析
  {
    if(argv[7] == NULL)
    {
      err_flag[0] = 1;
      return 0;
    }
    else
    {
      strcpy(sgu_module.file_ver, argv[7]);
      sem_post(&sgu_module.sgu_sem);
    }
  }
  else   //升级或者读取版本号
  {
    uint8_t update_com;
    mb_master_s *mb_master = mb_master_tab.head;

    update_com = atoi(&argv[2][3]);
    if(update_com < 1 || update_com > MAX_COM_NUM)
    {
      printf("端口非法范围! update_com = %d\n", update_com);
      err_flag[0] = 1;    //err_flag[0] 用于存放端口错误的标志
      return 0;
    }

    for (i = 0; i < mb_master_tab.num; i++)
    {
      if(mb_master->comport.uartid == update_com)
      {
        mb_master->update->update_com = update_com;
        break;
      }
      mb_master = mb_master->next;
    }
    
    if(i == mb_master_tab.num)
    {
      printf("未找到主站端口! update_com = %d\n", update_com);
      err_flag[update_com] = 1;
      return 0;
    }

    //xml没有配置设备
    if(mb_master->num < 1)
    {
      printf("xml没有配置设备(%d)，退出升级 !\n", mb_master->num);
      err_flag[update_com] = 1;
      return 0;
    }

    update_module_s *update = mb_master->update;

    pthread_mutex_lock(&(mb_master->update_mutex));
    if(update->start_flag == 1)
    {
      printf("(com%d) 已经在升级过程中，退出......\n", update->update_com);
      return 0;
    }
    pthread_mutex_unlock(&(mb_master->update_mutex));

    update->update_type = atoi(argv[5]);
    if(update->update_type >= MAX_UPDATE_TYPE)
    {
      printf("(com%d) 升级类型错误\n", update_com);
      err_flag[update_com] = 1;
      return 0;
    } 

    update->tot_num = atoi(argv[6]);
    if(update->tot_num > MAXSLAVERDEVS || update->tot_num == 0)
    {
      printf("(com%d) 升级总台数(%d)异常 \n", update_com, update->tot_num);
      err_flag[update_com] = 1;
      return 0;
    }

    if(argv[7] == NULL)
    {
      err_flag[update_com] = 1;
      return 0;
    }
    else
    {
      strcpy(update->file_ver, argv[7]);
    }

    //从地址串中获取地址
    char addr_str[256] = {0};
    char element[256] = {0};

    if(argv[8] == NULL)
    {
      if(update->update_type !=  SGU_TYPE)    //sgu解析不需要设置设备地址
      {
        printf("web未设置地址！\n");
        err_flag[update_com] = 1;
        return 0;
      }
     
    }
    else
    {
      strcpy(addr_str, argv[8]);
      uint16_t line_len = 0;
      for(i=0; i< update->tot_num;i++)
      {
        line_len += get_element_from_str(256, &addr_str[line_len],element,',');
        uint8_t temp_addr = strtoul(element, NULL, 0);
        if(temp_addr >= 1 && temp_addr <= 247)
        {
          update->iap[i].addr = temp_addr;
        }
      }

    }
    
    pthread_mutex_lock(&(mb_master->update_mutex));
    update->start_flag  = atoi(argv[3]);
    update->finish_flag = atoi(argv[4]);
    pthread_mutex_unlock(&(mb_master->update_mutex));

    sem_post(&(mb_master->update_sem));
  }

  return 0;
}

void db_insert_update_param_test(void)
{
  int start_f = 1;
  int end_f = 0;
  int up_type = IAP_TYPE;
  int tot_num = 1; //IAP_TYPE MCU_VER2_TYPE
  char com[] = "COM3";
  char file_ver[30] = "MDSP_SG60KTL_V11_V1_J";  //"LCD_SG60KTL_V11_V03_I_M";  MDSP_SG60KTL_V11_V1_J
  char addr_str[256]="1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25";
  int sgu_state =0;
  struct tm* local;
  time_t t;
  char strtmp[256] = {0};
  char strSQL1[256] = {0};

  t = time(NULL);
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  local = localtime(&t);


/*
  static uint8_t test_cnt =2;
  if(test_cnt == 0)
  {
    up_type = SGU_TYPE;
    test_cnt = 1;
  }
  else if(test_cnt == 1)
  {
    sleep(3);
    up_type = MCU_VER2_TYPE;
    test_cnt =2;  
  }
  else if(test_cnt == 2)
  {
    sleep(5);
    up_type = IAP_TYPE;
    test_cnt =3;
  }
  else if(test_cnt == 3)
  {
    return;
  }
  */

  
  db_open(UPDATE_DB,&db_update);

  memset(strSQL1,0,256);
  sprintf(strSQL1,"SELECT count(*) as rows FROM UPDATEPARAM WHERE COM = '%s';", com);
  db_done(strSQL1, db_update,db_sel_callback,strtmp);

  if(atoi(strtmp) > 0)
  {
    memset(strSQL1,0,256);
    sprintf(strSQL1,"DELETE from UPDATEPARAM where COM = '%s';", com); 
    db_done(strSQL1, db_update,NULL, NULL);
  }
  
  memset(strSQL1,0,256);
  sprintf(strSQL1,"INSERT INTO UPDATEPARAM VALUES (NULL , '%04d-%02d-%02d %02d:%02d:%02d','%s',%d, %d, %d, %d, '%s' ,'%s', %d); ",\
                    (local->tm_year + 1900),
                    (local->tm_mon + 1),
                    local->tm_mday,
                    local->tm_hour,
                    local->tm_min,
                    local->tm_sec,
                    com,
                    start_f,
                    end_f, 
                    up_type,
                    tot_num,
                    file_ver,
                    addr_str,
                    sgu_state);
  db_done(strSQL1, db_update,NULL,NULL);
  db_close(db_update);
  
  
  //SG50KTL
  /*
  strcpy(file_ver ,"LCD_SG50KTL-M_V11_V03_N_M");  //"LCD_SG50KTL-M_V11_V03_N_M");
  strcpy(com, "COM1");
  strcpy(addr_str, "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30"); //31,32,33,34,35,36,37,38,39,
  //40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64");

  tot_num = 30;
  
  db_open(UPDATE_DB,&db_update);
  memset(strSQL1,0,256);
  sprintf(strSQL1,"SELECT count(*) as rows FROM UPDATEPARAM WHERE COM = '%s';", com);
  db_done(strSQL1, db_update,db_sel_callback,strtmp);

  if(atoi(strtmp) > 0)
  {
    memset(strSQL1,0,256);
    sprintf(strSQL1,"DELETE from UPDATEPARAM where COM = '%s';", com); 
    db_done(strSQL1, db_update,NULL, NULL);
  }
  
  memset(strSQL1,0,256);
  sprintf(strSQL1,"INSERT INTO UPDATEPARAM VALUES (NULL , '%04d-%02d-%02d %02d:%02d:%02d','%s',%d, %d, %d, %d, '%s' ,'%s', %d); ",\
                    (local->tm_year + 1900),
                    (local->tm_mon + 1),
                    local->tm_mday,
                    local->tm_hour,
                    local->tm_min,
                    local->tm_sec,
                    com,
                    start_f,
                    end_f, 
                    up_type,
                    tot_num,
                    file_ver,
                    addr_str,
                    sgu_state);
  db_done(strSQL1, db_update,NULL,NULL);
  db_close(db_update);
  */
}

/****************************************************************************** 
* Function       :db_get_update_param
* Author         :luoj
* Date           :2016-9-1
* Description    :升级数据库，获取升级参数
* Calls          :none
* Table Accessed :db_update_param 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
void db_get_update_param(void)
{
  char strtmp[256] = {0};
  char strSQL1[256] = {0};
  char com[8] = {0};
  uint8_t param_err_flag[MAX_COM_NUM+1] = {0};

  db_open(UPDATE_DB,&db_update);
  memset(strSQL1,0,256);
  sprintf(strSQL1,"SELECT count(*) as rows FROM UPDATEPARAM WHERE START_FLAG = 1;");
  db_done(strSQL1, db_update,db_sel_callback,strtmp);
  db_close(db_update);

  //检测到启动标志
  if (atoi(strtmp) >0)
  {
    //获取参数
    db_open(UPDATE_DB,&db_update);
    printf("+++++++++++++++++++update启动标志置位+++++++++++++++++++\n");
    memset(strSQL1,0,256);
    sprintf(strSQL1,"SELECT * FROM UPDATEPARAM WHERE START_FLAG = 1; ");  
    db_done(strSQL1, db_update,db_update_param_callback, (void *)param_err_flag);
    db_close(db_update);

    //清空搜索启动标志
    printf("-------------------update清空启动标志-------------------\n");
    memset(strSQL1,0,256);
    sprintf(strSQL1,"UPDATE UPDATEPARAM SET START_FLAG = 0 WHERE START_FLAG = 1; ");
    db_operate(UPDATE_DB,strSQL1, db_update);

    //搜索参数错误
    int i;
    for(i=0; i< (MAX_COM_NUM+1);i++)
    {
      if(param_err_flag[i] == 1)
      {
        printf("(com%d) 升级参数错误,更新finish_flag........\n",i);
        sprintf(com, "COM%d", i);
        memset(strSQL1,0,256);
        sprintf(strSQL1,"UPDATE UPDATEPARAM SET FINISH_FLAG = 1 WHERE COM = '%s'; ", com);
        db_operate(UPDATE_DB, strSQL1,db_update);
      }
    } 
  }
}

/****************************************************************************** 
* Function       :db_update_master_mcu_ver
* Author         :luoj
* Date           :2016-9-1
* Description    :更新升级版本信息库
* Calls          :none
* Table Accessed :db_update_ver 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void db_update_master_mcu_ver(update_module_s *update)
{
  char com[8] = {0};
  char strSQL1[256] = {0};
  int i;

  //每个端口读取版本号结束，才写db  (update->mcu_ver_finish_flag == 1) 
  if((update->mcu_ver_tot_num > 0) && (update->mcu_ver_db_num < update->mcu_ver_tot_num))
  {
    sprintf(com, "COM%d", update->update_com);

    db_open(UPDATE_DB,&db_update);
    db_begin_trans(db_update);

    for(i=0; i< update->tot_num; i++)
    {
      if(update->iap[i].mcu_ver_flag == 1)
      {
        memset(strSQL1,0,256);
        sprintf(strSQL1,"UPDATE UPDATEINFO SET CURVER = '%s',GOALVER = '%s' WHERE COM = '%s' AND ADDR = %d; ", update->iap[i].cur_ver,update->file_ver, com,update->iap[i].addr);  
        db_done(strSQL1, db_update,NULL,NULL);

        update->iap[i].mcu_ver_flag = 0;
        update->mcu_ver_db_num++;
      }
    }

    db_commit(db_update);
    db_close(db_update); 
  }
}

/****************************************************************************** 
* Function       :db_update_iap_time
* Author         :luoj
* Date           :2016-9-1
* Description    :更新对应端口的完成标志
* Calls          :none
* Table Accessed :none 
* Table Updated  :db_update_param 
* Input          : 端口
* Output         :none 
* Return         :none
******************************************************************************/
static void db_update_iap_time(update_module_s *update)
{
  char com[8] = {0};
  char strSQL1[256] = {0};
  char strtmp[256] = {0};
  int i = 0;
  struct tm* local;
  time_t t;

  // db_open(UPDATE_DB,&db_update);
  // memset(strSQL1,0,256);
  // sprintf(strSQL1,"SELECT count(*) as rows FROM UPDATEINFO;");
  // db_done(strSQL1, db_update,db_sel_callback,strtmp);
  // db_close(db_update); 

  //检测到搜索启动标志
  // if (atoi(strtmp) <= 0)
  // {
  //   printf("(com%d) UPDATEINFO为空，不能插入update_state!!\n", update->update_com);
  //   return;
  // }

  sprintf(com, "COM%d", update->update_com);

  t = time(NULL);
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  local = localtime(&t);

  memset(strtmp, 0, 256);
  sprintf(strtmp, "%04d-%02d-%02d %02d:%02d:%02d",
                (local->tm_year + 1900),
                (local->tm_mon + 1),
                local->tm_mday,
                local->tm_hour,
                local->tm_min,
                local->tm_sec);

  db_open(UPDATE_DB,&db_update);

  db_begin_trans(db_update);

  for(i=0; i< update->tot_num; i++)
  {
    memset(strSQL1,0,256);
    sprintf(strSQL1,"UPDATE UPDATEINFO SET DATATIME = '%s' WHERE COM = '%s' AND ADDR = %d; ", 
                strtmp,
                com,
                update->iap[i].addr);  
    db_done(strSQL1, db_update,NULL,NULL);
  }

  db_commit(db_update);

  db_close(db_update); 
}


/****************************************************************************** 
* Function       :calc_process
* Author         :luoj
* Date           :2016-9-1
* Description    :计算升级进度
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void calc_process(update_module_s *update)
{
  int i = 0;
  uint8_t state = 0;

  if(update->tot_pkg_num == 0)
  {
    return;
  }

  //握手阶段不统计进度
  if(update->already_pkg_cnt == 0)
  {
    return;
  }

  for(i=0; i< update->tot_num; i++)
  {
    if(update->iap[i].update_flag == UPDATE_FAILED)
    {
      continue;
    }

    state = ((update->iap[i].state == UPDATE_STATE_SUCC)? 1:0);

    if(strstr(update->file_ver, "LCD") != NULL)
    {
      update->iap[i].process = (float)update->already_pkg_cnt /update->tot_pkg_num * 90 + (float)state* 10;  
    }
    else
    {
      update->iap[i].process = (float)update->already_pkg_cnt / update->tot_pkg_num* 85 + (float)state * 15;
    } 
  }
}

/****************************************************************************** 
* Function       :db_update_master_iap_process
* Author         :luoj
* Date           :2016-9-1
* Description    :更新主站的升级进度
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void db_update_master_iap_process(update_module_s *update)
{
  int i = 0;
  char com[8] = {0};
  char strSQL1[256] = {0};

  sprintf(com, "COM%d", update->update_com);
  
  calc_process(update);

  db_open(UPDATE_DB,&db_update);
  db_begin_trans(db_update);

  for(i=0; i< update->tot_num; i++)
  {
    memset(strSQL1,0,256);

    if(update->iap[i].update_flag > 0)
    {
      sprintf(strSQL1,"UPDATE UPDATEINFO SET PROCESS = %.1f, STATE = %d WHERE COM = '%s' AND ADDR = %d; ", update->iap[i].process,update->iap[i].update_flag, com,update->iap[i].addr);   
    }
    else
    {
      sprintf(strSQL1,"UPDATE UPDATEINFO SET PROCESS = %.1f WHERE COM = '%s' AND ADDR = %d; ", update->iap[i].process,com,update->iap[i].addr);   
    }

    db_done(strSQL1, db_update,NULL,NULL);
  }

  db_commit(db_update);
  db_close(db_update); 
}


/****************************************************************************** 
* Function       :db_update_iap_process_and_state
* Author         :luoj
* Date           :2016-9-1
* Description    :更新所有主站的升级进度和升级状态
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
void db_update_iap_process_and_state(void)
{
  mb_master_s *mb_master = mb_master_tab.head;
  int i = 0;

  for (i = 0; i < mb_master_tab.num; i++)
  {
    if(mb_master->update->start_flag == 1)
    {
      //iap升级，更新升级状态和进度
      if(mb_master->update->update_type == IAP_TYPE) 
      {
        db_update_master_iap_process(mb_master->update);

        if(mb_master->update->finish_flag == 1)   
        {
          db_update_iap_time(mb_master->update);
        }    
      }
      //mcu读取版本号
      else if((mb_master->update->update_type >= MCU_VER1_TYPE) && (mb_master->update->update_type <= MCU_VER8_TYPE))
      {
        db_update_master_mcu_ver(mb_master->update);  
      }
    }
    mb_master = mb_master->next;
  }

}

/****************************************************************************** 
* Function       :db_update_param_finish_flag
* Author         :luoj
* Date           :2016-9-1
* Description    :更新对应端口的完成标志
* Calls          :none
* Table Accessed :none 
* Table Updated  :db_update_param 
* Input          : 端口
* Output         :none 
* Return         :none
******************************************************************************/
static void db_update_param_finish_flag(uint8_t update_com)
{
  char com[8] = {0};
  char strSQL1[256] = {0};

  sprintf(com, "COM%d", update_com);

  memset(strSQL1,0,256);
  sprintf(strSQL1,"UPDATE UPDATEPARAM SET FINISH_FLAG = 1 WHERE COM = '%s' AND FINISH_FLAG = 0; ", com);  
  db_operate(UPDATE_DB,strSQL1, db_update);

}


/****************************************************************************** 
* Function       :db_update_sgu_state
* Author         :luoj
* Date           :2016-9-1
* Description    :更新sgu状态
* Calls          :none
* Table Accessed :none 
* Table Updated  :db_update_param 
* Input          : 端口, sgu_state
* Output         :none 
* Return         :none
******************************************************************************/
static void db_update_sgu_state(uint8_t sgu_state)
{
  char strSQL1[256] = {0};

  memset(strSQL1,0,256);
  sprintf(strSQL1,"UPDATE UPDATEPARAM SET FINISH_FLAG = 1,SGU_STATE = %d WHERE FINISH_FLAG = 0; ", sgu_state);  
  db_operate(UPDATE_DB,strSQL1, db_update);
}


/****************************************************************************** 
* Function       :is_it_sungrow_iap_v2
* Author         :luoj
* Date           :2016-9-1
* Description    :判断sgu文件头部
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          : 端口
* Output         :none 
* Return         :none
******************************************************************************/
static uint32_t is_it_sungrow_iap_v2(const char * sguInfo)//前26字节是sgu文件的明文信息, 包含"Sungrow","IAP","2.0".
{      
  uint32_t ret = 1;

  if( (NULL == strstr(&sguInfo[0], "Sungrow")) ||
      (NULL == strstr(&sguInfo[10], "IAP"))    ||
      (NULL == strstr(&sguInfo[20], "2.0"))  )
  {
      ret = 0;
  }

  return ret;    
}


/****************************************************************************** 
* Function       :is_sgu_ver_matched
* Author         :luoj
* Date           :2016-9-1
* Description    :判断sgu文件名和升级文件名是否一致
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          : 端口
* Output         :none 
* Return         :none
******************************************************************************/
static int is_sgu_ver_matched(char *sgu_ver, char *file_ver)
{ 
  int res = -1;
  int i=0;
  char tmp_ver[30] = {0};

  for(i=0; i< 30;i++)
  {
   // printf("%c, %d\n", sgu_ver[i], sgu_ver[i]);
    if(sgu_ver[i] == '\r' || sgu_ver[i] == '\n')
    {
      tmp_ver[i] = '\0';
      break;
    }
    else
    {
      tmp_ver[i] = sgu_ver[i];
    }
  }

  if(strcmp(tmp_ver, file_ver) == 0)
  {
    res = 0;
  }

  return res;
}


/*
static int sgu_crc_calc(char *bin_path, uint8_t en_flag)
{
  FILE *bin_fd = NULL;
  FILE *encry_fd= NULL;

  memset(sgu_module.buf8, 0, BUF8_LEN);
  memset(sgu_module.buf32, 0, BUF32_LEN);
  
  //读取补充后的bin文件,crc校验
  bin_fd = fopen(bin_path, "rb");
  fseek(bin_fd, 0, SEEK_END); 
  tot_len = ftell(bin_fd); 
  fseek(bin_fd, 0, SEEK_SET); 
  fread(sgu_module.buf8, 1, tot_len, bin_fd);

  uint32_t j;
  for(j=0; j< tot_len/4; j++)
  {
    sgu_module.buf32[j] = ((uint32_t)sgu_module.buf8[4*j+3] << 24)+((uint32_t)sgu_module.buf8[4*j+2] << 16)+((uint32_t)sgu_module.buf8[4*j+1] << 8) + sgu_module.buf8[4*j];
  }

  //u32校验
  tot_crc32 = CRC32_32(sgu_module.buf32, tot_len/4);

  //补充后的升级文件加密
  encry_fd = fopen(encry_path, "ab");
  uint8_t bin_buf[16] = {0};
  uint8_t encrypt_buf[16] = {0};
  uint32_t encry_tot_len = 0;
 
  fseek(bin_fd, 0, SEEK_SET); 
  for(i=0; i< tot_len/16; i++)
  {
    fread(bin_buf, 1, 16, bin_fd);
    aes_encrypt(bin_buf, encrypt_buf); 
    encry_tot_len += fwrite(encrypt_buf, 1, 16, encry_fd);
  }

  fclose(encry_fd);
  fclose(bin_fd);

  //获得加密后的bin文件，存入内存
  encry_fd = fopen(encry_path, "rb");
  memset(sgu_module.buf8, 0, BUF8_LEN);
  fread(sgu_module.buf8, 1, encry_tot_len, encry_fd);
  fclose(encry_fd);

  //计算总包数 包计数等参数
  sgu_module.tot_len = tot_len;
  sgu_module.tot_crc32 = tot_crc32;
  sgu_module.pkg_len = 0xd0;
  if(sgu_module.tot_len % sgu_module.pkg_len == 0)
  {
    sgu_module.tot_pkg_num = sgu_module.tot_len/sgu_module.pkg_len;
  }
  else
  {
    sgu_module.tot_pkg_num = sgu_module.tot_len/sgu_module.pkg_len + 1;
  }

  memset(log_txt, 0, 256);
  sprintf(log_txt, "analy_sgu ok: tot_len = %u, tot_crc32 = 0x%x, pkg_len = %u, tot_pkg_num = %u\n", sgu_module.tot_len,sgu_module.tot_crc32, sgu_module.pkg_len,sgu_module.tot_pkg_num);
  printf("%s",log_txt);
  update_log_write(log_fd, log_txt);

  close(log_fd);

  return 0; 
}
*/

/****************************************************************************** 
* Function       :analy_sgu_file
* Author         :luoj
* Date           :2016-9-1
* Description    :sgu文件解析
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          : 端口
* Output         :none 
* Return         :none
******************************************************************************/
static int analy_sgu_file(void)
{
  FILE * sgu_fd = NULL;
  FILE *bin_fd = NULL;
  FILE *encry_fd= NULL;
  FILE *txt_fd = NULL;
  char path[100] = {0};
  char encry_path[100] = {0};
  char sgu_path[100] = {0};
  char bin_path[100] = {0};
  char txt_path[100] = {0};
  char log_txt[256] = {0};
  uint32_t read_len = 0;
  uint8_t read_buf[256] = {0};
  uint8_t decrypt_buf[256] = {0};
  uint16_t ipkg =0;
  uint8_t sgu_ver_find =0;
  uint8_t sgu_crc_find=0;
  uint8_t sgu_len_find=0;
  uint8_t app_start_find =0;
  uint8_t code_start_find=0;
  uint8_t sgu_ok_flag =0;
  char sgu_crc[16] = {0};
  uint32_t sgu_len=0;
  uint32_t cal_crc = 0xFFFFFFFF;
  uint32_t length = 0;
  uint32_t already_read = 0;
  uint32_t i;
  char log_name[100] = {0};
  int32_t log_fd = 0;
  struct tm* local;
  time_t t;

  sprintf(path,"%s%s",UPDATE_FILE_PATH, sgu_module.file_ver); 
  strcpy(sgu_path, path);
  strcat(sgu_path, ".sgu");
  strcpy(bin_path, path);
  strcat(bin_path, ".bin");
  strcpy(encry_path, path);
  strcat(encry_path, "_en.bin");
  strcpy(txt_path, path);
  strcat(txt_path, ".txt");

  //打开sgu日志
  sprintf(log_name, "%ssgu_log.txt", UPDATE_FILE_PATH);
  log_fd = open(log_name, O_CREAT | O_RDWR | O_APPEND| O_TRUNC, 0666);
  if(log_fd <= 0)
  {
    printf("open sgu log error!\n");

    exit(EXIT_FAILURE);
  }

  //sgu解析启动写入日志
  t = time(NULL);
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  local = localtime(&t);

  memset(log_txt, 0, 256);
  sprintf(log_txt,"%04d-%02d-%02d %02d:%02d:%02d %s.sgu start analy.......\n",\
                  (local->tm_year + 1900),
                  (local->tm_mon + 1),
                  local->tm_mday,
                  local->tm_hour,
                  local->tm_min,
                  local->tm_sec,
                  sgu_module.file_ver);
  printf("%s\n", log_txt);
  update_log_write(log_fd, log_txt);

  do{
    //打开sgu文件
    sgu_fd = fopen(sgu_path, "rb");
    if(NULL == sgu_fd)
    {
      printf("open %s failed ! \n",  sgu_path);
      sgu_module.sgu_state = SGU_OPEN_ERROR;
      break;
    }
    
    //加密前的bin文件
    bin_fd = fopen(bin_path, "wb");
    if(NULL == bin_fd)
    {
      break;
    } 
    fclose(bin_fd);

    //加密后的bin文件
    encry_fd = fopen(encry_path, "wb");
    if(NULL == encry_fd)
    {
      break;
    } 
    fclose(encry_fd);

    //sgu文件头组成的txt文件
    txt_fd = fopen(txt_path, "wb");
    if(NULL == txt_fd)
    {
      break;
    } 
    fclose(txt_fd);
    
    bin_fd = fopen(bin_path, "ab");
    if(NULL == bin_fd)
    {
      break;
    }

    txt_fd = fopen(txt_path, "ab");
    if(NULL == txt_fd)
    {
      break;
    }

    //读取sgu文件头，并写入.txt
    read_len = fread(read_buf, 1, 64, sgu_fd);
    if(read_len > 0)
    {
      if(is_it_sungrow_iap_v2((const char*)read_buf) == -1)
      {
        printf("Can not find 'Sungrow IAP 2.0' in SGU file.\n");
        sgu_module.sgu_state = SGU_FORMAT_ERROR;
        break;
      }
    }
    fwrite(read_buf, 1, read_len, txt_fd);

    //读取信息体,解密并写入txt
    aes_expand_key((u8 *)AES_DECRYPT_KEY);
    read_len = fread(read_buf, 1, SGU_READ_LEN, sgu_fd);
    if(read_len != SGU_READ_LEN )
    {
      break;
    }

    for(ipkg=0; ipkg< SGU_READ_LEN/16; ipkg++)
    {
      aes_decrypt(&read_buf[16*ipkg], &decrypt_buf[16*ipkg]); 
    }
    fwrite(decrypt_buf, 1, read_len, txt_fd);

    //查找sgu版本号
    for(i=0; i< SGU_READ_LEN; i++) 
    {
      if('A'==decrypt_buf[i] && 'P'==decrypt_buf[i+1] &&
         'P'==decrypt_buf[i+2] && '='==decrypt_buf[i+3])
      {  
        if(is_sgu_ver_matched((char *)&decrypt_buf[i+4], sgu_module.file_ver) == 0)
        { 
          sgu_ver_find = 1;
          i = i + strlen((const char *)&decrypt_buf[i+4]);
          printf("sgu_ver check ok.\n");
          break;
        }
        else
        {
          printf("sgu版本和升级版本不匹配! sgu_ver = %s, update_ver = %s\n", &decrypt_buf[i+4], sgu_module.file_ver);
          sgu_ver_find = 0;
        }
      }  
    }

    //未找到，格式错误
    if(i == SGU_READ_LEN)
    {
      sgu_module.sgu_state = SGU_FORMAT_ERROR;
      break;
    }

    //sgu版本和升级版本不匹配，退出升级，更新db
    if(0 == sgu_ver_find)
    {
      sgu_module.sgu_state = SGU_VER_MATCH_ERROR;
      break;
    }

    //APP起始开始位置
    for(; i< SGU_READ_LEN; i++)
    {
      if('A' == decrypt_buf[i]   && 'P' == decrypt_buf[i+1] &&
         'P' == decrypt_buf[i+2] && '=' != decrypt_buf[i+3])
      {
        app_start_find = 1;
        i = i + 4; 
        break;
      }
    } 
    //没有找到代码区信息起始的地方. 
    if(0 == app_start_find)  
    {
      sgu_module.sgu_state = SGU_FORMAT_ERROR;
      break;
    }

    //查找crc
    for( ; i<SGU_READ_LEN; i++)
    {
      if('C'==decrypt_buf[i] && 'R'==decrypt_buf[i+1] && 
         'C'==decrypt_buf[i+2] && '='==decrypt_buf[i+3])
      {    
        memset(sgu_crc, 0, 16);
        sprintf(sgu_crc, "0x%c%c%c%c%c%c%c%c", decrypt_buf[i+4],
        decrypt_buf[i+5],decrypt_buf[i+7],decrypt_buf[i+8],
        decrypt_buf[i+10],decrypt_buf[i+11],decrypt_buf[i+13],decrypt_buf[i+14]);

        //printf("(com%d) sgu_crc= %s\n", com,sgu_crc);
        sgu_crc_find = 1;
        i = i + 15;
        break;
      }
    }
    //没有找到app代码区的CRC校验码. 
    if(0 == sgu_crc_find) 
    {
      sgu_module.sgu_state = SGU_FORMAT_ERROR;
      break;
    }

    //查找总长度
    for( ; i<SGU_READ_LEN; i++)
    {
      if('L'==decrypt_buf[i]   && 'E' == decrypt_buf[i+1] && 
         'N'==decrypt_buf[i+2] && '=' == decrypt_buf[i+3])
      { 
        sgu_len = atoi((const char *)&decrypt_buf[i+4]);
        //printf("(com%d) sgu_len = %u\n", com,sgu_len);
        sgu_len_find = 1;
        i = i + 5;
        break;
      }
    }
    //没有找到原bin文件的大小. 
    if(0 == sgu_len_find)  
    {
      sgu_module.sgu_state = SGU_FORMAT_ERROR;
      break;
    }

    // 'CODE='
    //查找代码开始的位置。接着之前的找，所以不复位下标 i。
    for( ; i< SGU_READ_LEN; i++) //前面有n字节 ，不管了。 
    {
      if('C'==decrypt_buf[i]   && 'O'==decrypt_buf[i+1] && 
         'D'==decrypt_buf[i+2] && 'E'==decrypt_buf[i+3] && 
         '='==decrypt_buf[i+4])
      {
        //printf("find 'CODE=' index = %d\n" , i+5);
        code_start_find = 1;
        break;
      }
    } 
    if(code_start_find ==0)
    {
      sgu_module.sgu_state = SGU_FORMAT_ERROR;
      break;
    } 

    //将已经解密的几个字节先写入到文件中去，i+5是代码区开始的下标。 
    length = SGU_READ_LEN-(i+5);
    already_read += length;
    fwrite(&decrypt_buf[i+5], 1, length, bin_fd);   
    cal_crc = crc32(&decrypt_buf[i+5], length, cal_crc);    
    //printf("length = %d\n", length);
    do
    {
      read_len = fread(read_buf,1,16,sgu_fd);  //读取加密的代码。

      if(16 != read_len)//加密的数据只能按16字节每组进行解密,所以读出来的数据必须是16字节。  
      {
        break;
      }
      already_read += read_len;
      
      if(already_read >= sgu_len)
      {
        length = 16 - (already_read - sgu_len);
        //printf("(alreadyWrite - binSize) = %d\n", (already_read - sgu_len));
      }
      else
      {
        length = 16;
      }
      
      aes_decrypt(read_buf, decrypt_buf); 
      fwrite(decrypt_buf, 1, length, bin_fd);
      
      cal_crc = crc32(decrypt_buf, length, cal_crc);
    }
    while(1);

    fclose(sgu_fd); 
    fclose(bin_fd); 
    fclose(txt_fd); 
    
    char cal_crc_str[16];
    sprintf(cal_crc_str,"0x%08X", ~cal_crc);  
    //printf("(com%d) cal_crc : %s\n",com, cal_crc_str);
    
    if(0 == strcmp(sgu_crc, cal_crc_str))
    {
      printf("crc check ok.\n");
      sgu_ok_flag = 1;
    }
    else
    {
      sgu_module.sgu_state = SGU_CRC_CHECK_ERROR;
      printf("crc check error!\n");
    }
      
  }while(0);

  if(sgu_ok_flag != 1)
  {
    if(sgu_module.sgu_state == SGU_NONE)
    {
      sgu_module.sgu_state = SGU_OTHER_ERROR;  
    }
    memset(log_txt, 0, 256);
    sprintf(log_txt, "sgu error, sgu_state = %d\n", sgu_module.sgu_state);
    printf("%s", log_txt );
    update_log_write(log_fd, log_txt);

    return -1;
  }
  else
  {
    sgu_module.sgu_state = SGU_ANALY_SUCCESS;
  }

  //0xff补充
  uint32_t tot_len=0, tot_crc32 = 0xFFFFFFFF;
  uint8_t add_buf[16];

  memset(add_buf, 0xff, 16);
  uint8_t add_num = (sgu_len % 16 == 0) ? 0: (16 - sgu_len % 16);
  if(add_num > 0)
  {
    printf("补充0xff个数add_num = %d\n", add_num);
    bin_fd = fopen(bin_path, "ab");
    fwrite(add_buf, 1, add_num, bin_fd);
    fclose(bin_fd);
  }
  else
  {
    //printf("(com%d) 升级文件长度已是16整数倍，不需补充0xff\n", com);
  }

  memset(sgu_module.buf8, 0, BUF8_LEN);
  memset(sgu_module.buf8_en, 0, BUF8_LEN);
  memset(sgu_module.buf32, 0, BUF32_LEN);
  
  //读取补充后的bin文件,crc校验
  bin_fd = fopen(bin_path, "rb");
  fseek(bin_fd, 0, SEEK_END); 
  tot_len = ftell(bin_fd); 
  fseek(bin_fd, 0, SEEK_SET); 
  fread(sgu_module.buf8, 1, tot_len, bin_fd);

  uint32_t j;
  for(j=0; j< tot_len/4; j++)
  {
    sgu_module.buf32[j] = ((uint32_t)sgu_module.buf8[4*j+3] << 24)+((uint32_t)sgu_module.buf8[4*j+2] << 16)+((uint32_t)sgu_module.buf8[4*j+1] << 8) + sgu_module.buf8[4*j];
  }

  //u32校验
  tot_crc32 = CRC32_32(sgu_module.buf32, tot_len/4);

  //补充后的升级文件加密
  encry_fd = fopen(encry_path, "ab");
  uint8_t bin_buf[16] = {0};
  uint8_t encrypt_buf[16] = {0};
  uint32_t encry_tot_len = 0;
 
  fseek(bin_fd, 0, SEEK_SET); 
  for(i=0; i< tot_len/16; i++)
  {
    fread(bin_buf, 1, 16, bin_fd);
    aes_encrypt(bin_buf, encrypt_buf); 
    encry_tot_len += fwrite(encrypt_buf, 1, 16, encry_fd);
  }

  fclose(encry_fd);
  fclose(bin_fd);

  //获得加密后的bin文件，存入内存
  encry_fd = fopen(encry_path, "rb");
  memset(sgu_module.buf8_en, 0, BUF8_LEN);
  fread(sgu_module.buf8_en, 1, encry_tot_len, encry_fd);
  fclose(encry_fd);
  
  //计算总包数 包计数等参数
  sgu_module.tot_len = tot_len;
  sgu_module.tot_crc32 = tot_crc32;
  sgu_module.pkg_len = 0xd0;
  if(sgu_module.tot_len % sgu_module.pkg_len == 0)
  {
    sgu_module.tot_pkg_num = sgu_module.tot_len/sgu_module.pkg_len;
  }
  else
  {
    sgu_module.tot_pkg_num = sgu_module.tot_len/sgu_module.pkg_len + 1;
  }

  memset(log_txt, 0, 256);
  sprintf(log_txt, "analy_sgu ok: tot_len = %u, tot_crc32 = 0x%x, pkg_len = %u, tot_pkg_num = %u\n", sgu_module.tot_len,sgu_module.tot_crc32, sgu_module.pkg_len,sgu_module.tot_pkg_num);
  printf("%s",log_txt);
  update_log_write(log_fd, log_txt);

  close(log_fd);

  return 0; 
}

/****************************************************************************** 
* Function       :show_update_result
* Author         :luoj
* Date           :2016-9-1
* Description    :显示升级结果
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void show_update_result(uint8_t index, update_module_s *update)
{
  char log_txt[256] = {0};
  struct tm* local;
  time_t t;

  t = time(NULL);
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  local = localtime(&t);

  //打印和写入日志
  if(update->iap[index].update_flag == UPDATE_SUCCESS)
  {
    sprintf(log_txt,"(com%d_%d) %04d-%02d-%02d %02d:%02d:%02d, 升级成功!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n",\
                    update->update_com,
                    update->iap[index].addr,
                    (local->tm_year + 1900),
                    (local->tm_mon + 1),
                    local->tm_mday,
                    local->tm_hour,
                    local->tm_min,
                    local->tm_sec);
    printf("%s", log_txt);
  }
  else if(update->iap[index].update_flag == UPDATE_FAILED)
  {
    sprintf(log_txt,"(com%d_%d) %04d-%02d-%02d %02d:%02d:%02d, 升级失败×××××××××××××××××××××××××××××××××××××××× \n",\
                    update->update_com,
                    update->iap[index].addr,
                    (local->tm_year + 1900),
                    (local->tm_mon + 1),
                    local->tm_mday,
                    local->tm_hour,
                    local->tm_min,
                    local->tm_sec);
    printf("%s", log_txt);
  }
  update_log_write(update->log_fd, log_txt);
}

void update_info_write_to_log(update_module_s *update, uint8_t flag)
{
  char log_txt[256] = {0};
  struct tm* local;
  time_t t;
  struct timeval update_t;

  t = time(NULL);
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  local = localtime(&t);

  if(flag == 0)
  {
    sprintf(log_txt,"\n(com%d) %04d-%02d-%02d %02d:%02d:%02d start update! file_ver = %s, tot_num = %u, tot_len = %u, tot_pkg_num = %u\n",\
                      update->update_com,
                      (local->tm_year + 1900),
                      (local->tm_mon + 1),
                      local->tm_mday,
                      local->tm_hour,
                      local->tm_min,
                      local->tm_sec,
                      update->file_ver,
                      update->tot_num,
                      update->tot_len,
                      update->tot_pkg_num);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);

    gettimeofday(&update_t, NULL);
    update->start_t =  ((long)update_t.tv_sec)*1000 + (long)update_t.tv_usec/1000;
  }
  else
  {
    sprintf(log_txt, "(com%d) %04d-%02d-%02d %02d:%02d:%02d 全部升级完成!!! 成功台数 = %d, 失败台数 = %d\n", \
                      update->update_com, 
                      (local->tm_year + 1900),
                      (local->tm_mon + 1),
                      local->tm_mday,
                      local->tm_hour,
                      local->tm_min,
                      local->tm_sec,
                      update->success_num, 
                      (update->tot_num -update->success_num));


    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);

    gettimeofday(&update_t, NULL);
    update->end_t =  ((long)update_t.tv_sec)*1000 + (long)update_t.tv_usec/1000;

    memset(log_txt, 0, 256);
    sprintf(log_txt, "(com%d) update_time = %.1f min\n", update->update_com, (float)(abs(update->end_t - update->start_t))/60000);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);
  }

}

/****************************************************************************** 
* Function       :create_update_cmd
* Author         :luoj
* Date           :2016-9-1
* Description    :创建升级命令
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static uint8_t create_update_cmd(uint8_t cmd_type, uint8_t index, update_module_s *update,uint8_t buf[])
{
  uint8_t cmd = 0;
  uint16_t regaddr = 0;
  uint16_t crcdata = 0xFFFF;
  uint8_t regnum = 0;
  uint8_t buf_len =0;
  uint8_t err =0;
  uint8_t devaddr = update->iap[index].addr;
  uint32_t temp_baud = 0;

  switch(cmd_type)
  {
    //读设备版本 04 lcd dsp dsp1 pvd
    case IAP_CMD_READ_MCU_VER1:
        cmd = MODBUS_CMD_04;
        regnum = 0x0f;
        regaddr = 2581;
        break;
    case IAP_CMD_READ_MCU_VER2:
        cmd = MODBUS_CMD_04;
        regnum = 0x0f;
        regaddr = 2596;
        break;
    case IAP_CMD_READ_MCU_VER3:
        cmd = MODBUS_CMD_04;
        regnum = 0x0f;
        regaddr = 2611;
        break;
    case IAP_CMD_READ_MCU_VER4:
        cmd = MODBUS_CMD_04;
        regnum = 0x0f;
        regaddr = 2626;
        break;
    case IAP_CMD_READ_MCU_VER5:
        cmd = MODBUS_CMD_04;
        regnum = 0x0f;
        regaddr = 2641;
        break;
    case IAP_CMD_READ_MCU_VER6:
        cmd = MODBUS_CMD_04;
        regnum = 0x0f;
        regaddr = 2656;
        break;
    case IAP_CMD_READ_MCU_VER7:
        cmd = MODBUS_CMD_04;
        regnum = 0x0f;
        regaddr = 2671;
        break;
    case IAP_CMD_READ_MCU_VER8:
        cmd = MODBUS_CMD_04;
        regnum = 0x0f;
        regaddr = 2686;
        break;

    //波特率相关
    case IAP_CMD_READ_PROPERTY:
        cmd = MODBUS_CMD_03;
        regnum = 0x01;
        regaddr = 0x4e23;
        break;
    case IAP_CMD_SET_DEST_BAUD:
        cmd = MODBUS_CMD_10;
        regnum = 0x02;
        regaddr = 21003;
        if(update->adjust_dirct == ADJUST_LOW_TO_HIGH)
        {
          temp_baud = HIGH_BAUD;
        }
        else
        {
          temp_baud = update->src_baud;
        }
        buf[7] = (temp_baud >> 8) & 0xFF;
        buf[8] = temp_baud & 0xFF;
        buf[9] = (temp_baud >> 24) & 0xFF;
        buf[10] = (temp_baud >> 16) & 0xFF;
        break;
    case IAP_CMD_SET_DEST_BAUD_SURE:
        cmd = MODBUS_CMD_10;
        regnum = 0x02;
        regaddr = 21005;
        if(update->adjust_dirct == ADJUST_LOW_TO_HIGH)
        {
          temp_baud = HIGH_BAUD;
        }
        else
        {
          temp_baud = update->src_baud;
        }
        buf[7] = (temp_baud >> 8) & 0xFF;
        buf[8] = temp_baud & 0xFF;
        buf[9] = (temp_baud >> 24) & 0xFF;
        buf[10] = (temp_baud >> 16) & 0xFF;
        break;
    case IAP_CMD_SET_TEST_DATA:
        cmd = MODBUS_CMD_10;
        regnum = 0x01;
        regaddr = 21007;
        uint16_t test_data = 0x55;
        buf[7] = (test_data >> 8) & 0xFF;
        buf[8] = test_data & 0xFF; 
        break;
    case IAP_CMD_SET_END_TEST:
        cmd = MODBUS_CMD_10;
        regnum = 0x01;
        regaddr = 21017;
        uint16_t end_test = 0xaa;
        buf[7] = (end_test >> 8) & 0xFF;
        buf[8] = end_test & 0xFF; 
        break;

    //iapv2升级
    case IAP_CMD_READ_IAPVER:
        cmd = MODBUS_CMD_03;
        regaddr = 0x4E1F;
        regnum = 0x02;
        break;
    case IAP_CMD_READ_STATUS:
        cmd = MODBUS_CMD_03;
        regaddr = MODBUS4X_ADDR + 1;
        regnum = 0x07;
        break;
    case IAP_CMD_READ_SOFTVER:
        cmd = MODBUS_CMD_03;
        regaddr = MODBUS4X_ADDR + 20;
        regnum = 0x18;
        break;
    case IAP_CMD_READ_LOST:
        cmd = MODBUS_CMD_03;
        regaddr = MODBUS4X_ADDR + 169;
        regnum = 0x05;
        break;
    case IAP_CMD_SET_LOGIN:
        cmd = MODBUS_CMD_10;
        regaddr = MODBUS4X_ADDR + 8;
        regnum = 0x09;
        buf[7] = 0x00;
        buf[8] = 0xAA;
        buf[9] = 0xA1;
        buf[10] = 0x90;
        buf[11] = 0x75;
        buf[12] = 0xF7;
        buf[13] = 0x09;
        buf[14] = 0xA2;
        buf[15] = 0xC7;
        buf[16] = 0x43;
        buf[17] = 0x00;
        buf[18] = update->iap[index].log_ovr; 
        buf[19] = 0x00;
        buf[20] = 0x00;
        buf[21] = 0x00;
        buf[22] = 0x00;
        /* 单台则查询传输，否则广播 */
        buf[23] = 0x00;
        buf[24] = update->iapcmd;
        break;
    case IAP_CMD_SET_LOGOUT:
        cmd = MODBUS_CMD_10;
        regaddr = MODBUS4X_ADDR + 8;
        regnum = 0x05;
        buf[7] = 0x00;
        buf[8] = 0x55;
        buf[9] = 0xA1;
        buf[10] = 0x90;
        buf[11] = 0x75;
        buf[12] = 0xF7;
        buf[13] = 0x09;
        buf[14] = 0xA2;
        buf[15] = 0xC7;
        buf[16] = 0x43;
        break;
    case IAP_CMD_SET_DEST:
        cmd = MODBUS_CMD_10;
        regaddr = MODBUS4X_ADDR + 17;
        regnum = 0x03;
        buf[7] = 0x00;
        buf[8] = 0x00;
        buf[9] = 0x00;
        buf[10] = 0x00;
        buf[11] = 0x00;
        buf[12] = 0x01;   //目标地址默认就是1
        break;
    case IAP_CMD_SET_SOFTVER:
        cmd = MODBUS_CMD_10;
        regaddr = MODBUS4X_ADDR + 20;
        regnum = 0x17;
        memcpy(&buf[7], update->file_ver, 30);
        // int t=0;
        // for(t=0; t< 30;t++)
        // {
        //   printf("%c, %d\n", buf[7+t], buf[7+t]);
        // }
        // printf("\n");
        buf[37] = HIGHBYTE(update->iap[index].auto_update);
        buf[38] = LOWBYTE(update->iap[index].auto_update);
        buf[39] = 0x00;
        buf[40] = 0x00;
        buf[41] = (update->tot_crc32 >> 8) & 0xFF;
        buf[42] = update->tot_crc32 & 0xFF;
        buf[43] = (update->tot_crc32 >> 24) & 0xFF;
        buf[44] = (update->tot_crc32 >> 16) & 0xFF;
        buf[45] = (update->tot_len >> 8) & 0xFF;
        buf[46] = update->tot_len  & 0xFF;
        buf[47] = (update->tot_len  >> 24) & 0xFF;
        buf[48] = (update->tot_len  >> 16) & 0xFF;
        buf[49] = (update->pkg_len  >> 8) & 0xFF;
        buf[50] = update->pkg_len & 0xFF;
        buf[51] = (update->tot_pkg_num >> 8) & 0xFF;
        buf[52] = update->tot_pkg_num & 0xFF;    
        break;
    case IAP_CMD_SET_DATASEND:
        cmd = MODBUS_CMD_10;
        regaddr = MODBUS4X_ADDR + 43;
        regnum = 4 + (update->dev_pkg_len >> 1);
        buf[7] = (update->pkg_cnt >> 8) & 0xFF;
        buf[8] = update->pkg_cnt & 0xFF;
        buf[9] = (update->pkg_start_addr >> 8) & 0xFF;
        buf[10] = update->pkg_start_addr & 0xFF;
        buf[11] = (update->pkg_start_addr >> 24) & 0xFF;
        buf[12] = (update->pkg_start_addr >> 16) & 0xFF;
        buf[13] = (update->dev_pkg_len >> 8) & 0xFF;
        buf[14] = update->dev_pkg_len  & 0xFF;

        int i=0;
        for(i=0; i< update->dev_pkg_len; i++)
        {
          buf[15+i] = update->data[i];
        }
        break; 
    case IAP_CMD_READ_LOWER_IAPING_NUM:
        cmd = MODBUS_CMD_03;
        regaddr = MODBUS4X_ADDR + 1;
        regnum = 0x07;
        break;
    default:
        err =1;
        break;
  }
  if(err == 0)
  {
    if(cmd_type == IAP_CMD_SET_DATASEND)
    {
      buf[0] = 0x00;
    }
    else
    {
      buf[0] = devaddr;
    } 
    buf[1] = cmd;
    buf[2] = (regaddr >> 8) & 0xFF;
    buf[3] = regaddr & 0xFF;
    buf[4] = 0x00;
    buf[5] = regnum;
    if(cmd == MODBUS_CMD_10)
    {
        buf[6] = regnum << 1;
        buf_len = 7 + buf[6];
    }
    else
    {
        buf_len = 6;
    }

    crcdata = getCRC16(buf, buf_len);
    buf[buf_len] = LOWBYTE(crcdata);
    buf[buf_len+1] = HIGHBYTE(crcdata);
    buf_len += 2;
  }

  return buf_len;

}

int32_t analy_rev_data(int32_t id,uint8_t *rev_buf, uint8_t buflen)
{
  int32_t real_len = 0;
  int res = 0;

  if (crccheck(rev_buf, buflen, 0) == 0)
  {       
    //校验错误时增加语法解析,删除前后导码,再进行crc校验
    real_len = grammar_analysis(id,rev_buf,buflen);//modbus语法解析,删除前后导码
    if (real_len > 0)
    {
      if (crccheck(rev_buf, real_len, 0) == 0)
      {
        res = -1;
      } 
    }
    else    
    {
      res =  -1;
    }   
  }
  
  return res;
}


/****************************************************************************** 
* Function       :modbus_process
* Author         :luoj
* Date           :2016-9-1
* Description    :modbus通信处理
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int modbus_process(uint8_t cmd_type, uint8_t index, update_module_s *update, uint8_t *read_data)
{
  int  i = 0;
  char log_txt[256] = {0};
  uint8_t send_buf[256] = {0};
  uint8_t rev_buf[256]={0};
  uint8_t send_len = 0 ; 
  uint8_t rev_len = 0;
  uint8_t rev_succeed = 0;
  fd_set rdfds;
  int32_t ret =0;
  struct timeval tempval;
  int16_t rev_timeout = 300;
  int32_t fd = update->fd;
  uint32_t temp_baud = 0;
  uint8_t com = update->update_com;
  int k=0;
  uint8_t canbuf[256]={0};
  uint16_t canbuf_len = 0;
  int canid_rev = 0;
  int canid_send = 0;

  send_len = create_update_cmd(cmd_type, index, update, send_buf);
  if(send_len == 0)
  {
    return -1;
  }

  switch(cmd_type)
  {
    case IAP_CMD_SET_DATASEND:
      if(update->tot_num == 1)
      {
        sprintf(log_txt, "\n(com%d_%d) 包计数= %d  @  ", com,update->iap[index].addr, update->pkg_cnt);
      }
      else
      {
        sprintf(log_txt, "\n(com%d_0) 包计数= %d  @  ", com, update->pkg_cnt);
      }  
    break;
    case IAP_CMD_READ_PROPERTY:
      sprintf(log_txt, "\n(com%d_%d) 查询属性  @  ", com, update->iap[index].addr);
    break;
    case IAP_CMD_SET_DEST_BAUD:
      sprintf(log_txt, "\n(com%d_%d) 设置目标波特率 = %u  @  ", com,update->iap[index].addr, temp_baud);
    break;
    case IAP_CMD_SET_DEST_BAUD_SURE:
      sprintf(log_txt, "\n(com%d_%d) 设置目标波特率确认  @  ", com, update->iap[index].addr);
    break;
    case IAP_CMD_SET_TEST_DATA:
      sprintf(log_txt, "\n(com%d_%d) 设置测试数据  @  ", com,update->iap[index].addr);
    break;
    case IAP_CMD_SET_END_TEST:
      sprintf(log_txt, "\n(com%d_%d) 通信质量好,设置结束测试  @  ", com,update->iap[index].addr);
    break;
    case IAP_CMD_READ_IAPVER:
      sprintf(log_txt, "\n(com%d_%d) 查询IAP协议版本号  @  ",com, update->iap[index].addr);
    break;
    case IAP_CMD_READ_STATUS:
      sprintf(log_txt, "\n(com%d_%d) 查询升级状态  @  ", com,update->iap[index].addr);
    break;
    case IAP_CMD_READ_SOFTVER:
      sprintf(log_txt, "\n(com%d_%d) 查询设备软件版本号  @  ", com,update->iap[index].addr);
    break;
    case IAP_CMD_SET_SOFTVER:
      sprintf(log_txt, "\n(com%d_%d) 设置设备软件版本号  @  ", com,update->iap[index].addr);
    break;
    case IAP_CMD_SET_LOGIN:
      sprintf(log_txt, "\n(com%d_%d) 设置登录  @  ", com,update->iap[index].addr);
    break;
    case IAP_CMD_SET_DEST:
      sprintf(log_txt, "\n(com%d_%d) 设置目标级数、目标地址 = %d  @  ", com,update->iap[index].addr,update->iap[index].addr);
    break;
    case IAP_CMD_READ_LOST:
      sprintf(log_txt, "\n(com%d_%d) 查询缺包数  @  ", com,update->iap[index].addr);
    break;
    default:
    break;
  }

  //update_log_write(update->log_fd, log_txt);

  if(update->com_type == can)
  {
    mb_slaver_s *mb_slaver = mb_slaver_tab.head;
    int t =0;
    for(t=0; t< mb_slaver_tab.num; t++)
    {
      if((update->update_com == mb_slaver->com) && (update->iap[index].addr == mb_slaver->com_id))
      {
        canid_send = mb_slaver->canid_send;
        canid_rev = mb_slaver->canid_rev;
        break;
      }
      mb_slaver = mb_slaver->next;
    }
  }
  
  //发送报文并等待接收
  for(i=0; i< MAX_UPDATE_SEND_NUM;i++)
  {
    if(update->com_type == can)
    {
      if(cmd_type == IAP_CMD_SET_DATASEND)
      {
        can_send(send_buf, send_len, canid_send, fd, 1);
      }
      else
      {
        can_send(send_buf, send_len, canid_send, fd, 0);
      }       
    }
    else if(update->com_type == uart)
    {
      write(fd, send_buf, send_len);
    }
    
    //打印发送命令 
   if(cmd_type != IAP_CMD_SET_DATASEND || update->pkg_cnt == 1)
    { 
      printf("%s",log_txt);
      int j;
      for(j=0; j<send_len;j++)
      {
        printf("0x%0x ", send_buf[j]);
      }
      printf("\n"); 
    }
 
    if(cmd_type == IAP_CMD_SET_DATASEND)
    {
      //发送升级数据,第一包延迟4s，其余包300ms
      if(update->pkg_cnt <= 1)
      {
        DELAY_MS(4000);
      }
      else
      {
        if(update->plc_enable == 0)
        {
          if(update->com_type == uart)
          {
            DELAY_MS(300);
          }
          else if(update->com_type == can)   //250K波特率
          {
            DELAY_MS(150);
          } 
        }
        else
        {
          DELAY_MS(PLC_CMD_DELAY);
        } 
      }
      //如果是广播，则只发一次，不用查询回复
      return 0;
    }
    else
    {
      if(update->plc_enable == 0)
      {
        rev_timeout = 300; 
      }
      else
      {
        rev_timeout = PLC_CMD_DELAY; 
      }
    }

    struct timeval t_start;
    gettimeofday(&t_start, NULL); 
    long start_rev = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000; 

    FD_ZERO(&rdfds);
    FD_SET(fd, &rdfds);
    rev_succeed = 0;
    memset(rev_buf, 0, 256);
    memset(canbuf, 0, 256);

    while(1)
    {
      tempval.tv_sec = rev_timeout/1000;  
      tempval.tv_usec = 1000*(rev_timeout%1000);

      ret = select(fd+1, &rdfds, NULL, NULL, &tempval);
      
      if(ret < 0)
      {
        printf("select error\n");
        break;
      } 
      else if(ret == 0) 
      {
        if(i == (MAX_UPDATE_SEND_NUM -1))  
        {   
          memset(log_txt, 0, 256);
          uint16_t regaddr = ((uint16_t)send_buf[2] << 8) + send_buf[3]; 
          sprintf(log_txt, "(com%d_%d) select timeout***********, cmd_send_cnt = %d, regaddr = 0x%0x\n", update->update_com, update->iap[index].addr, (i +1),regaddr);
          printf("%s", log_txt);

          if(update->update_type == IAP_TYPE)
          {
            update_log_write(update->log_fd, log_txt);
          }  
        }
        break;       
      }  
      else
      {
        if(FD_ISSET(fd, &rdfds))
        {
          rev_len = read(fd,rev_buf,256);
          if(rev_len > 0)
          {
            if(update->com_type == can)
            {
              canbuf_len = can_recv(canbuf, 255, rev_buf,fd, canid_rev); //can拼包
            
              if(canbuf_len > 0)
              {
                // int w =0;
                // printf("canbuf: ");
                // for(w =0; w< canbuf_len; w++)
                // {
                //   printf("0x%0x ", canbuf[w]);
                // }
                // printf("\n");
                if(analy_rev_data(send_buf[0], canbuf, canbuf_len) == 0)
                {
                  //crc正确后，根据发送报文和接收报文继续判定 (plc通信时发现会重复回复上条指令)
                  if((send_buf[1] == 0x03) || (send_buf[1] == 0x04))
                  {
                    if((send_buf[0] == canbuf[0]) && (send_buf[1] == canbuf[1]) && (canbuf[2] == (send_buf[5] <<1)))
                    {
                      rev_succeed = 1;
                    }
                  }
                  else if(send_buf[1] == 0x10)
                  {
                    if((send_buf[2] == canbuf[2]) && (send_buf[3] == canbuf[3]))
                    {
                      rev_succeed = 1;
                    }
                  }    
                }
              }
            }
            else if(update->com_type == uart)
            {
              if(analy_rev_data(send_buf[0], rev_buf, rev_len) == 0)
              {
                //crc正确后，根据发送报文和接收报文继续判定 (plc通信时发现会重复回复上条指令)
                if((send_buf[1] == 0x03) || (send_buf[1] == 0x04))
                {
                  if((send_buf[0] == rev_buf[0]) && (send_buf[1] == rev_buf[1]) && (rev_buf[2] == (send_buf[5] <<1)))
                  {
                    rev_succeed = 1;
                  }
                }
                else if(send_buf[1] == 0x10)
                {
                  if((send_buf[2] == rev_buf[2]) && (send_buf[3] == rev_buf[3]))
                  {
                    rev_succeed = 1;
                  }
                }    
              }
            }

            if(rev_succeed == 1)
            {
              if(update->com_type == can)
              {
                memcpy(rev_buf, canbuf, canbuf_len);
                rev_len = canbuf_len;
              }

              if(send_buf[1] == 0x03 || send_buf[1] == 0x04)
              {
                if(read_data != NULL)
                {
                  memcpy(read_data, &rev_buf[3], rev_len-5);
                }
              }

              printf("\n rev_cmd: ");
              for(k=0; k< rev_len; k++)
              {
                printf("0x%0x ", rev_buf[k]);
              }
              printf("\n"); 
              return 0;
            }
            else
            {
              /*
              memset(log_txt, 0, 256);
              sprintf(log_txt, "(com%d_%d) check error*********** \n", update->update_com, update->iap[index].addr);
              printf("%s", log_txt);
              update_log_write(update->log_fd, log_txt);
              
              printf("send_cmd: ");
              int k;
              for(k=0; k< send_len;k++)
              {
                printf("0x%0x ", send_buf[k]);
              }
              printf("\n rev_cmd: ");
              for(k=0; k< rev_len; k++)
              {
                printf("0x%0x ", rev_buf[k]);
              }
              printf("\n"); */

              gettimeofday(&t_start, NULL); 
              long end_rev = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;
              rev_timeout -= (int32_t)abs(end_rev-start_rev);
             // printf("rev_timeout = %d\n", rev_timeout);
              if(rev_timeout < 0)
              {
                rev_timeout = 0;
              }
            }
          }
        }
        //break;
      }  
    }
  }
  return -1;
}


/****************************************************************************** 
* Function       :mcu_ver_process,
* Author         :luoj
* Date           :2016-9-1
* Description    :读取设备版本号
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void mcu_ver_process(update_module_s *update)
{
  int i = 0;
  int res = 0;
  uint8_t cmd_type = 0;
  uint8_t buf[256] = {0};
  uint8_t com = update->update_com;
  int error = 0;
  char log_txt[256] = {0};
  struct tm* local;
  time_t t;

  t = time(NULL);
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  local = localtime(&t);

  memset(log_txt, 0, 256);
  sprintf(log_txt,"(com%d) %04d-%02d-%02d %02d:%02d:%02d 查询设备版本号.......\n",\
                  update->update_com,
                  (local->tm_year + 1900),
                  (local->tm_mon + 1),
                  local->tm_mday,
                  local->tm_hour,
                  local->tm_min,
                  local->tm_sec);
  printf("%s", log_txt);
  update_log_write(update->log_fd, log_txt);

  switch(update->update_type)
  {
    case MCU_VER1_TYPE:
      cmd_type = IAP_CMD_READ_MCU_VER1;
    break;
    case MCU_VER2_TYPE:
      cmd_type = IAP_CMD_READ_MCU_VER2;
    break;
    case MCU_VER3_TYPE:
      cmd_type = IAP_CMD_READ_MCU_VER3;
    break;
    case MCU_VER4_TYPE:
      cmd_type = IAP_CMD_READ_MCU_VER4;
    break;
    case MCU_VER5_TYPE:
      cmd_type = IAP_CMD_READ_MCU_VER5;
    break;
    case MCU_VER6_TYPE:
      cmd_type = IAP_CMD_READ_MCU_VER6;
    break;
    case MCU_VER7_TYPE:
      cmd_type = IAP_CMD_READ_MCU_VER7;
    break;
    case MCU_VER8_TYPE:
      cmd_type = IAP_CMD_READ_MCU_VER8;
    break;
    default:
      error = 1;
    break;
  }

  update->mcu_ver_tot_num = 0;

  if(error == 0)
  {
    for(i=0; i< update->tot_num;i++)
    {
      memset(buf, 0, 256);
      res = modbus_process(cmd_type, i, update, buf);
      if(res == -1)
      {
        update->iap[i].mcu_ver_flag = 0;
        memset(log_txt, 0, 256);
        sprintf(log_txt, "(com%d_%d) 读取设备版本号 通信失败\n", com, update->iap[i].addr);
        printf("%s", log_txt);
        update_log_write(update->log_fd, log_txt);

      }
      else
      {
        memcpy(update->iap[i].cur_ver, buf, 30);
        update->iap[i].mcu_ver_flag = 1;
        update->mcu_ver_tot_num++;
        //printf("(com%d_%d) cur_ver = %s\n",com, update->iap[i].addr, update->iap[i].cur_ver);
      }
    }
    update->mcu_ver_finish_flag = 1;

    if(update->mcu_ver_tot_num == 0)
    {
      memset(log_txt, 0, 256);
      sprintf(log_txt, "(com%d) 读取版本号 全失败 !!!\n", update->update_com);
      printf("%s", log_txt);
      update_log_write(update->log_fd, log_txt);
    }
    else 
    {
      while(1)
      {
        DELAY_MS(2000);

        if(update->mcu_ver_tot_num == update->mcu_ver_db_num)
        {
          memset(log_txt, 0, 256);
          sprintf(log_txt, "(com%d) 读取版本号完成，已写入db !!!\n", update->update_com);
          printf("%s", log_txt);
          update_log_write(update->log_fd, log_txt);

          break;
        }
      }
    }
  
    //更新完成标志
    db_update_param_finish_flag(update->update_com);
    //printf("(com%d) 读取版本号 更新finish_flag !!\n", update->update_com);
  }
}

/****************************************************************************** 
* Function       :iap_read_property,
* Author         :luoj
* Date           :2016-9-1
* Description    :查询属性,是否支持波特率
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
*****************************************************************************
static int iap_read_property(uint8_t index, update_module_s *update)
{
  int res = 0;
  uint8_t buf[256] = {0};
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;
  uint8_t com = update->update_com;

  res = modbus_process(IAP_CMD_READ_PROPERTY, index, update, buf);
  if(res == -1)
  {
   // iap[index].state = UPDATE_STATE_COMM_ERR;
    sprintf(log_txt, "(com%d_%d) 查询属性 通信失败\n", com,iap[index].addr);
    printf("%s",log_txt);
    update_log_write(update->log_fd, log_txt);
  }
  else
  {
    uint16_t prop = get_u16(&buf[0]);
    if((prop >>3) & 0x01)
    {
      sprintf(log_txt, "(com%d_%d) 支持动态波特率调节\n", com,iap[index].addr);
      printf("%s",log_txt);
      update_log_write(update->log_fd,log_txt);
    }
    else
    {
      sprintf(log_txt, "(com%d_%d) 不支持动态波特率调节\n", com,iap[index].addr);
      printf("%s",log_txt);
      update_log_write(update->log_fd,log_txt);
      res = -1;
    } 
  }
  return res;
}


static int iap_set_dest_baud(uint8_t index, update_module_s *update)
{
  int res = 0;
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;
  uint8_t com = update->update_com;

  res = modbus_process(IAP_CMD_SET_DEST_BAUD, index, update, NULL);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 设置目标波特率 通信失败\n", com, iap[index].addr);
    printf("%s",log_txt);
    update_log_write(update->log_fd, log_txt);
  }
  
  return res;
}

static int iap_set_dest_baud_sure(uint8_t index, update_module_s *update)
{
  int res = 0;
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;
  uint8_t com = update->update_com;

  res = modbus_process(IAP_CMD_SET_DEST_BAUD_SURE, index, update, NULL);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 设置目标波特率确认 通信失败\n", com,iap[index].addr);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);
  }
  
  return res;
}

static int iap_set_test_data(uint8_t index, update_module_s *update)
{
  int res = 0;
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;
  uint8_t com = update->update_com;

  res = modbus_process(IAP_CMD_SET_TEST_DATA, index, update, NULL);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 设置测试数据 通信失败\n", com,iap[index].addr);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);
  }
  
  return res;
}

static int iap_set_end_test(uint8_t index, update_module_s *update)
{
  int res = 0;
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;

  res = modbus_process(IAP_CMD_SET_END_TEST, index, update, NULL);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 设置结束测试 通信失败\n", update->update_com, iap[index].addr);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);
  }
  
  return res;
}

*/

/****************************************************************************** 
* Function       :iap_ver,
* Author         :luoj
* Date           :2016-9-1
* Description    :读取iap版本
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_read_iapver(uint8_t index, update_module_s *update)
{
  int res = 0;
  uint8_t buf[256] = {0};
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;

  res = modbus_process(IAP_CMD_READ_IAPVER, index, update, buf);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 查询升级协议版本 通信失败\n", update->update_com, iap[index].addr);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);
  }
  else
  {
    if((buf[0] == 0x01) && (buf[1] == 0x00) && (buf[2] == 0x02) && (buf[3] == 0x00))
    {
     
    }
    else
    {
      iap[index].state = UPDATE_STATE_IAPVER_ERR;
      sprintf(log_txt, "(com%d_%d) IAP版本不为V2\n", update->update_com, iap[index].addr);
      printf("%s", log_txt);
      update_log_write(update->log_fd, log_txt);
      res = -1;
    }
  }
  return res;
}

/****************************************************************************** 
* Function       :iap_read_update_state,
* Author         :luoj
* Date           :2016-9-1
* Description    :读取升级状态
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_read_update_state(uint8_t index, update_module_s *update)
{
  int res = 0;
  uint8_t buf[256] = {0};
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;

  res = modbus_process(IAP_CMD_READ_STATUS, index, update, buf);
  if(res == -1)
  {
    if(iap[index].last_query_state_flag == 0)
    {
      sprintf(log_txt, "(com%d_%d) 查询升级状态 通信失败\n", update->update_com, iap[index].addr);
      printf("%s", log_txt );
      update_log_write(update->log_fd, log_txt);
    }
  }
  else
  {
    iap[index].dsp_tot_num = get_u16(&buf[0]);
    iap[index].property = get_u16(&buf[2]);
    iap[index].state = get_u16(&buf[4]);
    iap[index].anomally = get_u16(&buf[6]);
    iap[index].dsp_ok_flag = get_u16(&buf[10]);

    uint8_t aes_suppoerted = GETBIT(iap[index].property, 0);
    memset(log_txt, 0, 256);
    if(aes_suppoerted == 0)
    {
      update->aes_flag = 1;   //不支持解密，logger需要解密,发明文
      sprintf(log_txt, "(com%d_%d) 不支持解密，logger要发明文\n", update->update_com, iap[index].addr);
      printf("%s", log_txt);
    }
    else
    {
      update->aes_flag = 0;   //支持解密，logger不需要解密，发密文
      sprintf(log_txt, "(com%d_%d) 支持解密，logger要发密文\n", update->update_com, iap[index].addr);
      printf("%s", log_txt);
    }

    if(iap[index].last_query_state_flag == 0)
    {
      update_log_write(update->log_fd, log_txt);
    }
    
    if(iap[index].dsp_ok_flag > 0)
    {
      int i;
      iap[index].dsp_already_num = 0;
      for(i=0; i<iap[index].dsp_tot_num; i++)
      {
        if(((iap[index].dsp_ok_flag >> i) & 0x01))
        {
          iap[index].dsp_already_num++;
        }
      }
    }
  }
  return res;
}

/****************************************************************************** 
* Function       :iap_set_login
* Author         :luoj
* Date           :2016-9-1
* Description    :设置登录命令
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_set_login(uint8_t index, update_module_s *update)
{
  int res = 0;
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;

  res = modbus_process(IAP_CMD_SET_LOGIN, index, update, NULL);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 握手登录 通信失败\n", update->update_com, iap[index].addr);
    printf("%s",log_txt );
    update_log_write(update->log_fd, log_txt);
  }
  return res;
}

/****************************************************************************** 
* Function       :iap_set_dest
* Author         :luoj
* Date           :2016-9-1
* Description    :设置目标级数
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_set_dest(uint8_t index, update_module_s *update)
{
  int res = 0;
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;

  res = modbus_process(IAP_CMD_SET_DEST , index, update, NULL);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 设置目标级数、目标地址 通信失败\n", update->update_com, iap[index].addr);
    printf("%s",log_txt );
    update_log_write(update->log_fd, log_txt);
  }
  return res;
}


/****************************************************************************** 
* Function       :iap_read_softver
* Author         :luoj
* Date           :2016-9-1
* Description    :读取软件版本
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_read_softver(uint8_t index, update_module_s *update)
{
  int res = 0;
  uint8_t buf[256] = {0};
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;
  //char dev_ver[30];

  res = modbus_process(IAP_CMD_READ_SOFTVER , index, update, buf);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 读取软件版本号、自动升级标志、校验值、总长度、每包长度、总包数、包计数 通信失败\n", update->update_com, iap[index].addr);
    printf("%s",log_txt);
    update_log_write(update->log_fd, log_txt);
  }
  else
  {
    //判断设备版本和要升级的目标版本，如果一致且包计数不为0,启动断点续传

    /*
    memcpy(dev_ver,buf, 30);

    int result = strcmp(update->file_ver, dev_ver);
    if(result == 0)
    {
      printf("(com%d_%d) 升级文件版本和设备软件版本号一致\n", update->iap[index].addr);

      uint16_t tot_pkg_num , pkg_len, pkg_cnt;
      uint32_t tot_crc32, tot_len;

      tot_crc32 = get_u32(&buf[34]); //总crc32
      tot_len = get_u32(&buf[38]);  //总长度
      pkg_len = get_u16(&buf[42]);  //每包长度 
      tot_pkg_num = buf[44] * 256 + buf[45];//总包数
      pkg_cnt = buf[46] * 256 + buf[47];//包计数

      if(tot_pkg_num != pkg_cnt)
      {
        printf("(com%d_%d) 有未完成的升级\n", update->iap[index].addr);
      }

      //判断crc32
      if(tot_crc32 == update->tot_crc32)
      {
        printf("(com%d_%d) 校验值一致\n", update->iap[index].addr);
        update->tot_crc32 = tot_crc32;
        update->tot_len = tot_len;
        update->pkg_len = pkg_len; 
        update->tot_pkg_num = tot_pkg_num;
        update->begin_pkg_cnt = pkg_cnt;

        //版本号一致，crc32一致，包计数不为0,断点续传
        if(pkg_cnt != 0)
        {
          update->iap[index].go_on_flag = 1;
          printf("(com%d_%d) 断点续传，pkg_cnt = %d, tot_pkg_num = %d\n", update->iap[index].addr, pkg_cnt, tot_pkg_num);
        }
      }
      else
      {
        printf("(com%d_%d) 校验值不一致\n", update->iap[index].addr);
        res = -1;
      }
    }
    else if(result > 0)
    {
      printf("(com%d_%d) 升级文件版本 > 设备软件版本, 自动升级\n", update->iap[index].addr);
      update->iap[index].auto_update = 0xaa;
    }
    else if(result < 0)
    {
      printf("(com%d_%d) 升级文件版本 < 设备软件版本, 强制升级\n", update->iap[index].addr);
      //强制升级
      update->iap[index].auto_update = 0xaa;
    } */

    update->iap[index].auto_update = 0xaa;
  }
  return res;
}

/****************************************************************************** 
* Function       :iap_set_softver
* Author         :luoj
* Date           :2016-9-1
* Description    :读取软件版本
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_set_softver(uint8_t index, update_module_s *update)
{
  int res = 0;
  char log_txt[256] = {0};
  iap_module_s *iap = update->iap;

  res = modbus_process(IAP_CMD_SET_SOFTVER , index, update, NULL);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 设置软件版本号、自动升级标志、校验值等 通信失败\n",update->update_com,  iap[index].addr);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);
  }
  return res;
}

/****************************************************************************** 
* Function       :iap_send_data
* Author         :luoj
* Date           :2016-9-1
* Description    :发送升级数据
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_send_data(update_module_s *update)
{
  int i = 0;
  int res = 0;
  char log_txt[256] = {0};

  if(update->begin_pkg_cnt < 1)
  {
    printf("begin_pkg_cnt < 1 !\n");
    return -1;
  }

  //时间统计
  long start_t,end_t;
  struct timeval t;

  gettimeofday(&t, NULL);
  start_t =  ((long)t.tv_sec)*1000+(long)t.tv_usec/1000;

  sprintf(log_txt, "(com%d) tot_pkg_num = %d, tot_len = %d\n", update->update_com, update->tot_pkg_num, update->tot_len);
  printf("%s", log_txt);
  update_log_write(update->log_fd, log_txt);
  
  update->already_pkg_cnt = 0;

  for(i= update->begin_pkg_cnt; i<= update->tot_pkg_num; i++)
  {
    update->pkg_cnt = i;   //包计数
    update->pkg_start_addr = (i -1) * update->pkg_len;  //每包的起始地址

    if (i == update->tot_pkg_num)
    {
      update->dev_pkg_len = update->tot_len - (i - 1) * update->pkg_len; //下发的每包长度
    }
    else
    {
      update->dev_pkg_len = update->pkg_len;
    }

    //剩余5包时，恢复波特率
    /*
    if((update->adjust_flag == 1) &&(update->tot_num == 1) && (update->pkg_cnt == (update->tot_pkg_num -5)))
    {
      res = recover_baud(0, update);
      if(res == 0)
      {
        update->adjust_flag = 0;
        printf("动态波特率 恢复成功!!!!!\n");    
      }
      else
      {
        printf("动态波特率 回复失败××××××\n");   
      }
    } */

    uint32_t id = (uint32_t)(i-1) * update->pkg_len;
    memset(update->data, 0, 208);

    if(update->aes_flag == 1)   //logger需要解密，发的是明文
    {
      memcpy(update->data, &sgu_module.buf8[id], update->dev_pkg_len);
    }
    else   //发的是密文
    {
      memcpy(update->data, &sgu_module.buf8_en[id], update->dev_pkg_len);
    }
    
    
    //全部广播升级
    res = modbus_process(IAP_CMD_SET_DATASEND, 0, update, NULL);
    if(res == -1)
    {
      /*
      iap[index].state = UPDATE_STATE_COMM_ERR;
      printf("(com%d_%d) 发送升级数据 通信失败, pck_cnt = %d\n", update->update_com, iap[index].addr, update->pkg_cnt);
      sprintf(log_txt, "(com%d_%d) 发送升级数据 通信失败，pck_cnt = %d", update->update_com, iap[index].addr, update->pkg_cnt);
      //update_log_write(update->log_fd, log_txt);
      continue;  //一包不回，继续发下一台，留补包
      */  
    }
    else
    {
      update->already_pkg_cnt++;
    }
  }

  gettimeofday(&t, NULL);
  end_t =  ((long)t.tv_sec)*1000+(long)t.tv_usec/1000;
  sprintf(log_txt, "(com%d_0) update_send_data_t = %.1f min\n", update->update_com, (float)(abs(end_t - start_t))/60000);
  printf("%s",log_txt );
  update_log_write(update->log_fd, log_txt);
  return res;
}


/****************************************************************************** 
* Function       :iap_read_lost
* Author         :luoj
* Date           :2016-9-1
* Description    :读取缺包数
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_read_lost(uint8_t index, update_module_s *update)
{
  int res = 0;
  char log_txt[256] = {0};
  uint8_t buf[256] = {0};
  iap_module_s *iap = update->iap;

  res = modbus_process(IAP_CMD_READ_LOST , index, update, buf);
  if(res == -1)
  {
    /*
    printf("(com%d_%d) 读缺包数 通信失败\n", update->update_com, iap[index].addr);
    sprintf(log_txt, "(com%d_%d) 读缺包数 通信失败", update->update_com, iap[index].addr);
    //update_log_write(update->log_fd, log_txt);
    */
  }
  else
  {
    iap[index].lost_no = get_u16(&buf[0]);
    iap[index].lost_start_addr = get_u32(&buf[2]);
    iap[index].lost_len = get_u16(&buf[6]);
    iap[index].lost_num = get_u16(&buf[8]);
    
    if(iap[index].lost_num > 0 )
    {
      sprintf(log_txt, "(com%d_%d) 缺包lost_no = %d, lost_num = %d\n", update->update_com, iap[index].addr, iap[index].lost_no, iap[index].lost_num);
      printf("%s",log_txt );
      update_log_write(update->log_fd, log_txt);
    }
  }
  return res;
}

/****************************************************************************** 
* Function       :iap_send_lost_data
* Author         :luoj
* Date           :2016-9-1
* Description    :发送缺包数据
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_send_lost_data(uint8_t index, update_module_s *update)
{
  int res = 0;
 // char log_txt[256] = {0};
  iap_module_s *iap = update->iap;

  update->pkg_cnt = iap[index].lost_no;   //缺包计数
  update->pkg_start_addr = iap[index].lost_start_addr;  //缺包起始地址

  update->dev_pkg_len = 208;// iap[index].lost_len; //缺包长度

  if(update->pkg_cnt < 1)
  {
    printf("缺包计数为0，不能补包！\n");
    return -1;
  }
  uint32_t id = (uint32_t)(update->pkg_cnt-1) * update->pkg_len;

  memset(update->data, 0, 208);

  if(update->aes_flag == 1)   //logger需要解密，发的是明文
  {
    memcpy(update->data, &sgu_module.buf8[id], update->dev_pkg_len);
  }
  else   //发的是密文
  {
    memcpy(update->data, &sgu_module.buf8_en[id], update->dev_pkg_len);
  }

  res = modbus_process(IAP_CMD_SET_DATASEND, index, update, NULL);
  if(res == -1)
  {
    /*
    iap[index].state = UPDATE_STATE_COMM_ERR;
    printf("(com%d_%d) 发送补包数据 通信失败, lost_no = %d\n", update->update_com, iap[index].addr, iap[index].lost_no);
    sprintf(log_txt, "(com%d_%d) 发送补包数据 通信失败, lost_no = %d", update->update_com, iap[index].addr, iap[index].lost_no);
    //update_log_write(update->log_fd, log_txt);
    */
  }
  else
  {

  }
  return res;
}

/****************************************************************************** 
* Function       :iap_read_lower_iaping_num
* Author         :luoj
* Date           :2016-9-1
* Description    :读取正在向下级升级的发包数
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
*****************************************************************************
static int iap_read_lower_iaping_num(uint8_t index,update_module_s *update)
{
  int res = 0;
  char log_txt[256] = {0};
  uint8_t buf[256] = {0};
  iap_module_s *iap = update->iap;

  res = modbus_process(IAP_CMD_READ_LOWER_IAPING_NUM , index, update, buf);
  if(res == -1)
  {
    sprintf(log_txt, "(com%d_%d) 查询向下发包计数下 通信失败\n", update->update_com, iap[index].addr);
    printf("%s",log_txt );
    update_log_write(update->log_fd, log_txt);
  }
  else
  {
    iap[index].lower_iaping_num = get_u16(&buf[0]);
  }
  return res;
}*/

/****************************************************************************** 
* Function       :iap_hand_process
* Author         :luoj
* Date           :2016-9-1
* Description    :广播升级数据之前，握手处理
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static int iap_hand_process(uint8_t index, update_module_s *update)
{
  int res =0;
  char log_txt[256] = {0};
  int32_t log_fd = update->log_fd;

  //查询iap版本
  res = iap_read_iapver(index, update);
  if(res == -1)
  {
    return -1;
  }

  //查询升级状态
  res = iap_read_update_state(index, update);
  if(res == -1)
  {
    return -1;
  }

  if(update->iap[index].state != UPDATE_STATE_LOWER_IAPING)    //正在向下级升级
  {
    //握手登录
    res = iap_set_login(index, update);
    if(res == -1)
    {
      return -1;
    }

    //设置目标级数
    res = iap_set_dest(index, update);
    if(res == -1)
    {
      return -1;
    }

    //读软件版本等信息
    res = iap_read_softver(index, update);
    if(res == -1)
    {
      return -1;
    }
    //非断点续传,设置软件版本等
    if(update->iap[index].go_on_flag == 0)
    {
      res = iap_set_softver(index, update);
      if(res == -1)
      {
        return -1;
      }
    }
    //查询升级状态
    res = iap_read_update_state(index, update);
    if(res == -1)
    {
      return -1;
    }
    if(update->iap[index].state != UPDATE_STATE_IAPING)
    {
      sprintf(log_txt, "(com%d_%d) 不是'正在升级', 升级失败退出 !!!!, state = %d, anomally = %d\n", update->update_com,update->iap[index].addr, update->iap[index].state, update->iap[index].anomally);
      printf("%s", log_txt);
      update_log_write(log_fd, log_txt);
      return -1;
    }      
  } 
  else   
  {
    printf("正在向下级升级.......\n");
    update_log_write(log_fd, "正在向下级升级.......\n");

    //循环查询状态，等待向下级升级结束
    uint16_t cnt =0;
    while(cnt < LOWER_IAP_WAIT_TIME)  //暂定400次
    {
      cnt++;

      res = iap_read_update_state(index, update);
      if(res == -1)
      {
        return -1;
      }

      if(update->iap[index].state == UPDATE_STATE_LOWER_IAPING)  //正在向下级升级
      {
        sleep(1);
        continue;
      }
      else if(update->iap[index].state == UPDATE_STATE_LOWER_IAPEND) //向下级升级结束
      {
        //是不是轮循各子设置的状态信息 DSP PVD等
        sprintf(log_txt, "(com%d_%d) 向下级升级结束，升级成功退出\n",update->update_com, update->iap[index].addr);
        printf("%s", log_txt);
        update_log_write(log_fd, log_txt);
        update->iap[index].update_flag = UPDATE_SUCCESS;
        return 0;
      }
    }

    if(cnt == LOWER_IAP_WAIT_TIME)
    {
      sprintf(log_txt, "(com%d_%d) 正在向下级升级 超时退出\n",update->update_com, update->iap[index].addr);
      printf("%s", log_txt);
      update_log_write(log_fd, log_txt);
      update->iap[index].update_flag = UPDATE_FAILED;
      return -1;
    }
  }

  return 0;
}


/****************************************************************************** 
* Function       :set_dev_baud,
* Author         :luoj
* Date           :2016-9-1
* Description    :设置设备波特率
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
*****************************************************************************
static int set_dev_baud(uint8_t index, update_module_s *update, uint32_t dev_baud)
{
  int i;
  comport_s comport;

  mb_master_s *mb_master = mb_master_tab.head;
  for (i = 0; i < mb_master_tab.num; i++)
  {
    if(mb_master->comport.uartid == update->update_com)
    {
      memcpy(&comport, &mb_master->comport, sizeof(comport_s));
      break;
    }
    mb_master = mb_master->next;
  }

  printf("\n(com%d_%d) 关闭串口", update->update_com, update->iap[index].addr);
  CloseComPort(update->fd);
  
  comport.uartbaudrate = dev_baud;
  printf("，设置串口波特率 = %d\n", comport.uartbaudrate);
  update->fd = OpenComPort(comport.uartid, comport.uartbaudrate, comport.uartdatabit, comport.uartstopbit, comport.uartparity);
  if(update->fd== -1)
  {
    printf("重新打开串口失败！\n");
    return -1;
  }
  mb_master->fd = update->fd;

  return 0;
}

*/


/****************************************************************************** 
* Function       :recover_baud,
* Author         :luoj
* Date           :2016-9-1
* Description    :动态波特率回复
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
*****************************************************************************
static int recover_baud(uint8_t index, update_module_s *update)
{
  int res;

  printf("(com%d_%d) 恢复波特率为%d\n", update->update_com, update->iap[index].addr,update->src_baud);

  if(update->adjust_flag == 0)
  {
    printf("(com%d_%d) 已经是低波特率，不需调节\n", update->update_com, update->iap[index].addr);
    return -1;
  }

  if(update->tot_num > 1)
  {
    printf("(com%d_%d) 设备台数>1, 不支持波特率回复\n", update->update_com, update->iap[index].addr);
    return -1;
  }

  //以57600设置目标波特率：9600
  update->adjust_dirct = ADJUST_HIGH_TO_LOW;
  res = iap_set_dest_baud(index, update);
  if(res == -1)
  {
    return -1;
  }

  //设置目标波特率确认
  res = iap_set_dest_baud_sure(index, update);
  if(res == -1)
  {
    return -1;
  }

  //关闭串口，以9600打开
  res = set_dev_baud(index, update, update->src_baud);
  if(res == -1)
  {
    return -1;
  }

  //向设备发送9600波特率确认包
  sleep(5);
  res = iap_set_dest_baud(index, update);
  if(res == -1)
  {
    return -1;
  }

  //设置目标波特率确认
  res = iap_set_dest_baud_sure(index, update);
  if(res == -1)
  {
    return -1;
  }

  return 0;
}

*/


/****************************************************************************** 
* Function       :adjust_baud,
* Author         :luoj
* Date           :2016-9-1
* Description    :动态波特率调节
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :目标波特率 原有波特率
* Output         :none 
* Return         :none
*****************************************************************************
static int adjust_baud(uint8_t index,update_module_s *update)
{
  int res = 0, i;

  printf("(com%d_%d) 调节波特率为%d\n", update->update_com, update->iap[index].addr,HIGH_BAUD);

  if(update->tot_num > 1)
  {
    printf("(com%d_%d) 设备台数>1, 不支持波特率调节\n", update->update_com, update->iap[index].addr);
    return -1;
  }

  if(update->src_baud >= HIGH_BAUD)
  {
    printf("(com%d_%d) 默认波特率(%u > %u)较高，不调节 \n", update->update_com, update->iap[index].addr, update->src_baud, HIGH_BAUD);
    return -1;
  }

  //查询属性
  res = iap_read_property(index, update);
  if(res == -1)
  {
    return -1;
  }

  //设置目标波特率
  update->adjust_dirct = ADJUST_LOW_TO_HIGH;
  res = iap_set_dest_baud(index, update);
  if(res == -1)
  {
    return -1;
  }

  //设置目标波特率确认
  res = iap_set_dest_baud_sure(index, update);
  if(res == -1)
  {
    return -1;
  }

  //关闭串口，以57600打开
  res = set_dev_baud(index, update, HIGH_BAUD);
  if(res == -1)
  {
    return -1;
  }

  //以新波特率测试通信
  sleep(10);
  uint16_t fail_time =0;
  for(i=0; i< 20; i++)
  {
    res = iap_set_test_data(index, update);
    if(res == 0)
    {
      continue;
    }
    else
    {
      fail_time++;
      if(fail_time > 5)   //通信质量不好
      {
        //恢复成默认波特率
        printf("(com%d_%d) 通信质量不好，恢复成默认波特率%d\n", update->update_com, update->iap[index].addr, update->src_baud);
        res = recover_baud(index, update);
        if(res == 0)
        {
          update->adjust_flag = 0;
          printf("动态波特率 恢复成功!!!!!\n");    
        }
        else
        {
          printf("动态波特率 回复失败××××××\n");   
        }
        return -1;
      }
    }
  }

  //通信质量好，结束数据测试
  if(fail_time <= 5)
  {
    iap_set_end_test(index, update);
    if(res == -1)
    {
      return -1;
    } 
  }

  return 0;
} 

*/


  //如果是单台，当前波特率小于目标波特率，调节到57600
  /*
  if(update->tot_num == 1)
  {
    res = adjust_baud(0, update);
    if(res == 0)
    {
      update->adjust_flag = 1;
      printf("动态波特率 调节成功!!!!!\n");   
    }
    else
    {
      update->adjust_flag = 0;
      printf("动态波特率 调节失败××××××\n");   
    }
  } */

  //恢复到原有波特率
  /*
  if(update->adjust_flag == 1 && update->tot_num ==1)
  {
    res = recover_baud(0, update);
    if(res == 0)
    {
      update->adjust_flag = 0;
      printf("动态波特率 恢复成功!!!!!\n");    
    }
    else
    {
      printf("动态波特率 回复失败××××××\n");   
    }
  }
  */

/****************************************************************************** 
* Function       :iap_process,
* Author         :luoj
* Date           :2016-9-1
* Description    :升级处理
* Calls          :none
* Table Accessed :none 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
static void iap_process(update_module_s *update)
{
  int i,j;
  int res =0;
  char log_txt[256] = {0};

  while(1)
  {
    //升级完成，更新数据库，退出
    if(update->finish_flag == 1)
    {
      //延时1.1s，用于定时线程写状态
      DELAY_MS(2000); 

      //更新升级完成标志s
      db_update_param_finish_flag(update->update_com);
      memset(log_txt, 0, 256);
      sprintf(log_txt, "(com%d) finish_flag写入db.\n", update->update_com);
      printf("%s", log_txt);
      update_log_write(update->log_fd, log_txt);
      break;
    }

    //判断是否全部升级完成
    for(i=0; i< update->tot_num; i++)
    {
      if(update->iap[i].update_flag == UPDATE_NONE_STATE)
      {
        break;
      }
    }

    //全部升级完成
    if(i == update->tot_num)
    {
      update->success_num = 0;

      for(j=0; j< update->tot_num; j++)
      {
        if(update->iap[j].update_flag != UPDATE_NONE_STATE)
        {
          memset(log_txt, 0, 256);
          sprintf(log_txt, "(com%d_%d) state = %d, anomally = %d, update_flag = %d\n", update->update_com, update->iap[j].addr, update->iap[j].state, update->iap[j].anomally, update->iap[j].update_flag);
          printf("%s", log_txt);
          update_log_write(update->log_fd, log_txt);
        }
        if(update->iap[j].state == UPDATE_STATE_SUCC)
        {
          update->success_num++;
        }
      }

      update_info_write_to_log(update, 1);
      //更新完成标志
      update->finish_flag = 1;
      continue;
    }

    memset(log_txt, 0, 256);
    sprintf(log_txt, "(com%d) 升级前握手.........\n",update->update_com);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);
    //循环向每台发送握手命令
    for(i=0; i< update->tot_num;i++)
    {
      res = iap_hand_process(i, update);
      if(res == -1)
      {
        update->iap[i].update_flag = UPDATE_FAILED;
        show_update_result(i, update);
      }
      else 
      {
        if(update->iap[i].update_flag == UPDATE_SUCCESS)   //正在向下级升级成功情况
        {
          show_update_result(i, update);
        }
      } 
    }
    
    //如果每一台握手都失败，则退出升级流程
    for(i=0; i< update->tot_num; i++)
    {
      if(update->iap[i].update_flag == UPDATE_NONE_STATE)
      {
        break;
      }
    }
    if(i == update->tot_num)   
    {
      memset(log_txt, 0, 256);
      sprintf(log_txt, "(com%d) 握手均失败，退出升级流程.........\n",update->update_com);
      printf("%s", log_txt);
      update_log_write(update->log_fd, log_txt);
      continue;
    }

    char test_txt[10] = {0};
    if(update->aes_flag == 1)
    {
      strcpy(test_txt, "明文");
    }
    else
    {
      strcpy(test_txt,"密文");
    }

    memset(log_txt, 0, 256);
    sprintf(log_txt, "(com%d) 发送(%s)升级数据, .........\n",update->update_com, test_txt);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);
    iap_send_data(update);

    memset(log_txt, 0, 256);
    sprintf(log_txt, "(com%d) 补包及状态查询.........\n",update->update_com);
    printf("%s", log_txt);
    update_log_write(update->log_fd, log_txt);
    for(i=0; i< update->tot_num;i++)
    {
      //已经成功或失败，则不补包
      if(update->iap[i].update_flag != UPDATE_NONE_STATE)
      {
        continue;
      }

      //读缺包数, 设备不回，不处理，等待状态查询
      res = iap_read_lost(i, update);
      if(res == -1)
      {
        memset(log_txt, 0, 256);
        sprintf(log_txt, "(com%d_%d) < 初始 > 读缺包失败.\n", update->update_com, update->iap[i].addr);               
        printf("%s",log_txt);
        update_log_write(update->log_fd, log_txt);
      }

      //循环补包
      uint16_t lost_tot_num = 0;
      while( update->iap[i].lost_num > 0)
      {        
        iap_send_lost_data(i, update);    //发送缺包数

        res = iap_read_lost(i, update);   //读取缺包数不回就跳出，直接查询升级状态
        if(res == -1)
        {
          memset(log_txt, 0, 256);
          sprintf(log_txt, "(com%d_%d) < 补包过程中 > 读缺包失败, lost_num = %d\n", update->update_com, update->iap[i].addr, update->iap[i].lost_num);               
          printf("%s",log_txt);
          update_log_write(update->log_fd, log_txt);
          break;
        }

        //限定每次补包总数，防止死循环
        lost_tot_num++;
        if(lost_tot_num >= (update->tot_pkg_num/2))
        {
          break;
        }
      }

      //补包结束，如果补包不全，不再查升级状态，退出升级流程
      //问题??  如果补完最后一包，但还没来得及查询缺包，设备立即重启了，此时logger已经退出状态查询，注定这台升级失败
      if(update->iap[i].lost_num > 1)
      {
        memset(log_txt, 0, 256);
        sprintf(log_txt, "(com%d_%d) 补包失败，退出状态查询.\n", update->update_com, update->iap[i].addr);
        printf("%s", log_txt);
        update_log_write(update->log_fd, log_txt);

        update->iap[i].update_flag = UPDATE_FAILED;
        show_update_result(i, update);

        continue;
      }

      //长时间查询设备状态
      update->iap[i].last_query_state_flag = 1;
      memset(log_txt, 0, 256);
      sprintf(log_txt, "(com%d_%d) 补包结束，升级状态查询.......\n",update->update_com, update->iap[i].addr);
      printf("%s", log_txt);
      update_log_write(update->log_fd, log_txt);

      //最大超时1min读取每台的升级状态
      struct timeval t;
      long start_t = 0;
      long end_t = 0;

      gettimeofday(&t, NULL);
      start_t =  ((long)t.tv_sec)*1000 + (long)t.tv_usec/1000;

      while(1)
      {
        gettimeofday(&t, NULL);
        end_t =  ((long)t.tv_sec)*1000 + (long)t.tv_usec/1000;

        if(abs(end_t - start_t) > 60000)   //1min
        {
          memset(log_txt, 0, 256);
          sprintf(log_txt, "(com%d_%d) 超时未查询升级成功，退出. state = %d\n",update->update_com, update->iap[i].addr,  update->iap[i].state);
          printf("%s", log_txt);
          update_log_write(update->log_fd, log_txt);
          break;
        }
        else
        {
          //查询状态
          res = iap_read_update_state(i, update);
          if(res == -1)
          {
            continue;
          }

          //CRC校验失败，退出
          if(update->iap[i].anomally == 0x200)
          {
            update->iap[i].update_flag = UPDATE_FAILED;
            break;
          }

          //升级成功
          if(update->iap[i].state == UPDATE_STATE_SUCC)              
          {
            update->iap[i].update_flag = UPDATE_SUCCESS;
            show_update_result(i,update);
            break;
          }
          //升级失败
          else if(update->iap[i].state == UPDATE_STATE_ERROR)
          {
            update->iap[i].update_flag = UPDATE_FAILED;
            show_update_result(i,update);
            break;
          }
          //本级升级结束，分析dsp升级情况
          else if(update->iap[i].state == UPDATE_STATE_LOWER_IAPDSP)  
          {
            if(update->iap[i].dsp_tot_num == update->iap[i].dsp_already_num)
            {
              update->iap[i].state = UPDATE_STATE_SUCC;
              update->iap[i].update_flag = UPDATE_SUCCESS;
              memset(log_txt, 0, 256);
              sprintf(log_txt, "(com%d_%d) DSP升级成功.\n", update->update_com, update->iap[i].addr);   
              printf("%s", log_txt);
              update_log_write(update->log_fd, log_txt);
            }
            else
            {
              update->iap[i].update_flag = UPDATE_FAILED;
              //查询下哪台失败
              memset(log_txt, 0, 256);
              sprintf(log_txt, "(com%d_%d) DSP升级失败. dsp总数 = %d, 已升级总数 = %d, 升级成功标志 = %d.\n", \
                update->update_com, 
                update->iap[i].addr,
                update->iap[i].dsp_tot_num,
                update->iap[i].dsp_already_num,
                update->iap[i].dsp_ok_flag); 
              printf("%s", log_txt);
              update_log_write(update->log_fd, log_txt);
            }
            break;
          } 
        }
      }

      if(update->iap[i].state != UPDATE_STATE_SUCC)
      {
        update->iap[i].update_flag = UPDATE_FAILED;
        show_update_result(i, update);
      }
    }
  }
}


/****************************************************************************** 
* Function       :update_process
* Author         :luoj
* Date           :2016-9-1
* Description    :升级处理
* Calls          :none
* Table Accessed :none 
* Table Updated  :db_update 
* Input          :*update 升级模块
* Output         :none 
* Return         :none
******************************************************************************/
static void update_process(update_module_s *update)
{
  update_log_open(update);    //创建端口升级日志

  if(update->update_type == IAP_TYPE)  //iap升级
  { 
    if((sgu_module.sgu_state == SGU_ANALY_SUCCESS) && (sgu_module.tot_len > 0))
    {
      update->tot_len = sgu_module.tot_len;
      update->tot_crc32 = sgu_module.tot_crc32;
      update->tot_pkg_num = sgu_module.tot_pkg_num;
      update->pkg_len = sgu_module.pkg_len;

      update_info_write_to_log(update, 0);

      iap_process(update);
    }
    else
    {
      char log_txt[256] = {0};
      sprintf(log_txt, "%s.sgu解析失败，不能升级(可能进程重启，内存清空) !!! sgu_state = %d, tot_len = %u\n", sgu_module.file_ver, sgu_module.sgu_state, sgu_module.tot_len);
      printf("%s\n", log_txt);
      update_log_write(update->log_fd, log_txt);
      //置位完成标志
      db_update_param_finish_flag(update->update_com);
    }  
  }
  else  //读取设备版本号
  { 
    mcu_ver_process(update);
  } 

  update_log_close(update->log_fd);
}


/****************************************************************************** 
* Function       :ModbusMasterUpdate
* Author         :luoj
* Date           :2016-9-1
* Description    :升级线程
* Calls          :none
* Table Accessed :none 
* Table Updated  :db_update 
* Input          :*mb_master 主站
* Output         :none 
* Return         :none
******************************************************************************/
void *ModbusMasterUpdate(void *arg) 
{
  mb_master_s *mb_master =(mb_master_s*)arg;
  mb_comm_dev_s *p_dev = mb_master->head;
  mb_slaver_s *mb_slaver;
  uint8_t com = mb_master->comport.uartid;

  DEBUG("主站%d Update 线程启动，开始等待升级...\n",mb_master->index);

  while(1)
  {
    sem_wait(&(mb_master->update_sem));
    DEBUG("(com%d) update获取升级信号量 ,start_flag = %d\n", com, mb_master->update->start_flag);
     
    sem_wait(&(mb_master->bus_lock));
    DEBUG("(com%d) update获取总线\n", com);
     
    //清空端口的所有队列报文
    clear_master_all_cmd(mb_master);

    //升级处理
    update_process(mb_master->update);

    //清空升级内存
    clear_update_memory(mb_master); 

    //升级结束,释放总线
    sem_post(&(mb_master->bus_lock)); 
    printf("(com%d) update释放总线 !!!\n", com); 

    //恢复主站中从站初始化报文 
    int i;
    for (i = 0; i < mb_master->num; i++)
    {
      mb_slaver = p_dev->dev;
      (mb_slaver->ModbusRtuMaster_init)(mb_slaver);
      p_dev = p_dev->next;                  
    }  
  }
}



/****************************************************************************** 
* Function       :ModbusMasterSgu
* Author         :luoj
* Date           :2016-8-1
* Description    :监听线程，定时1min检索数据库
* Calls          :none
* Table Accessed :db_search_param 
* Table Updated  :none 
* Input          :none
* Output         :none 
* Return         :none
******************************************************************************/
void *ModbusMasterSgu(void *arg) 
{

  DEBUG("SGU 线程启动...\n");

  while(1)
  {
    sem_wait(&sgu_module.sgu_sem);
    DEBUG("获取sgu信号量 \n");

    analy_sgu_file();
    db_update_sgu_state(sgu_module.sgu_state);
    printf("sgu_state(%d)写入db!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", sgu_module.sgu_state);
  }
}