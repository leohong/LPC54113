// ==============================================================================
// FILE NAME: UTILLOG.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 02/07/2013, Leo written
// --------------------
// ==============================================================================


#include "Board_Uart.h"
#include "utilLogAPI.h"
#include "retarget.h"

#define MAX_STR_SIZE    (256)

const char *pstrDbgMask[eDB_MSK_LIST_NUMBERS] =
{
    "ASSERT",
    "IAP",
    "VIDEO_SWITCHER",
    "INPUT",
    "PROC_CMD",
    "GIM_CMDGIM_CMD",
    "EVENT_QUEUE",
    "SYSCTRL",
};

static DWORD m_dwDebugMask = 0;
SemaphoreHandle_t m_xLogSemaphore = NULL;
StaticSemaphore_t m_xLogSemaphoreBuffer;

// ==============================================================================
// FUNCTION NAME: utilLog_Init
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
// 2014/10/30, Leo Create
// --------------------
// ==============================================================================
void utilLog_Init(void)
{
    DWORD dwDbgFlag = 0;

    m_dwDebugMask = (m_dwDebugMask | dwDbgFlag);
    m_xLogSemaphore = xSemaphoreCreateMutexStatic(&m_xLogSemaphoreBuffer);
}

// ==============================================================================
// FUNCTION NAME: utilLog_Mask_Set
// DESCRIPTION:
//
//
// Params:
// DWORD dwMask:
// BYTE cEnable:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/30, Leo Create
// --------------------
// ==============================================================================
void utilLog_Mask_Set(eDB_MSK_LIST eMask, BYTE cEnable)
{
    if(eDB_MSK_LIST_NUMBERS == eMask)
    {
        if(TRUE == cEnable)
        {
            m_dwDebugMask = 0xFFFFFFFF;
        }
        else
        {
            m_dwDebugMask = 0;
        }
    }
    else
    {
        DWORD dwMask = (0x0001 << eMask);

        if(TRUE == cEnable)
        {
            m_dwDebugMask |= dwMask;
        }
        else
        {
            m_dwDebugMask &= ~dwMask;
        }
    }

    // utilLog_Param_Set(eLOG_PARAM_FLAG, &m_dwDebugMask);
}

// ==============================================================================
// FUNCTION NAME: utilLog_Mask_Get
// DESCRIPTION:
//
//
// Params:
// DWORD dwMask:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/30, Leo Create
// --------------------
// ==============================================================================
BYTE utilLog_Mask_Get(eDB_MSK_LIST eMask)
{
    if(eDB_MSK_LIST_NUMBERS == eMask)
    {
        return m_dwDebugMask;
    }
    else
    {
        return !!(m_dwDebugMask & (0x0001 << eMask));
    }
}

// ==============================================================================
// FUNCTION NAME: utilLog_Mask_Print_Out
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
// 2019/04/11, Leo Create
// --------------------
// ==============================================================================
void utilLog_Mask_Print_Out(void)
{
    BYTE cCount = 0;

    for(cCount = 0; cCount < eDB_MSK_LIST_NUMBERS; cCount++)
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "%s %d\r\n", pstrDbgMask[cCount], utilLog_Mask_Get((eDB_MSK_LIST)cCount));
    }
}

// ==============================================================================
// FUNCTION NAME: UTILLOGPRINTF
// DESCRIPTION:
//
//
// Params:
// DWORD dwDbMask:
// char *pcfmtstring:
// ...:
//
// Returns:
//
//
// modification history
// --------------------
// 02/07/2013, Leo written
// --------------------
// ==============================================================================
static char acString[MAX_STR_SIZE] = {'\0'};
void utilLog_Printf(eDB_MSK_LIST eMask, char *pcfmtstring, ...)
{
    if(NULL != m_xLogSemaphore)
    {
        if(pdTRUE == xSemaphoreTake(m_xLogSemaphore, 0))
        {
            //if((TRUE == !!(m_dwDebugMask & (0x0001 << eMask))) || (ASSERT_MASK == (0x0001 << eMask)))
            {
                //initial a va_list memory point
                va_list pcInputString;

                memset(acString, 0, MAX_STR_SIZE);

                //Get the fmt address point
                va_start(pcInputString, pcfmtstring);

                //format string "Need heap memory"
                vsnprintf(acString, MAX_STR_SIZE, pcfmtstring, pcInputString);

                //write message by UART
                Board_Uart_Write(API_RS232_PORT, strlen(acString), (BYTE *)acString, 0);

                //release memory of va_list
                va_end(pcInputString);
            }

            xSemaphoreGive(m_xLogSemaphore);
        }
    }
}

