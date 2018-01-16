/*************************************************
FileName:     infoplat-MiniGUIMain2.h
Copyright:    GoCom
Link:         infoplat-MiniGUIMain2.c
History:
**************************************************/

#ifndef _INFOPLATMINIGUIMAIN2_H
#define _INFOPLATMINIGUIMAIN2_H

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>



#define FILE_BITMAP_GREEN     ("/infoplat/pic/green.bmp" )
#define FILE_BITMAP_RED       ("/infoplat/pic/red.bmp" )
#define FILE_BITMAP_LAMP_BLUE ("/infoplat/pic/blueLamp.gif")
#define FILE_BITMAP_LAMP_RED  ("/infoplat/pic/redLamp.gif")
#define FILE_BITMAP_LAMP_GREEN ("/infoplat/pic/greenLamp.gif")
#define FILE_BITMAP_LAMP_WHITE ("/infoplat/pic/whiteLamp.gif")




int MiniGUIMain2 (int argc, const char* argv[]);


void tableInit2(HWND* phWnd, HDC *hdc);
void textInit2(HWND* phWnd, HDC *hdc);
void textLabelInit2(HWND* phWnd, HDC *hdc);

void taskTableInit2(HWND* phWnd);
void buttonInit2(HWND* phWnd);
void configInit();

void loginInitRight2(void);
/*�ƻ����ݳ�ʼ��*/
void planTableInitGis();



/*GPS����״̬*/
extern int gfl_communiStatusGPS;    //ͨ��״̬

/*��̨����״̬*/
extern int gfl_communiStatusDD;    //ͨ��״̬



#endif
