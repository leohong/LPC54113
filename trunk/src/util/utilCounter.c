// ==============================================================================
// FILE NAME: UTILCOUNTER.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 19/03/2013, Leohong written
// --------------------
// ==============================================================================


#include "utilCounterAPI.h"
#include "Board_RITimer.h"
#include "Board_GPIO.h"
#include "utilLogAPI.h"


static sTIMER_EVENT m_asTimerEvent[eTIMER_EVENT_TYPE_NUMBERS];
static TimeOut_t m_axTimeOut[eCOUNTER_TYPE_NUMBERS];
static TickType_t m_axTicksToWait[eCOUNTER_TYPE_NUMBERS] = {0};

static StaticTask_t m_xCNT_EventTaskTCB;
static StackType_t m_axCNT_EventStack[CNT_EVENT_STACK_SZ];

#if 0
// non FreeRTOS version utilCounterGet/utilCounterSet Functions

static QWORD m_aqwTickCountRef[eCOUNTER_TYPE_NUMBERS];
static QWORD m_aqwTickCountStart[eCOUNTER_TYPE_NUMBERS];
void utilCounterSet(eCOUNTER_TYPE eCounter, DWORD dwMilliSecond)
{
    ASSERT(eCOUNTER_TYPE_NUMBERS > eCounter);

    // store timer start value and
    m_aqwTickCountStart[eCounter] = (dwMilliSecond / portTICK_PERIOD_MS);

    // get current value of timer 1 and store it, reset rollover flag
    m_aqwTickCountRef[eCounter] = utilCounter_Tick_Get();
}

DWORD utilCounterGet(eCOUNTER_TYPE eCounter)
{
    QWORD qwTime = 0;

    ASSERT(eCOUNTER_TYPE_NUMBERS > eCounter);

    // validate arguments in debug mode only
    ASSERT(eCounter < eCOUNTER_TYPE_NUMBERS);

    // if the count timeout is zero, just return remaining count of zero
    if(m_aqwTickCountStart[eCounter] > 0)
    {
        // compute the timer ticks that have elapsed so far
        qwTime = utilCounter_Tick_Get();

        // 如果Ref Count > qwTime, 表示Timer counter已經有Ring
        if(m_aqwTickCountRef[eCounter] > qwTime)
        {
            qwTime = (0xFFFFFFFFFFFFFFFF - m_aqwTickCountRef[eCounter]) + qwTime;
        }
        else
        {
            qwTime = qwTime - m_aqwTickCountRef[eCounter];
        }

        // compute the counter ticks remaining until timeout
        if(m_aqwTickCountStart[eCounter] > qwTime)
        {
            // this subtraction requires unsigned, same size start and timer values
            qwTime = m_aqwTickCountStart[eCounter] - qwTime;
        }
        else
        {
            // the counter has finished so disable it and return zero
            m_aqwTickCountStart[eCounter] = 0;
            qwTime = 0;
        }
    }

    return qwTime;
}
#endif // 0

// ==============================================================================
// FUNCTION NAME: utilTimer_Event_Check
// DESCRIPTION:
//
//
// Params:
// eTIMER_EVENT_TYPE eSwTimer:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/04/02, Leo Create
// --------------------
// ==============================================================================
static DWORD utilTimer_Event_Check(eTIMER_EVENT_TYPE eSwTimer)
{
    if(m_asTimerEvent[eSwTimer].dwTimer > 0)
    {
        m_asTimerEvent[eSwTimer].dwTimer--;
    }

    return m_asTimerEvent[eSwTimer].dwTimer;
}

// ==============================================================================
// FUNCTION NAME: utilCounter_Event_Task
// DESCRIPTION:
//
//
// PARAMETERS:
// void *pParameters:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/03/24, Leo Create
// --------------------
// ==============================================================================
static void utilCounter_Event_Task(void *pParameters)
{
    BYTE cCount = 0;
    TickType_t xDelayTime;

    while(1)
    {
        for(cCount = 0; cCount < eTIMER_EVENT_TYPE_NUMBERS; cCount++)
        {
            if(NULL != m_asTimerEvent[cCount].fpEventCallback)
            {
                if(0 == utilTimer_Event_Check((eTIMER_EVENT_TYPE)cCount))
                {
                    // 計數時間達到，觸發Event Callback
                    m_asTimerEvent[cCount].fpEventCallback();

                    // 需重新載入
                    if(m_asTimerEvent[cCount].cReload)
                    {
                        m_asTimerEvent[cCount].dwTimer = m_asTimerEvent[cCount].dwReloadTimer;
                    }
                    // 需重複次數?
                    else if(m_asTimerEvent[cCount].wRepeatCount)
                    {
                        m_asTimerEvent[cCount].wRepeatCount--;
                        m_asTimerEvent[cCount].dwTimer = m_asTimerEvent[cCount].dwReloadTimer;

                        if(!m_asTimerEvent[cCount].wRepeatCount)
                        {
                            // 重複次數結束，觸發結束Finish Event Callback
                            m_asTimerEvent[cCount].fpEventFinishCallback();

                            // 清除Timer設定
                            m_asTimerEvent[cCount].fpEventCallback = NULL;
                            m_asTimerEvent[cCount].fpEventFinishCallback = NULL;
                        }
                    }
                    else
                    {
                        // 清除Timer設定
                        m_asTimerEvent[cCount].fpEventCallback = NULL;
                        m_asTimerEvent[cCount].fpEventFinishCallback = NULL;
                    }
                }
            }
        } // End of For

        vTaskDelayUntil(&xDelayTime, pdMS_TO_TICKS(1));
    }

    // __BKPT(0x05);
}

