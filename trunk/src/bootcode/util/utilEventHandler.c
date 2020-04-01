// ==============================================================================
// FILE NAME: UTILEVENTHANDLER.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2014/03/30, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"
#include "utilLogAPI.h"
#include "utilEventHandlerAPI.h"
#include "utilCounterAPI.h"
#include "appInputProcAPI.h"

#define EVENT_QUEUE_LENGTH  (32)
#define EVENT_ITEM_SIZE     (sizeof(sEVENT_DATA))

static StaticQueue_t m_xEventQueue;
static QueueHandle_t m_xEventQueueHDL;
static BYTE m_acEventBuffer[EVENT_QUEUE_LENGTH * EVENT_ITEM_SIZE];

static StaticTask_t m_xEventTaskTCB;
static StackType_t m_axEventStack[EVENT_TASK_STACK_SZ];

static TaskHandle_t xEvent_Handle = NULL;

// ==============================================================================
// FUNCTION NAME: UTILEVENTDISPATCH
// DESCRIPTION:
//
//
// Params:
// sEVENT_DATA sEvent:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/30, Leo Create
// --------------------
// ==============================================================================
static void utilEventDispatch(sEVENT_DATA *psEvent)
{
    switch(psEvent->eEvent)
    {
        case eEVENT_LIST_POWER:
            switch(psEvent->uEVET_DATA.wData)
            {
                case eSYSTEM_EVENT_POWER_ON:
                    //appSysProc_State_Set(eSYS_STATE_GOTO_POWER_NORMAL);
                    //appVideoProcessor_State_Set(eVIDEO_PROCESSOR_STATE_WARM_UP);
                    break;

                case eSYSTEM_EVENT_RESTORE_MODE:
                    //appSysProc_State_Set(eSYS_STATE_GOTO_POWER_NORMAL);
                    //appVideoProcessor_State_Set(eVIDEO_PROCESSOR_STATE_RESTORE_MODE);
                    break;

                case eSYSTEM_EVENT_POWER_SUPPLY_READY:
                    //halBoard_Chips_Power_Enable(TRUE);
                    break;

                case eSYSTEM_EVENT_POWER_NORMAL:
                    //appSysProc_State_Set(eSYS_STATE_POWER_NORMAL);
                    break;

                case eSYSTEM_EVENT_POWER_OFF:
                    //appSysProc_State_Set(eSYS_STATE_GOTO_STANDBY);
                    //appVideoProcessor_State_Set(eVIDEO_PROCESSOR_STATE_GOTO_STANDBY);
                    break;

                case eSYSTEM_EVENT_TEST_MODE:
                    //appVideoProcessor_Test_Mode(TRUE);
                    //appSysProc_State_Set(eSYS_STATE_GOTO_POWER_NORMAL);
                    //appVideoProcessor_State_Set(eVIDEO_PROCESSOR_STATE_WARM_UP);
                    break;

                case eSYSTEM_EVENT_CARD_POWER:
                    //appSysProc_Card_Power_Set(psEvent->uEVET_DATA.awData[1]);
                    break;

                default:
                    break;
            }
            break;

        case eEVENT_LIST_HDMI_SWITCHER:
            switch(psEvent->uEVET_DATA.wData)
            {
                case eSWITCHER_CTRL_POWER_ON:
                    break;

                case eSWITCHER_CTRL_RESET_DRIVER:
                    break;

                case eSWITCHER_CTRL_SWITCH_INPUT:
                    break;

                case eSWITCHER_CTRL_FUNC_TEST:
                    break;

                default:
                    break;
            }
            break;

        case eEVENT_LIST_KEY:
            switch(psEvent->uEVET_DATA.wData)
            {
                case eKEY_CTRL_TEST_MODE_POWER_ON:
                {
                }
                break;

                case eKEY_CTRL_TEST_EVENT:
                    break;

                default:
                    break;
            }
            break;

        default:
            LOG_MSG(eDB_MSK_LIST_EVENT_QUEUE, "EventDispatch Unknow Event\r\n");
            break;
    } // End of Switch
}

// ==============================================================================
// FUNCTION NAME: utilEventHandlerTask
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
static void utilEventHandlerTask(void *pParameters)
{
    sEVENT_DATA sEvent;

    while(1)
    {
        // Block Always
        if((NULL != m_xEventQueueHDL) && (pdTRUE == xQueueReceive(m_xEventQueueHDL, &sEvent, portMAX_DELAY)))
        {
            utilEventDispatch(&sEvent);
        }
    }
}

// ==============================================================================
// FUNCTION NAME: UTILEVENT_SEND
// DESCRIPTION:
//
//
// eEVENT_LIST eEvent:
// WORD wSize:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/31, Leo Create
// --------------------
// ==============================================================================
void utilEvent_Send(eEVENT_LIST eEvent, WORD wSize, void *pvData)
{
    sEVENT_DATA sTempEvent;
    sTempEvent.eEvent = eEvent;

    if(wSize > 0)
    {
        memcpy(sTempEvent.uEVET_DATA.acEventData, pvData, wSize);
    }

    if(NULL != m_xEventQueueHDL)
    {
        // Wait 100ms for Queue free
        if(pdFALSE == xQueueSend(m_xEventQueueHDL, (void *)&sTempEvent, pdMS_TO_TICKS(100)))
        {
            LOG_MSG(eDB_MSK_LIST_EVENT_QUEUE, "Event Queue Full\r\n");
        }
    }
}

// ==============================================================================
// FUNCTION NAME: utilEvent_Send_Notify
// DESCRIPTION:
//
//
// PARAMETERS:
// DWORD dwNotifyValue:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/06/05, Leo Create
// --------------------
// ==============================================================================
void utilEvent_Send_Notify(DWORD dwNotifyValue)
{
    xTaskNotify( xEvent_Handle, dwNotifyValue, eSetValueWithOverwrite );
}

// ==============================================================================
// FUNCTION NAME: UTILEVENTHANDLER_INIT
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
// 07/01/2014, Leo Create
// --------------------
// ==============================================================================
void utilEventHandler_Init(void)
{
    m_xEventQueueHDL = xQueueCreateStatic(EVENT_QUEUE_LENGTH,
                                          EVENT_ITEM_SIZE,
                                          m_acEventBuffer,
                                          &m_xEventQueue);

    if(NULL == m_xEventQueueHDL)
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "m_xEventQueueHDL Create Fail\r\n");
    }

    /* Create the task without using any dynamic memory allocation. */
    xEvent_Handle = xTaskCreateStatic(  utilEventHandlerTask,       /* Function that implements the task. */
                                        "Event_Handler_Task",       /* Text name for the task. */
                                        EVENT_TASK_STACK_SZ,        /* Number of indexes in the xStack array. */
                                        NULL,                       /* Parameter passed into the task. */
                                        EVENT_TASK_PRIORITY,        /* Priority at which the task is created. */
                                        m_axEventStack,             /* Array to use as the task's stack. */
                                        &m_xEventTaskTCB);          /* Variable to hold the task's data structure. */

    if(NULL == xEvent_Handle)
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "EventHandlerTask Create Fail\r\n");
    }
}


