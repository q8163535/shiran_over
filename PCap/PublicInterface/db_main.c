#include <stdio.h>
#include <string.h>

#include "db.h"
#define UPDATE_DB "./test_db_data"
#define UPDATE_FILE_PATH "Update/"
sqlite3  *db_update;
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
  char *sql = "CREATE TABLE IF NOT EXISTS UPDATEPARAM("  
  "ID integer PRIMARY KEY autoincrement ,"   
  "DATATIME       TEXT," 
  "COM            TEXT UNIQUE,"    
  "START_FLAG     BLOB,"    
  "FINISH_FLAG    BLOB," 
  "UPDATE_TYPE    INT,"   
  "TOT_NUM      INT,"   
  "FILE_VER       TEXT,"               
  "ADDR           TEXT," 
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

int main()
{
  db_creat_table_update_info();
  db_creat_table_web_savefile();
}