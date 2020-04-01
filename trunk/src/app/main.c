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
#include "appSysmon.h"

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
    __enable_irq();

    // FreeRTOS needs system clock for system tick.
    SystemCoreClockUpdate();
    //halBoard_Init();
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

    /* System Monitor thread */
    xTaskCreateStatic(appMain_Task,
                      "Main_Task",
                      APP_MAIN_TASK_STACK_SZ,
                      NULL,
                      APP_MAIN_TASK_PRIORITY,
                      m_axAppMainStack,
                      &m_xAppMainTaskTCB);

    vTaskStartScheduler();

    return 0;
}

