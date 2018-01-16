/*************************************************
FileName:     communi_can.h
Copyright:    GoCom
Link:         communi_can.c
History:
**************************************************/

#ifndef _COMMUNI_CAN_H
#define _COMMUNI_CAN_H

int canInit();
void OnTimer_can(void);

extern unsigned int sbPosition;
extern unsigned int lastsbPosition;


extern unsigned int mdXinhao;
extern float jcSpeed;
extern unsigned char bCanCommStatus;

extern float zdguanPressure;    //制动管压
extern float zdgangPressure;    //制动缸压
extern float jcSpeed;          //机车速度
extern float cyjSpeed;



//extern int gint_limitSpeed;

// extern int gint_nShiftNo;    //运行里程距离
// extern int gint_nCurRunLen ;  // 公里标
// extern int gint_nSumRunLen ; // 累计

#endif
