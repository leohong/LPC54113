// ==============================================================================
// FILE NAME: BOARD_SYSTEMTICK.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 09/01/2013, Leohong written
// --------------------
// ==============================================================================


#include "Board_SystemTick.h"

#define CPU_MSEC(x) (((uint64_t)(x) * SystemCoreClock)/1000)
static void (*m_fpSystemTick_Callback[eBOARD_SYSTICK_NUMBERS])(void) = {NULL};

// ==============================================================================
// FUNCTION NAME: SYSTICK_HANDLER
// DESCRIPTION:
//
// 處理system tick產生之中斷
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 15/01/2013, Leohong written
// --------------------
// ==============================================================================
void SysTick_Handler(void)
{
    BYTE cSysTickCount = 0;

    for(cSysTickCount = 0; cSysTickCount < eBOARD_SYSTICK_NUMBERS; cSysTickCount++)
    {
        if(m_fpSystemTick_Callback[cSysTickCount] != NULL)
        {
            m_fpSystemTick_Callback[cSysTickCount]();
        }
    }
}

// ==============================================================================
// FUNCTION NAME: BOARD_SYSTICK_INIT
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 15/01/2013, Leohong written
// --------------------
// ==============================================================================
void Board_SysTick_Init(void)
{
    /* Update SystemCoreClock variable */
    SystemCoreClockUpdate();

    /* Generate interrupt each 1 ms      */
    SysTick_Config(CPU_MSEC(1));
}

// ==============================================================================
// FUNCTION NAME: BOARD_SYSTICK_DEINIT
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 22/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_SysTick_DeInit(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

// ==============================================================================
// FUNCTION NAME: BOARD_SYSTICK_VAL_GET
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 29/11/2013, Leo Create
// --------------------
// ==============================================================================
DWORD Board_SysTick_Val_Get(void)
{
    return SysTick->VAL;
}

// ==============================================================================
// FUNCTION NAME: BOARD_REGSYSTICK_CALLBACK
// DESCRIPTION:
//
//
// Params:
// eBOARD_SYSTICK_TYPE eType:
// void (*fpCallback)(void):
//
// Returns:
//
//
// modification history
// --------------------
// 01/01/2014, Leo Create
// --------------------
// ==============================================================================
void Board_RegSysTick_Callback(eBOARD_SYSTICK_TYPE eType, void (*fpCallback)(void))
{
    m_fpSystemTick_Callback[eType] = fpCallback;
}

