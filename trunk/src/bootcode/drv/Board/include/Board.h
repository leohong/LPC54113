#ifndef BOARD_H
#define BOARD_H
// ==============================================================================
// FILE NAME: BOARD.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 12/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"
#include "Board_Timer.h"

extern const sBOARD_TIMER_CFG m_sRpmCountCfg;

typedef enum
{
    eINT_PRIORITY_LEVEL_0 = 0, // HIGH
    eINT_PRIORITY_LEVEL_1,
    eINT_PRIORITY_LEVEL_2,
    eINT_PRIORITY_LEVEL_3,
    eINT_PRIORITY_LEVEL_4,
    eINT_PRIORITY_LEVEL_5,
    eINT_PRIORITY_LEVEL_6,
    eINT_PRIORITY_LEVEL_7,  //LOW

    eINT_PRIORITY_NUMBERS,
} eINT_PRIORITY;

void Board_Init(void);
void Board_CommMode_On_ReInit(void);
void Board_CommMode_Off_ReInit(void);
void Board_SystemOperationClock(void);
void Board_PowerOn_ReInit(void);
void Board_PowerOff_ReInit(void);
void Board_Goto_BL_DeInit(void);

#endif /* BOARD_H */
