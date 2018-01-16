#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <sqlite3.h> 
#include <errno.h> 
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#include "io.h"
#include "db.h"


typedef struct db_db_last_time_st
{
  uint16_t year;
  uint16_t month;
  uint16_t date;
  uint16_t hour;
  uint16_t min;
  uint16_t sec; 
  uint32_t t_sec;
  uint32_t d_sec;
}db_db_last_time_s;

db_db_last_time_s db_last_time;
sem_t sem_his_data,sem_his_energe;
sqlite3  *db_his_record,*db_his_data,*db_his_Eyear,*db_his_Emonth,*db_his_Eday,*db_his_Pday;
char strSQL[256];
pthread_t timing_store_ptd[3];



//数据库回调函数
int db_sel_callback(void *data, int argc, char **argv, char **azColName)
{
  strcpy((char *)data,argv[0]);

  return 0;
}

//获得四舍五入的值
float get_round(float value, uint8_t bit)
{
  float tmp; 

  if(bit == 1)
  {
    tmp = value >0? (value-0.05):(value+0.05);
  }
  else if(bit == 3)
  {
    tmp = value >0? (value-0.0005):(value+0.0005);
  }
  return tmp;
}


//打开数据库
void db_open(const char *path,sqlite3 **db)
{
 int rc;

 rc = sqlite3_open(path, db);
 if( rc )
 {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
    exit(0);
 }
}


int busy_callback(void* lpVoid,int c)  
{  
  char *sql = (char *)lpVoid;

  usleep(500);

  if(c >= 10000)
  { 
    printf("sql_busy_handler 超时5s, 退出 ！！ @ %s\n", sql);
    return 0;       
  }
  else
  {
  //  printf("sql_busy_handler cnt = %d @ %s\n", c, sql);
    return 1;   
  }
}

//执行数据库操作语句,带回调函数
int  db_done(char *sql, sqlite3 *db, DB_Callback Fcallback, char *data)
{
  char *zErrMsg = NULL;
  int rc;

  sqlite3_busy_handler(db, busy_callback,(void *)sql);  //等待锁

  if ((Fcallback != NULL)&&(data != NULL))
  {
    rc = sqlite3_exec(db, sql, Fcallback, data, &zErrMsg);
  }
  else
  {
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg); 
  }

  if( rc != SQLITE_OK )
  {
    fprintf(stderr, "SQL done error: %s, %s\n", zErrMsg, sql);
    sqlite3_free(zErrMsg);
  }

  return rc;
}

