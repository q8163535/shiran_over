
/*************************************************
FileName:     infoplat-MiniGUIMain3.h
Copyright:    GoCom
Link:         infoplat-MiniGUIMain3.c
History:
**************************************************/
#ifndef _INFOPLATMINIGUIMAIN3_H
#define _INFOPLATMINIGUIMAIN3_H

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>


int MiniGUIMain3 (int argc, const char* argv[]);
void buttonInit3(HWND* phWnd);


void tableInit3(HWND* phWnd, HDC *hdc);
void textInit3(HWND* phWnd, HDC *hdc);
void textLabelInit3(HWND* phWnd, HDC *hdc);

void taskTableInit3(HWND* phWnd);


void infoWindowInit(HWND* phWnd);


void alarmWindowInit(HWND* phWnd);

void InitSystemForNG();


void SavePlanForNG(char *PlanBuf,unsigned short count);

void FillPlanTableForNG(char *lpbPlanRecvBuffer,unsigned short m_nPlanDataSizeCounter);


#endif