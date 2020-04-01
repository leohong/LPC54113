#ifndef BOARD_WATCHDOG_H
#define BOARD_WATCHDOG_H
// ===============================================================================
// FILE NAME: Board_Watchdog.h
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2015/03/19, Leo Create
// --------------------
// ===============================================================================


#include "CommonDef.h"

void Board_WatchDog_Init(void);
void Board_WatchDog_Feed(void);
void Board_WatchDog_Set_Timeout(DWORD dwMilliSecond);
void Board_WatchDog_Reset_Enable(BYTE cEnable);

#endif /* BOARD_WATCHDOG_H */

