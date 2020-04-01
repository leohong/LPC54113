// ==============================================================================
// FILE NAME: APPINPUTPROC.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 22/04/2013, Leohong Create
// --------------------
// ==============================================================================


#include "Board_GPIO.h"
#include "Board_UART.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"
#include "halKeyCodeList.h"
#include "halInputCtrlAPI.h"
#include "halBoardCtrlAPI.h"
#include "appInputProcAPI.h"
#include "utilEventHandlerAPI.h"

#include "appSysmon.h"
//#include "halHdmiSwitcher.h"


#define KEY_BUFFER_SIZE (8)
#define POWER_KEY_TIMEOUT   (2*ONE_SECOND)
#define FRAME_RATE_TIMEOUT  (10 * ONE_SECOND)

#define KEY_DATA_QUEUE_LENGTH    (KEY_BUFFER_SIZE)
#define KEY_DATA_ITEM_SIZE       (sizeof(sKEY_DATA) / sizeof(BYTE))

static BYTE m_acKEY_DATA_Buffer[KEY_DATA_QUEUE_LENGTH * KEY_DATA_ITEM_SIZE];
static QueueHandle_t m_xKEY_DATA_QueueHDL;
static StaticQueue_t m_xKEY_DATA_Queue;


// ==============================================================================
// FUNCTION NAME: APPINPUTPROCINIT
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
// 22/04/2013, Leohong Create
// --------------------
// ==============================================================================
eRESULT appInputProc_Init(void)
{
    BYTE cCustomIdIndex = 1;

    m_xKEY_DATA_QueueHDL = xQueueCreateStatic(KEY_DATA_QUEUE_LENGTH,
                                              KEY_DATA_ITEM_SIZE,
                                              m_acKEY_DATA_Buffer,
                                              &m_xKEY_DATA_Queue);

    if(NULL == m_xKEY_DATA_QueueHDL)
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "m_xKEY_DATA_QueueHDL Create Fail\r\n");
    }

    halInputCtrl_IR_CustomId_Set(cCustomIdIndex);

    halInputCtrl_Init();
    halInputCtrl_Register_Callback(appInputProc_BufferInsert);

#ifdef TEST_BY_PV7
    {
        sKEY_DATA sKeyData = {0, eKEY_EVENT_PRESSED, eKEYINPUT_KEYPAD};
        sKeyData.wKeyCode = keRELEASE_TEST_MODE;

        appInputProc_BufferInsert(&sKeyData);
    }
#endif /* TEST_BY_PV7 */
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: APPINPUTPROC_BUFFERINSERT
// DESCRIPTION:
//
//
// Params:
// void *pvKey:
//
// Returns:
//
//
// modification history
// --------------------
// 24/12/2013, Leo Create
// --------------------
// ==============================================================================
eRESULT appInputProc_BufferInsert(void *pvKey)
{
    eRESULT eResult = rcERROR;

    if((NULL != m_xKEY_DATA_QueueHDL) && (pdTRUE == xQueueSend(m_xKEY_DATA_QueueHDL, (void *)pvKey, 0)))
    {
        eResult = rcSUCCESS;
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_INPUT, "KEY Queue Full\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: appInputProc_Process
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
// 12/11/2013, Leo Create
// --------------------
// ==============================================================================
void appInputProc_Process(void)
{
    sKEY_DATA sInputKey = {0};

    halInputCtrl_KeyPad_Decode();
    halInputCtrl_IR_Decode();

    while((NULL != m_xKEY_DATA_QueueHDL) && (pdTRUE == xQueueReceive(m_xKEY_DATA_QueueHDL, (void *)&sInputKey, 0)))
    {
        LOG_MSG(eDB_MSK_LIST_INPUT, "Key %d\r\n", sInputKey.wKeyCode);

        switch(sInputKey.wKeyCode)
        {
            // ==============================================================================
            // Pressed key
            // ==============================================================================
            case kePOWERON:
            {
                LOG_MSG(eDB_MSK_LIST_INPUT, "kePOWERON\r\n");
            }
            break;

            case keRELEASE_POWERON:
            {
                sEVENT_DATA sEventData;
                sEventData.uEVET_DATA.wData = eKEY_CTRL_TEST_MODE_POWER_ON;
                utilEvent_Send(eEVENT_LIST_KEY, 2, &sEventData.uEVET_DATA);

                LOG_MSG(eDB_MSK_LIST_INPUT, "Test Mode Power On\r\n");
            }
            break;

            default:
                break;
        }
    }
}

// ==============================================================================
// FUNCTION NAME: APPINPUTPROC_CLIHANDLER
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// modification history
// --------------------
// 12/11/2013, Leo Create
// --------------------
// ==============================================================================
eRESULT appInputProc_CLIHandler(BYTE cKey)
{
    eRESULT eResult = rcERROR;

    ASSERT(eKEY_LIST_NUMBER > cKey);

    if(eKEY_LIST_NUMBER > cKey)
    {
        sKEY_DATA sKeyData = {0, eKEY_EVENT_PRESSED, eKEYINPUT_KEYPAD};
        sKeyData.wKeyCode = cKey;

        appInputProc_BufferInsert(&sKeyData);
        eResult = rcSUCCESS;
    }

    return eResult;
}