int db_begin_trans(sqlite3 *db)
{
  char *zErrMsg = NULL;
  int ret = sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, &zErrMsg);

  if (ret != SQLITE_OK) 
  {
    printf("SQL excutive error: %s, begin transaction\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  return ret;
}


int db_commit(sqlite3 *db)
{
  char *zErrMsg = NULL;
  int ret = sqlite3_exec(db, "COMMIT TRANSACTION", 0, 0, &zErrMsg);

  if (ret != SQLITE_OK) 
  {
    printf("SQL excutive error: %s, commit transaction\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  return ret;
}

void db_close(sqlite3 *db)
{
  if(db != NULL)
  {
    sqlite3_close(db);
  }
}


//数据库通用操作接口，不带回调函数
void db_operate(const char *path, char *sql,sqlite3 *db)
{

  db_open(path, &db);

  db_done(sql, db, NULL,NULL);

  db_close(db);
}


//建立历史数据表格
/*
void db_creat_table_his_data(void)
{ 
  char *sql = "CREATE TABLE IF NOT EXISTS HISDATA("  \
              "ID integer PRIMARY KEY autoincrement ," \
              "DATATIME       TEXT," \
              "DATATIME_T     INTEGER," \
              "P_AC           REAL," \
              "Q_AC           REAL," \
              "E_DAY          REAL," \
              "E_MON          INT," \
              "E_YEAR         INT," \
              "E_TOTAL        INT," \
              "STATE          INT," \
              "V_DC           REAL," \
              "I_DC           REAL," \
              "V_AC           REAL," \
              "I_AC           REAL," \
              "F_AC           REAL," \
              "T_INV          REAL," \
              "R_ISO_P        REAL," \
              "R_ISO_N        REAL," \
              "POWER_RELAY    INT," \
              "MINUTES_RUN    INT," \
              "HOURS_RUN      INT);";
  db_operate(HISDATA_DB,sql,db_his_data);
  
}

//建立历史记录表格
void db_creat_table_his_record(void)
{ 
  
  char *sql = "CREATE TABLE IF NOT EXISTS HISRECORD("  \
              "ID integer PRIMARY KEY autoincrement ," \
              "DATATIME               TEXT," \
              "DATATIME_T             INTEGER," \
              "TYPE                   INT," \
              "RECORD                 INT," \
              "STATE                  INT);";

  db_operate(HISRECORD_DB,sql,db_his_record);
}

//建立Eyear表格
void db_creat_table_his_Eyear(void)
{ 
  
  char *sql = "CREATE TABLE IF NOT EXISTS HISEYEAR("  \
              "ID integer PRIMARY KEY autoincrement ," \
              "YEAR                   INT," \
              "EYEAR                  REAL);";

  db_operate(HISEYEAR_DB,sql,db_his_Eyear);
  
}


//建立Emonth表格
void db_creat_table_his_Emonth(void)
{ 
  
  char *sql = "CREATE TABLE IF NOT EXISTS HISEMONTH("  \
              "ID integer PRIMARY KEY autoincrement ," \
              "YEAR                 INT," \
              "MONTH                INT," \
              "EMONTH               REAL);";

  db_operate(HISEMONTH_DB,sql,db_his_Emonth);
  
}

//建立Eday表格
void db_creat_table_his_Eday(void)
{ 
  
  char *sql = "CREATE TABLE IF NOT EXISTS HISEDAY("  \
              "ID integer PRIMARY KEY autoincrement ," \
              "YEAR                 INT," \
              "MONTH                INT," \
              "DAY                  INT," \
              "EDAY                 REAL);";
  db_operate(HISEDAY_DB,sql,db_his_Eday);
 
}

//建立pday索引表格
void db_creat_table_his_Pdayinf(void)
{
  
  memset(strSQL,0,256);
  sprintf(strSQL,"CREATE TABLE IF NOT EXISTS HISPDAYINF("  \
                  "ID integer PRIMARY KEY autoincrement ," \
                  "HISPDAYNAME            TEXT);"); 

  db_operate(HISPDAY_DB,strSQL,db_his_Pday);
  
}

*/


//建立Pday表格
/*
void db_creat_table_his_Pday(uint16_t year_t,uint16_t month_t,uint16_t date_t,uint16_t dsec_t)
{ 
  
  char strtmp[256] = {0};
  char PdayTableName[256] = {0};

  db_open(HISPDAY_DB,&db_his_Pday);

  sprintf(PdayTableName,"HISPDAY%04d%02d%02d" ,\
                  year_t,
                  month_t,
                  date_t);

  memset(strSQL,0,256);
  sprintf(strSQL,"SELECT count(*) as rows FROM HISPDAYINF WHERE HISPDAYNAME = '%s';",\
                  PdayTableName);

  db_done(strSQL, db_his_Pday,db_sel_callback,strtmp);//查找索引表里是否有已存在的日功率曲线表记录

  if (atoi(strtmp)==0)
  {
    memset(strSQL,0,256);
    sprintf(strSQL,"INSERT INTO HISPDAYINF VALUES (NULL ,'%s');",\
                    PdayTableName);   
    db_done(strSQL, db_his_Pday,NULL,NULL);//不存在建立记录
  }

  memset(strSQL,0,256);
  sprintf(strSQL,"SELECT SEQ FROM SQLITE_SEQUENCE where name = 'HISPDAYINF';");   
  db_done(strSQL, db_his_Pday,db_sel_callback,strtmp);//检查索引表的记录条数

  uint32_t his_Pday_tab_id = atoi(strtmp);  
  printf("his_Pday_tab_id = %d\n", his_Pday_tab_id);

  if (his_Pday_tab_id > HISPDAY_MAX_TABLE_NUM)//超过2年
  {
    memset(strSQL,0,256);
    sprintf(strSQL,"SELECT HISPDAYNAME FROM HISPDAYINF where id = %d"  ,\
                    (his_Pday_tab_id - HISPDAY_MAX_TABLE_NUM)); 

    db_done(strSQL, db_his_Pday,db_sel_callback,PdayTableName);//查找索引表最老的记录对应的功率曲线表格名（找不到怎么办？）

    memset(strSQL,0,256);
    sprintf(strSQL,"DROP TABLE %s; ", PdayTableName);//清除表格
    db_done(strSQL, db_his_Pday,NULL,NULL);

    memset(strSQL,0,256);
    sprintf(strSQL,"DELETE from HISPDAYINF where id = %d"  ,\
                    (his_Pday_tab_id - HISPDAY_MAX_TABLE_NUM)); 

    db_done(strSQL, db_his_Pday,NULL,NULL);//删除索引表最老的记录   

  }

  memset(strSQL,0,256);
  sprintf(strSQL,"CREATE TABLE IF NOT EXISTS HISPDAY%04d%02d%02d("  \
                  "ID integer PRIMARY KEY autoincrement ," \
                  "YEAR                 INT," \
                  "MONTH                  INT," \
                  "DAY                  INT," \
                  "DAYSEC                 INT," \
                  "PDAY                   REAL);" ,\
                  year_t,
                  month_t,
                  date_t);  

  db_done(strSQL, db_his_Pday,NULL,NULL);


  memset(strSQL,0,256);
  sprintf(strSQL,"DELETE from HISPDAY%04d%02d%02d where DAYSEC > %d"  ,\
                  year_t,
                  month_t,
                  date_t,
                  dsec_t);  

  db_done(strSQL, db_his_Pday,NULL,NULL);

  sqlite3_close(db_his_Pday);
  
}
*/

//插入历史发电量/功率
/*
int32_t db_inset_his_energe(mb_slaver_s *mb_slaver)
{
  
  struct tm* local;
  time_t t;
  char strtmp[256] = {0};

  sem_wait(&sem_his_energe);

  t = time(NULL);
  
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }

  local = localtime(&t);

  db_open(HISEYEAR_DB,&db_his_Eyear);
  db_open(HISEMONTH_DB,&db_his_Emonth);
  db_open(HISEDAY_DB,&db_his_Eday);

  //printf("energy store!!!\n");
  //--------------------------------------------------------------------------------------------
  //printf("%d-%d-%d\n",(local->tm_year + 1900),(local->tm_mon + 1),local->tm_mday);
  memset(strSQL,0,256);
  sprintf(strSQL,"SELECT count(*) as rows FROM HISEYEAR WHERE YEAR = %04d; ",(local->tm_year + 1900));

  db_done(strSQL, db_his_Eyear,db_sel_callback,strtmp);

  if (atoi(strtmp)>0)
  {

    sprintf(strSQL,"UPDATE HISEYEAR SET EYEAR = %.1f WHERE YEAR = %04d; " , 
                    get_round((float)mb_slaver->data[E_YEAR].s32/1000, 1),
                    (local->tm_year + 1900)); //已经存在相同时间戳则更新

  }
  else
  {
    memset(strSQL,0,256);
    sprintf(strSQL,"INSERT INTO HISEYEAR VALUES (NULL , %04d, %.1f); " ,\
                    (local->tm_year + 1900),
                    get_round((float)mb_slaver->data[E_YEAR].s32/1000, 1));//没有相同时间戳则插入
  }
//  printf("E_YEAR = %.1f \n", get_round((float)mb_slaver->data[E_YEAR].s32/1000, 1));
  db_done(strSQL, db_his_Eyear,NULL,NULL);//插入年发电量

  //--------------------------------------------------------------------------------------------
  memset(strSQL,0,256);
  sprintf(strSQL,"SELECT count(*) as rows FROM HISEMONTH WHERE YEAR = %04d AND MONTH = %02d; ",\
                  (local->tm_year + 1900),(local->tm_mon + 1));

  db_done(strSQL, db_his_Emonth,db_sel_callback,strtmp);

  if (atoi(strtmp)>0)
  {
    memset(strSQL,0,256);
    sprintf(strSQL,"UPDATE HISEMONTH SET EMONTH = %.1f WHERE YEAR = %04d AND MONTH = %02d; " ,\
                    get_round((float)mb_slaver->data[E_MON].s32, 1),
                    (local->tm_year + 1900),
                    (local->tm_mon + 1)); 
    //printf("UPDATE EMONTH!!!\n");
  }
  else
  {
    memset(strSQL,0,256);
    sprintf(strSQL,"INSERT INTO HISEMONTH VALUES (NULL , %04d, %02d, %.1f); " ,\
                    (local->tm_year + 1900),
                    (local->tm_mon + 1),
                    get_round((float)mb_slaver->data[E_MON].s32, 1));
    //printf("INSERT EMONTH!!!\n");
  }
 // printf("E_MON = %.1f\n", get_round((float)mb_slaver->data[E_MON].s32, 1));
  db_done(strSQL, db_his_Emonth,NULL,NULL);//插入月发电量

  //--------------------------------------------------------------------------------------------
  memset(strSQL,0,256);
  sprintf(strSQL,"SELECT count(*) as rows FROM HISEDAY WHERE YEAR = %04d AND MONTH = %02d AND DAY = %02d; ",\
                  (local->tm_year + 1900),(local->tm_mon + 1),local->tm_mday);

  db_done(strSQL, db_his_Eday,db_sel_callback,strtmp);

  if (atoi(strtmp)>0)
  {
    memset(strSQL,0,256);
    sprintf(strSQL,"UPDATE HISEDAY SET EDAY = %.1f WHERE YEAR = %04d AND MONTH = %02d AND DAY = %02d; " ,\
                    get_round(mb_slaver->data[E_DAY].f32, 1),
                    (local->tm_year + 1900),
                    (local->tm_mon + 1),
                    local->tm_mday);  
    //printf("UPDATE EDAY=%f!!!\n",mb_slaver->data[E_DAY].f32);
  }
  else
  {
    memset(strSQL,0,256);
    sprintf(strSQL,"INSERT INTO HISEDAY VALUES (NULL , %04d, %02d, %02d, %.1f); " ,\
                    (local->tm_year + 1900),
                    (local->tm_mon + 1),
                    local->tm_mday,
                    get_round(mb_slaver->data[E_DAY].f32, 1));
    //printf("INSERT EDAY!!!\n");
  }
 // printf("E_DAY = %.1f\n",  get_round(mb_slaver->data[E_DAY].f32, 1));
  db_done(strSQL, db_his_Eday,NULL,NULL);//插入日发电量


  uint32_t tmp_dsec = (local->tm_hour*60 + local->tm_min)*60 + local->tm_sec;
  //----------------------------------------------------------------------------------------------
  if ((db_last_time.year!=(local->tm_year + 1900))||
      (db_last_time.month!=(local->tm_mon + 1))||
      (db_last_time.date!=local->tm_mday))//发生天切换
  {
    db_creat_table_his_Pday((local->tm_year + 1900),(local->tm_mon + 1),local->tm_mday,
                            tmp_dsec);
  }
  else if (tmp_dsec < db_last_time.d_sec)//当前时间比数据库记录的最后一条时间戳古老
  {
    memset(strSQL,0,256);
    sprintf(strSQL,"DELETE from HISPDAY%04d%02d%02d where DAYSEC > %d"  ,\
                  db_last_time.year,
                  db_last_time.month,
                  db_last_time.date,
                  tmp_dsec);  

    db_operate(HISPDAY_DB,strSQL,db_his_Pday);//清除掉比当前时间还要新的记录
  }

  memset(strSQL,0,256);
  sprintf(strSQL,"INSERT INTO HISPDAY%04d%02d%02d VALUES (NULL , %04d, %02d, %02d, %d, %.3f); " ,\
                (local->tm_year + 1900),
                (local->tm_mon + 1),
                local->tm_mday,                     
                (local->tm_year + 1900),
                (local->tm_mon + 1),
                local->tm_mday,
                tmp_dsec,
                get_round(mb_slaver->data[PT].f32, 3));
  
  db_operate(HISPDAY_DB,strSQL,db_his_Pday);
 // printf("PT = %.3f\n", get_round(mb_slaver->data[PT].f32, 3));

  db_last_time.year = (local->tm_year + 1900);
  db_last_time.month = (local->tm_mon + 1);
  db_last_time.date = local->tm_mday;
  db_last_time.hour = local->tm_hour;
  db_last_time.min = local->tm_min;
  db_last_time.sec = local->tm_sec;
  db_last_time.t_sec = t;
  db_last_time.d_sec = tmp_dsec;

  sqlite3_close(db_his_Eyear);
  sqlite3_close(db_his_Emonth);
  sqlite3_close(db_his_Eday);

  sem_post(&sem_his_energe);  

  return 0;
}
*/

//插入历史数据
/*
int32_t db_inset_his_data(mb_slaver_s *mb_slaver)
{
  struct tm* local;
  time_t t;

  sem_wait(&sem_his_data);

  t = time(NULL);

  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  //get local current time and date
  local = localtime(&t);

  HMI_cfg.data.his_data_num++;
 // printf("his_data_num = %d\n", HMI_cfg.data.his_data_num);

  hmi_do_cfg_store();

  db_open(HISDATA_DB,&db_his_data);

  if (HMI_cfg.data.his_data_num > HISDATA_MAX_NUM)//满2年开始先进先出
  {
 //   printf("dddddddd\n");
    memset(strSQL,0,256);
    sprintf(strSQL,"DELETE from HISDATA where ID = %d"  ,\
                  (HMI_cfg.data.his_data_num-HISDATA_MAX_NUM)); 
    db_done(strSQL, db_his_data,NULL,NULL);   
  }

  memset(strSQL,0,256);
  sprintf(strSQL,"INSERT INTO HISDATA VALUES (NULL ,'%04d-%02d-%02d %02d:%02d:%02d', %d, %f, %f, %f, %d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %d, %d, %d); " ,\
                  (local->tm_year + 1900),
                  (local->tm_mon + 1),
                  local->tm_mday,
                  local->tm_hour,
                  local->tm_min,
                  local->tm_sec,
                  (int)t,
                  mb_slaver->data[PT].f32,
                  mb_slaver->data[QTOT].f32,
                  mb_slaver->data[E_DAY].f32,
                  mb_slaver->data[E_MON].s32,
                  mb_slaver->data[E_YEAR].s32,
                  mb_slaver->data[E_TOT].s32,
                  mb_slaver->data[STATE_LCD].s32,
                  mb_slaver->data[VDC1].f32,
                  mb_slaver->data[IDC1].f32,
                  mb_slaver->data[VAB].f32,
                  mb_slaver->data[IA].f32,
                  mb_slaver->data[F].f32,
                  mb_slaver->data[TEMPSHELL].f32,
                  mb_slaver->data[P_R].f32,
                  mb_slaver->data[N_R].f32,
                  ((mb_slaver->data[NODE2].s32>>16)&0x01),
                  mb_slaver->data[TRUN_DAY].s32,
                  mb_slaver->data[T_TOT].s32);

  db_done(strSQL, db_his_data,NULL,NULL);

  sqlite3_close(db_his_data);

  sem_post(&sem_his_data);
  
  return 0;
}
*/

//插入历史事件
/*
int32_t db_inset_his_record_event(mb_slaver_s *mb_slaver,uint16_t name,data_u old_data, uint8_t calltype)
{
  
  struct tm* local;
  time_t t;
  uint32_t type_tmp,record_tmp,state_tmp;

  //printf("db_inset_his_record_event\n");
  t = time(NULL);
  
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  //get local current time and date
  local = localtime(&t);

  if (mb_slaver->data[name].s32 != old_data.s32)
  {
    type_tmp = 1;
    record_tmp = mb_slaver->data[name].s32;
    state_tmp = 1;

    HMI_cfg.data.his_record_event_num += 1;
    hmi_do_cfg_store();

    db_open(HISRECORD_DB,&db_his_record);
      
    if (HMI_cfg.data.his_record_event_num > HISRECORD_MAX_NUM)//满HISRECORD_MAX_NUM条开始先进先出
    {
      memset(strSQL,0,256);
      sprintf(strSQL,"DELETE from HISRECORD where ID = %d"  ,\
                    (HMI_cfg.data.his_record_event_num-HISRECORD_MAX_NUM)); 
      db_done(strSQL, db_his_record,NULL,NULL);   
    }

    memset(strSQL,0,256);
    sprintf(strSQL,"INSERT INTO HISRECORD VALUES (NULL ,'%04d-%02d-%02d %02d:%02d:%02d', %d, %d, %d, %d);",\
                    (local->tm_year + 1900),
                    (local->tm_mon + 1),
                    local->tm_mday,
                    local->tm_hour,
                    local->tm_min,
                    local->tm_sec,
                    (int)t,
                    type_tmp,
                    record_tmp,
                    state_tmp);   
    db_done(strSQL, db_his_record,NULL,NULL);

    sqlite3_close(db_his_record);
  }   
  
  return 0; 
}
*/

//插入历史告警和故障
  /*
int32_t db_inset_his_record_err(mb_slaver_s *mb_slaver,uint16_t name,data_u old_data,uint8_t calltype)
{

  struct tm* local;
  time_t t;
  uint32_t type_tmp,type_tmp1,record_tmp,err_tmp,old_err_tmp;
  int32_t state_tmp;

  t = time(NULL);
  
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  //get local current time and date
  local = localtime(&t);

  err_tmp = mb_slaver->data[name].s32;
  old_err_tmp = old_data.s32;

  db_open(HISRECORD_DB,&db_his_record);

  if ((name == ERR1)||(name == ERR2)||(name == ERR3))
  {
    type_tmp = 3;
  }
  // else if ((name = waring1)||(name = waring1))
  // {
  //  type_tmp = 2;
  // }
  
  if (err_tmp != old_err_tmp)
  {
    int i = 0;
    for (i = 0; i < 32; ++i)
    {
      type_tmp1 = type_tmp;
      state_tmp = -1;
      if(((err_tmp>>i)&0x01)&&(!((old_err_tmp>>i)&0x01)))
      {
        state_tmp = 1;
        HMI_cfg.data.his_record_event_num++;
        if(HMI_cfg.data.his_record_event_num % 500 == 0)
        {
          printf("his_record_event_num = %d \n", HMI_cfg.data.his_record_event_num);
        }

        hmi_do_cfg_store();
          
        if (HMI_cfg.data.his_record_event_num > HISRECORD_MAX_NUM )
        {
          memset(strSQL,0,256);
          sprintf(strSQL,"DELETE from HISRECORD where ID = %d"  ,\
                        (HMI_cfg.data.his_record_event_num-HISRECORD_MAX_NUM)); 

          db_done(strSQL, db_his_record,NULL,NULL);
        }         
      }
      else if(((old_err_tmp>>i)&0x01)&&(!((err_tmp>>i)&0x01)))
      {
        state_tmp = 0;
      }
      if(state_tmp > 0)//only store happen records 
      {
        if (name == ERR1)
        {
          record_tmp = i +1;   //luoj
        }
        else if (name == ERR2)
        {       
          switch (i)
          {
            case 11:
            case 12:
              record_tmp = i-11;
              type_tmp1 = 2;
              break;
            case 13:     //采样故障
              record_tmp = 46;
              break;
            case 14:     //控制电源异常
              record_tmp = 47;
              break;
            case 15:     //交流熔断器故障
              record_tmp = 48;
              break;
            case 16:     //LVRT运行
              record_tmp = 2;
              type_tmp1 = 2;          
              break;
            case 17:     //直流分量故障
              record_tmp = 49;
              break;
            case 18:
            case 19:
              record_tmp = i-15;
              type_tmp1 = 2;                
              break;
            case 20:     //直流防雷器故障
              record_tmp = 50;
              break;
            case 21:
            case 22:    
            case 23:  
              record_tmp = i-16;
              type_tmp1 = 2;                                  
              break;
            case 24:     //控制柜温度异常
              record_tmp = 51;
              break;
            case 25:
            case 26:    
            case 27:
            case 28:  
              record_tmp = i-17;
              type_tmp1 = 2;                      
              break;
            case 29:     //载波同步故障
              record_tmp = 52;
              break;
            case 30:
            case 31:
              record_tmp = i-18;
              type_tmp1 = 2;              
              break;
            case 17:
              record_tmp = 49;        
              break;
            case 20:
              record_tmp = 50;              
              break;
            case 24:
              record_tmp = 51;            
              break;
            case 29:
              record_tmp = 52;              
              break;                                                                                 
            default:
              record_tmp = i+32 +1;
              break;
          }
        }
        else if (name == ERR3)
        {
          switch (i)
          {
            case 1:
            case 2:
            case 3:   
            case 4:                 
              record_tmp = i+13;
              type_tmp1 = 2;
              break;
            case 6:             //交流空开异常
              record_tmp = 18;
              type_tmp1 = 2;          
              break;
            case 0:
              record_tmp = 53;   //交流开关故障  or 中点偏移     
              break;
            case 5:
              record_tmp = 54;   //交流开关故障         
              break;
            case 8:              //直流电压采样故障
              record_tmp = 56;            
              break;
            case 9:             //软启动故障
              record_tmp = 55;              
              break;                                                                                  
            default:
              record_tmp = i+64;
              break;
          }

        }
   
        memset(strSQL,0,256);
        sprintf(strSQL,"INSERT INTO HISRECORD VALUES (NULL ,'%04d-%02d-%02d %02d:%02d:%02d', %d, %d, %d, %d);",\
                        (local->tm_year + 1900),
                        (local->tm_mon + 1),
                        local->tm_mday,
                        local->tm_hour,
                        local->tm_min,
                        local->tm_sec,
                        (int)t,
                        type_tmp1,
                        record_tmp,
                        state_tmp);   

        db_done(strSQL, db_his_record,NULL,NULL);  
   
      }
    }
  } 
  sqlite3_close(db_his_record);
  

  return 0;     
}
*/


  /*
void clear_db(void)
{ 

  sem_wait(&sem_his_data);
  sem_wait(&sem_his_energe);

  memset(strSQL,0,256);
  sprintf(strSQL,"DROP TABLE HISRECORD; "); 
  db_operate(HISRECORD_DB,strSQL,db_his_record);//清空数据库内容，但是不释放存储空间

  memset(strSQL,0,256);
  sprintf(strSQL,"DROP TABLE HISDATA; "); 
  db_operate(HISDATA_DB,strSQL,db_his_data);

  memset(strSQL,0,256);
  sprintf(strSQL,"DROP TABLE HISEYEAR; ");  
  db_operate(HISEYEAR_DB,strSQL,db_his_Eyear);

  memset(strSQL,0,256);
  sprintf(strSQL,"DROP TABLE HISEMONTH; "); 
  db_operate(HISEMONTH_DB,strSQL,db_his_Emonth);

  memset(strSQL,0,256);
  sprintf(strSQL,"DROP TABLE HISEDAY; "); 
  db_operate(HISEDAY_DB,strSQL,db_his_Eday);

  remove(HISPDAY_DB);//删除数据库文件

  db_creat_table_his_record();
  db_creat_table_his_data();

  db_creat_table_his_Eyear();
  db_creat_table_his_Emonth();
  db_creat_table_his_Eday();

  db_creat_table_his_Pdayinf();
  db_creat_table_his_Pday(db_last_time.year,db_last_time.month,db_last_time.date,db_last_time.d_sec);

  HMI_cfg.data.his_data_num = 0;
  HMI_cfg.data.Pday_table_num = 0;
  HMI_cfg.data.his_record_event_num = 0;
  hmi_do_cfg_store();

  printf("clear db_his_record\n");

  sem_post(&sem_his_data);
  sem_post(&sem_his_energe);
}
*/


//定时存储线程
  /*
void *timing_store(void *arg) 
{


  struct timeval tempval;  
  int32_t tempval_rev;
  int i=0;

  mb_slaver_s *mb_slaver = mb_slaver_tab.head;
  for (i = 0; i < mb_slaver_tab.num; ++i)
  {
    if(strcmp(mb_slaver->name, "SGINV") == 0)
    {
      break;
    }
    mb_slaver = mb_slaver->next;
  }

  while(1)
  {
    tempval_rev = 60000*HMI_cfg.data.his_data_cycle;   //15分钟
    //tempval_rev = 6000;//test
    tempval.tv_sec = tempval_rev/1000;  //load timing
    tempval.tv_usec = 1000*(tempval_rev%1000);

    select(0, NULL, NULL, NULL, &tempval);
    db_inset_his_data(mb_slaver);     //历史数据
  }
  
}
*/



//定时存储线程
  /*
void *timing_store_3min(void *arg) 
{

  struct timeval tempval;  
  int32_t tempval_rev;
  int i;

  mb_slaver_s *mb_slaver = mb_slaver_tab.head;
  for (i = 0; i < mb_slaver_tab.num; ++i)
  {
    if(strcmp(mb_slaver->name, "SGINV") == 0)
    {
      break;
    }
    mb_slaver = mb_slaver->next;
  }

  while(1)
  {
    tempval_rev = 1800;   //3分钟
    //tempval_rev = 5000;//test
    tempval.tv_sec = tempval_rev/1000;  //load timing
    tempval.tv_usec = 1000*(tempval_rev%1000);

    select(0, NULL, NULL, NULL, &tempval);
    db_inset_his_energe(mb_slaver);   //历史发电量/功率
  }
  
}
*/

  /*
void *timing_store_1min(void *arg) 
{

  struct timeval tempval;  
  int32_t tempval_rev;
  int i;
  dev_data_t transmission[6];
  mb_slaver_s *mb_slaver = mb_slaver_tab.head;
  for (i = 0; i < mb_slaver_tab.num; ++i)
  {
    if(strcmp(mb_slaver->name, "SGINV") == 0)
    {
      break;
    }
    mb_slaver = mb_slaver->next;
  }
  while(1)
  {
    tempval_rev = 60000;   //1分钟定时下发时间
    tempval.tv_sec = tempval_rev/1000;  
    tempval.tv_usec = 1000*(tempval_rev%1000);
    select(0, NULL, NULL, NULL, &tempval);
    transmission[0].data_id = YEAR_LCD;
    transmission[1].data_id = MONTH_LCD;
    transmission[2].data_id = DATE_LCD;
    transmission[3].data_id = HOUR_LCD;
    transmission[4].data_id = MIN_LCD;
    transmission[5].data_id = SECOND_LCD;
    for(i=0; i< 6; i++)
    {
      transmission[i].data.s32 = mb_slaver->data[transmission[i].data_id].s32;
      transmission[i].data_type = S_INT_T;
    }
    create_cmd(&mb_master_tab,(uint8_t *)&transmission[0],sizeof(transmission));   
  }
}
*/

//建立定时记录历史数据的线程
/*
static void creat_timing_store_pthread(void)
{
  int32_t res;
  //void *thread_result;
  pthread_attr_t thread_attr;
  size_t stacksize;

  res = pthread_attr_init(&thread_attr);
  if (res != 0) {
    perror("Attribute creation failed");
    exit(EXIT_FAILURE);
  }
  stacksize = 1*1024;//512K
  res = pthread_attr_setstacksize(&thread_attr, stacksize);
    
  res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
  if (res != 0) {
    perror("Setting detached attribute failed");
    exit(EXIT_FAILURE);
  }

  res = pthread_create(&timing_store_ptd[0], &thread_attr, timing_store, NULL);
  if (res != 0) {
    perror("Thread timing_store creation failed");
    exit(EXIT_FAILURE);
  }

  res = pthread_create(&timing_store_ptd[1], &thread_attr, timing_store_3min, NULL);
  if (res != 0) {
    perror("Thread timing_store creation failed");
    exit(EXIT_FAILURE);
  } 
  res = pthread_create(&timing_store_ptd[2], &thread_attr, timing_store_1min, NULL);
  if (res != 0) {
    perror("Thread timing_store creation failed");
    exit(EXIT_FAILURE);
  } 

  (void)pthread_attr_destroy(&thread_attr);
  
}
*/

//数据库初始化,启动定时记录历史数据的线程
void db_init(void)
{
  /*
  struct tm* local;
  time_t t;

  sem_init(&sem_his_data, 0, 1);
  sem_init(&sem_his_energe, 0, 1);

  t = time(NULL);
  if (t == (time_t)-1)
  {
    fprintf(stderr,"Time error:%s\n",strerror(errno));
  }
  //get local current time and date
  local = localtime(&t);

  db_last_time.year = (local->tm_year + 1900);
  db_last_time.month = (local->tm_mon + 1);
  db_last_time.date = local->tm_mday;
  db_last_time.hour = local->tm_hour;
  db_last_time.min = local->tm_min;
  db_last_time.sec = local->tm_sec;
  db_last_time.t_sec = t;
  db_last_time.d_sec = ((local->tm_hour*60 + local->tm_min)*60 + local->tm_sec);

  db_creat_table_his_data();
  db_creat_table_his_record();
  db_creat_table_his_Eyear();
  db_creat_table_his_Emonth();
  db_creat_table_his_Eday();
  db_creat_table_his_Pdayinf();

  db_creat_table_his_Pday(db_last_time.year,db_last_time.month,db_last_time.date,db_last_time.d_sec);

  creat_timing_store_pthread();
  */
}




