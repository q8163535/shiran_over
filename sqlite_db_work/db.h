#ifndef _DB_H_
#define _DB_H_
#include "io.h"
#include <sqlite3.h> 

#define HISDATA_MAX_NUM 21024
#define HISRECORD_MAX_NUM 1000
#define HISPDAY_MAX_TABLE_NUM 731

#define HISEYEAR_DB "db_his_Eyear"
#define HISEMONTH_DB "db_his_Emonth"
#define HISEDAY_DB "db_his_Eday"
#define HISPDAY_DB "db_his_Pday"


typedef int (*DB_Callback)(void *data, int argc, char **argv, char **azColName);

void db_init(void);
int32_t db_inset_his_record_event(mb_slaver_s *mb_slaver,uint16_t name,data_u old_data, uint8_t calltype);
int32_t db_inset_his_record_err(mb_slaver_s *mb_slaver,uint16_t name,data_u old_data,uint8_t calltype);
void clear_db(void);

void db_open(const char *path,sqlite3 **db);
int db_done(char *sql, sqlite3 *db, DB_Callback Fcallback, char *data);
void db_operate(const char *path, char *sql,sqlite3 *db);
void db_close(sqlite3 *db);
int db_commit(sqlite3 *db);
int db_begin_trans(sqlite3 *db);
int db_sel_callback(void *data, int argc, char **argv, char **azColName);
#endif 
