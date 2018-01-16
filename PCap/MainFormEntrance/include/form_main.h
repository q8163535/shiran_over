/*************************************************
FileName:     form_main.h
Copyright:    GoCom
Link:         form_main.c
History:
**************************************************/
#ifndef _FORM_MAIN_H
#define _FORM_MAIN_H

#include <minigui/common.h>

void refreshRecordprogress( );

typedef struct _ST_PLANDATA
{
    char order[4];           //序号
    char stationLine[10];    //股道
    char stationWork[10];    //作业
    char num[3];    //辆数
    char remark[30];    //备注
}ST_PLANDATA, *PST_PLANDATA;

typedef struct _ST_CURPLAN
{
    char num;               //勾数
    char date[14];          //时间
    char planNum[6];        //计划号
    char versionNum[4];     //版本号
    char unKnown1[6];       //未知数据1
    char unKnown2[6];       //未知数据2
    char unKnown3[4];       //未知数据3
    char unKnown4[4];       //未知数据4
    ST_PLANDATA st_planData[30];
}ST_CURPLAN, *PST_CURPLAN;

extern char gchar_planNo[12];   //计划编号
extern int gint_versionNo;   //版本号
extern int gint_leftHook;
extern int gint_limitSpeed;  //限速



extern BOOL WDFlag;
extern BOOL WDFlag1;

void mainFormInit(HWND* phWnd);
void mainFormInitLogin(HWND* phWnd);

void mainFormInit2(HWND* phWnd);
void mainFormInit3(HWND* phWnd);
void tableInit(HWND* phWnd, HDC *hdc);
void textInit(HWND* phWnd, HDC *hdc);
void textLabelInit(HWND* phWnd, HDC *hdc);
void recordInit(HWND* phWnd);
void taskTableInit(HWND* phWnd);
void buttonInit(HWND* phWnd);
void configInit();

void refreshTime();
void refreshInfo(HWND hWnd);
void planTableInit();
void setLampColor(int lampNum, DWORD color);

extern int OnSpeeding(WPARAM wParam,LPARAM nMaxSpeed);

#endif
