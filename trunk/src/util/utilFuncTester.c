// ===============================================================================
// FILE NAME: utilFuncTester.c
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2019/03/26, Leo Create
// --------------------
// ===============================================================================


#include "Commondef.h"
#include "Board_Uart.h"
#include "Board_GPIO.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"
#include "utilFuncTesterAPI.h"
#include "utilEventHandlerAPI.h"
#include "halHdmiSwitcher.h"
#include "appSysmon.h"

#define tskRUNNING_CHAR     ( 'X' )
#define tskBLOCKED_CHAR     ( 'B' )
#define tskREADY_CHAR       ( 'R' )
#define tskDELETED_CHAR     ( 'D' )
#define tskSUSPENDED_CHAR   ( 'S' )

static char *prvWriteNameToBuffer( char *pcBuffer, const char *pcTaskName )
{
    size_t x;

    /* Start by copying the entire string. */
    strcpy( pcBuffer, pcTaskName );

    /* Pad the end of the string with spaces to ensure columns line up when
    printed out. */
    for( x = strlen( pcBuffer ); x < ( size_t ) ( configMAX_TASK_NAME_LEN - 1 ); x++ )
    {
        pcBuffer[ x ] = ' ';
    }

    /* Terminate. */
    pcBuffer[ x ] = 0x00;

    /* Return the new end of string. */
    return &( pcBuffer[ x ] );
}

void vTaskList_Static( char * pcWriteBuffer )
{
    TaskStatus_t acTaskStatusArray[10];
    TaskStatus_t *pxTaskStatusArray = acTaskStatusArray;
    UBaseType_t uxArraySize, x;
    char cStatus;

    /*
     * PLEASE NOTE:
     *
     * This function is provided for convenience only, and is used by many
     * of the demo applications.  Do not consider it to be part of the
     * scheduler.
     *
     * vTaskList() calls uxTaskGetSystemState(), then formats part of the
     * uxTaskGetSystemState() output into a human readable table that
     * displays task names, states and stack usage.
     *
     * vTaskList() has a dependency on the sprintf() C library function that
     * might bloat the code size, use a lot of stack, and provide different
     * results on different platforms.  An alternative, tiny, third party,
     * and limited functionality implementation of sprintf() is provided in
     * many of the FreeRTOS/Demo sub-directories in a file called
     * printf-stdarg.c (note printf-stdarg.c does not provide a full
     * snprintf() implementation!).
     *
     * It is recommended that production systems call uxTaskGetSystemState()
     * directly to get access to raw stats data, rather than indirectly
     * through a call to vTaskList().
     */


    /* Make sure the write buffer does not contain a string. */
    *pcWriteBuffer = ( char ) 0x00;

    /* Take a snapshot of the number of tasks in case it changes while this
    function is executing. */
    //uxArraySize = uxCurrentNumberOfTasks;
    uxArraySize = uxTaskGetNumberOfTasks();
    uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );

    /* Allocate an array index for each task.  NOTE!  if
    configSUPPORT_DYNAMIC_ALLOCATION is set to 0 then pvPortMalloc() will
    equate to NULL. */
    //pxTaskStatusArray = pvPortMalloc( uxCurrentNumberOfTasks * sizeof( TaskStatus_t ) ); /*lint !e9079 All values returned by pvPortMalloc() have at least the alignment required by the MCU's stack and this allocation allocates a struct that has the alignment requirements of a pointer. */

    if( pxTaskStatusArray != NULL )
    {
        /* Generate the (binary) data. */
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );

        /* Create a human readable table from the binary data. */
        for( x = 0; x < uxArraySize; x++ )
        {
            switch( pxTaskStatusArray[ x ].eCurrentState )
            {
                case eRunning:      cStatus = tskRUNNING_CHAR;
                                    break;

                case eReady:        cStatus = tskREADY_CHAR;
                                    break;

                case eBlocked:      cStatus = tskBLOCKED_CHAR;
                                    break;

                case eSuspended:    cStatus = tskSUSPENDED_CHAR;
                                    break;

                case eDeleted:      cStatus = tskDELETED_CHAR;
                                    break;

                case eInvalid:      /* Fall through. */
                default:            /* Should not get here, but it is included
                                    to prevent static checking errors. */
                                    cStatus = ( char ) 0x00;
                                    break;
            }

            /* Write the task name to the string, padding with spaces so it
            can be printed in tabular form more easily. */
            pcWriteBuffer = prvWriteNameToBuffer( pcWriteBuffer, pxTaskStatusArray[ x ].pcTaskName );

            /* Write the rest of the string. */
            sprintf( pcWriteBuffer, "\t%c\t%u\t%u\t%u\r\n", cStatus, ( unsigned int ) pxTaskStatusArray[ x ].uxCurrentPriority, ( unsigned int ) pxTaskStatusArray[ x ].usStackHighWaterMark, ( unsigned int ) pxTaskStatusArray[ x ].xTaskNumber ); /*lint !e586 sprintf() allowed as this is compiled with many compilers and this is a utility function only - not part of the core kernel implementation. */
            pcWriteBuffer += strlen( pcWriteBuffer ); /*lint !e9016 Pointer arithmetic ok on char pointers especially as in this case where it best denotes the intent of the code. */
        }

        /* Free the array again.  NOTE!  If configSUPPORT_DYNAMIC_ALLOCATION
        is 0 then vPortFree() will be #defined to nothing. */
        //vPortFree( pxTaskStatusArray );
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
}

