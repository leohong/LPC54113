// ===============================================================================
// FILE NAME: main.c
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2019/03/24, Leo Create
// --------------------
// ===============================================================================


#include "board.h"
#include "halBoardCtrlAPI.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"
#include "appIapProcAPI.h"

/* Structure that will hold the TCB of the task being created. */
StaticTask_t m_xAppMainTaskTCB;

/* Buffer that the task being created will use as its stack. */
StackType_t m_axAppMainStack[APP_MAIN_TASK_STACK_SZ];

/* Sets up system hardware */
// ==============================================================================
// FUNCTION NAME: prvSetupHardware
// DESCRIPTION:
//
//
// PARAMETERS:
// void:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/03/22, Leo Create
// --------------------
// ==============================================================================
static void prvSetupHardware(void)
{
    __disable_irq();

    // FreeRTOS needs system clock for system tick.
    SystemCoreClockUpdate();
}

// ==============================================================================
// FUNCTION NAME: main
// DESCRIPTION:
//
//
// PARAMETERS:
// void:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/03/22, Leo Create
// --------------------
// ==============================================================================
int main(void)
{
    prvSetupHardware();

    appIapProc_Init();

    return 0;
}

