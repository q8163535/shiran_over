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
    char order[4];           //���
    char stationLine[10];    //�ɵ�
    char stationWork[10];    //��ҵ
    char num[3];    //����
    char remark[30];    //��ע
}ST_PLANDATA, *PST_PLANDATA;

typedef struct _ST_CURPLAN
{
    char num;               //����
    char date[14];          //ʱ��
    char planNum[6];        //�ƻ���
    char versionNum[4];     //�汾��
    char unKnown1[6];       //δ֪����1
    char unKnown2[6];       //δ֪����2
    char unKnown3[4];       //δ֪����3
    char unKnown4[4];       //δ֪����4
    ST_PLANDATA st_planData[30];
}ST_CURPLAN, *PST_CURPLAN;

extern char gchar_planNo[12];   //�ƻ����
extern int gint_versionNo;   //�汾��
extern int gint_leftHook;
extern int gint_limitSpeed;  //����



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