// ==============================================================================
// FUNCTION NAME: vTaskListPrint
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
// 2019/03/25, Leo Create
// --------------------
// ==============================================================================
void vTaskListPrint(void)
{
    static char m_acTaskListBuffer[512];
    vTaskList_Static(m_acTaskListBuffer);

    LOG_MSG(eDB_MSK_LIST_ASSERT, "task_name             state priority freestack task_num\n");
    LOG_MSG(eDB_MSK_LIST_ASSERT, "%s", m_acTaskListBuffer);
    LOG_MSG(eDB_MSK_LIST_ASSERT, "\r\n");
}

// ==============================================================================
// FUNCTION NAME: utilFunc_Test_Loop
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
// 2020/02/12, Leo Create
// --------------------
// ==============================================================================
void utilFunc_Test_Loop(void)
{
    if(0 == utilCounterGet(eCOUNTER_TYPE_FUNCTION_TEST))
    {
        utilCounterSet(eCOUNTER_TYPE_FUNCTION_TEST, 1000);

#ifdef SIM_PV7
        {
            sEVENT_DATA sEventData;
            static BYTE cInput = 0;
            cInput = (cInput + 1) % eSOURCE_LIST_NUMBERS;

            if(eSYS_STATE_GOTO_POWER_NORMAL < appSysProc_State_Get())
            {
                sEventData.uEVET_DATA.wData = eSWITCHER_CTRL_FUNC_TEST;
                utilEvent_Send(eEVENT_LIST_HDMI_SWITCHER, 2, &sEventData.uEVET_DATA);

                sEventData.uEVET_DATA.wData = eSWITCHER_CTRL_SWITCH_INPUT;
                sEventData.uEVET_DATA.acEventData[2] = cInput;
                utilEvent_Send(eEVENT_LIST_HDMI_SWITCHER, 3, &sEventData.uEVET_DATA);

                LOG_MSG(eDB_MSK_LIST_INPUT, "Switch to %d\r\n", cInput);
            }
        }
#else
#if 0
        if(eSYS_STATE_GOTO_POWER_NORMAL < appSysProc_State_Get())
        {
            sEVENT_DATA sEventData;
            sEventData.uEVET_DATA.wData = eSWITCHER_CTRL_FUNC_TEST;
            utilEvent_Send(eEVENT_LIST_HDMI_SWITCHER, 2, &sEventData.uEVET_DATA);
        }
#endif // 0
#endif // SIM_PV7
    }
}


