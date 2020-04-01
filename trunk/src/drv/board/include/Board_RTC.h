#ifndef BOARD_RTC_H
#define BOARD_RTC_H
// ==============================================================================
// FILE NAME: BOARD_RTC.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 25/12/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"


void Board_RTC_Init(void);
BOOL Board_RTC_IS_Enable(void);
void Board_RTC_Set_Timer(RTC_TIME_T *psFullTime);
void Board_RTC_Get_Timer(RTC_TIME_T *psFullTime);
void Board_RTC_Test(void);

#endif /* BOARD_RTC_H */