// ==============================================================================
// FUNCTION NAME: UTILCOUNTER_INIT
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
// 28/10/2013, Leo Create
// --------------------
// ==============================================================================
void utilCounter_Init(void)
{
    BYTE cCount = 0;

    for(cCount = 0; cCount < eTIMER_EVENT_TYPE_NUMBERS; cCount++)
    {
        m_asTimerEvent[cCount].dwTimer = 0;
        m_asTimerEvent[cCount].dwReloadTimer = 0;
        m_asTimerEvent[cCount].cReload = 0;
        m_asTimerEvent[cCount].wRepeatCount = 0;
        m_asTimerEvent[cCount].fpEventCallback = NULL;
        m_asTimerEvent[cCount].fpEventFinishCallback = NULL;
    }

    /* Create the task without using any dynamic memory allocation. */
    xTaskCreateStatic(utilCounter_Event_Task,   /* Function that implements the task. */
                      "Counter_Event_Task",     /* Text name for the task. */
                      CNT_EVENT_STACK_SZ,       /* Number of indexes in the xStack array. */
                      NULL,                     /* Parameter passed into the task. */
                      CNT_EVENT_TASK_PRIORITY,  /* Priority at which the task is created. */
                      m_axCNT_EventStack,       /* Array to use as the task's stack. */
                      &m_xCNT_EventTaskTCB);    /* Variable to hold the task's data structure. */
}

// ==============================================================================
// FUNCTION NAME: UTILCOUNTERSET
// DESCRIPTION:
//
//
// Params:
// eCOUNTER_TYPE eCounter:
// WORD wMilliSecond:
//
// Returns:
//
//
// modification history
// --------------------
// 19/03/2013, Leohong written
// --------------------
// ==============================================================================
void utilCounterSet(eCOUNTER_TYPE eCounter, DWORD dwMilliSecond)
{
    m_axTicksToWait[eCounter] = pdMS_TO_TICKS(dwMilliSecond);
    vTaskSetTimeOutState(&m_axTimeOut[eCounter]);
}

// ==============================================================================
// FUNCTION NAME: UTILCOUNTERGET
// DESCRIPTION:
//
//
// Params:
// eCOUNTER_TYPE eCounter:
//
// Returns:
//
//
// modification history
// --------------------
// 19/03/2013, Leohong written
// --------------------
// ==============================================================================
DWORD utilCounterGet(eCOUNTER_TYPE eCounter)
{
    if(pdPASS == xTaskCheckForTimeOut(&m_axTimeOut[eCounter], &m_axTicksToWait[eCounter]))
    {
        return 0;
    }

    return m_axTicksToWait[eCounter];
}

// ==============================================================================
// FUNCTION NAME: UTILDELAYMS
// DESCRIPTION:
//
//
// Params:
// WORD wMilliSecond:
//
// Returns:
//
//
// modification history
// --------------------
// 19/03/2013, Leohong written
// --------------------
// ==============================================================================
void utilDelayMs(DWORD dwMilliSecond)
{
    TickType_t xDelayTime;

    xDelayTime = xTaskGetTickCount();
    vTaskDelayUntil(&xDelayTime, pdMS_TO_TICKS(dwMilliSecond));
}

// ==============================================================================
// FUNCTION NAME: utilCounter_UnReg_Timer_Event
// DESCRIPTION:
//
//
// Params:
// eTIMER_EVENT_TYPE eTimer:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/04/24, Leo Create
// --------------------
// ==============================================================================
void utilCounter_UnReg_Timer_Event(eTIMER_EVENT_TYPE eTimer)
{
    m_asTimerEvent[eTimer].fpEventCallback = NULL;
}

// ==============================================================================
// FUNCTION NAME: utilCounter_Reg_Timer_Event
// DESCRIPTION:
//
//
// Params:
// eTIMER_EVENT_TYPE eTimer:                Timer ID
// DWORD dwTimer:                           Timer 時間
// BYTE cReloard:                           重新載入
// WORD wRepeatCount:                       重複次數
// fpTimerEventCallback fpCallback:         Timer 觸發的Event Callback
// fpTimerEventCallback fpFinishCallback:   Timer 結束後觸發的 Finsih Event Callback
//
// Returns:
//
//
// modification history
// --------------------
// 2014/04/25, Leo Create
// --------------------
// ==============================================================================
void utilCounter_Reg_Timer_Event(eTIMER_EVENT_TYPE eTimer,
                                 DWORD dwTimer,
                                 BYTE cReloard,
                                 WORD wRepeatCount,
                                 fpTimerEventCallback fpCallback,
                                 fpTimerEventCallback fpFinishCallback)
{
    m_asTimerEvent[eTimer].dwTimer = dwTimer;
    m_asTimerEvent[eTimer].dwReloadTimer = dwTimer;
    m_asTimerEvent[eTimer].cReload = cReloard;
    m_asTimerEvent[eTimer].wRepeatCount = wRepeatCount;
    m_asTimerEvent[eTimer].fpEventCallback = fpCallback;
    m_asTimerEvent[eTimer].fpEventFinishCallback = fpFinishCallback;

    if(wRepeatCount)
    {
        m_asTimerEvent[eTimer].cReload = FALSE;
    }

}

// ==============================================================================
// FUNCTION NAME: utilCounter_Tick_Get
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/05/27, Leo Create
// --------------------
// ==============================================================================
DWORD utilCounter_Tick_Get(void)
{
    return (DWORD)xTaskGetTickCount();
}

