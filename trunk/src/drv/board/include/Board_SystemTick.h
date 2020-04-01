#ifndef BOARD_SYSTEMTICK_H
#define BOARD_SYSTEMTICK_H
// ==============================================================================
// FILE NAME: BOARD_SYSTEMTICK.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 09/01/2013, Leohong written
// --------------------
// ==============================================================================

#include "CommonDef.h"
#include "GenericTypeDefs.h"

typedef enum
{
    eBOARD_SYSTICK_COUNTER = 0,
    eBOARD_SYSTICK_FANCOUNTER,

    eBOARD_SYSTICK_NUMBERS,
} eBOARD_SYSTICK_TYPE;

void Board_SysTick_Init(void);
void Board_SysTick_DeInit(void);
DWORD Board_SysTick_Val_Get(void);

void Board_RegSysTick_Callback(eBOARD_SYSTICK_TYPE eType, void (*fpCallback)(void));

#endif //BOARD_SYSTEMTICK_H

