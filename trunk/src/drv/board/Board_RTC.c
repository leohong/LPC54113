// ==============================================================================
// FILE NAME: BOARD_RTC.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 25/12/2013, Leo Create
// --------------------
// ==============================================================================


#include "Board_RTC.h"
#include "utilMathAPI.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"

// ==============================================================================
// FUNCTION NAME: BOARD_RTC_INIT
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
// 25/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_RTC_Init(void)
{
    LOG_MSG(eDB_MSK_LIST_RTC, "RTC Time\r\n");

    LOG_MSG(eDB_MSK_LIST_RTC, "Enable RTC\r\n");

    Chip_RTC_Init(LPC_RTC);

#if 0
    /* Set the RTC to generate an interrupt on each second */
    Chip_RTC_CntIncrIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, ENABLE);

    /* Enable matching for alarm for second, minute, hour fields only */
    Chip_RTC_AlarmIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC | RTC_AMR_CIIR_IMMIN | RTC_AMR_CIIR_IMHOUR,
                            ENABLE);

    /* Clear interrupt pending */
    Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE | RTC_INT_ALARM);

    /* Enable RTC interrupt in NVIC */
    NVIC_EnableIRQ((IRQn_Type) RTC_IRQn);
#endif //0

    /* Enable RTC (starts increase the tick counter and second counter register) */
    Chip_RTC_Enable(LPC_RTC, ENABLE);
}

// ==============================================================================
// FUNCTION NAME: BOARD_RTC_IS_ENABLE
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
// 25/12/2013, Leo Create
// --------------------
// ==============================================================================
BOOL Board_RTC_IS_Enable(void)
{
    BOOL cIsEnable = FALSE;
    cIsEnable = (BOOL)GETBIT(LPC_RTC->CCR, RTC_CCR_CLKEN);

    return cIsEnable;
}

// ==============================================================================
// FUNCTION NAME: BOARD_RTC_SET_TIMER
// DESCRIPTION:
//
//
// Params:
// RTC_TIME_T *psFullTime:
//
// Returns:
//
//
// modification history
// --------------------
// 25/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_RTC_Set_Timer(RTC_TIME_T *psFullTime)
{
    Chip_RTC_SetFullTime(LPC_RTC, psFullTime);
}

// ==============================================================================
// FUNCTION NAME: BOARD_RTC_GET_TIMER
// DESCRIPTION:
//
//
// Params:
// RTC_TIME_T *psFullTime:
//
// Returns:
//
//
// modification history
// --------------------
// 25/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_RTC_Get_Timer(RTC_TIME_T *psFullTime)
{
    Chip_RTC_GetFullTime(LPC_RTC, psFullTime);
}

// ==============================================================================
// FUNCTION NAME: BOARD_RTC_TEST
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
// 03/01/2014, Leo Create
// --------------------
// ==============================================================================
void Board_RTC_Test(void)
{
    RTC_TIME_T sTime = {0};

    if(0 == utilCounterGet(eCOUNTER_TYPE_RTC))
    {
        utilCounterSet(eCOUNTER_TYPE_RTC, 1000);

        if(!Board_RTC_IS_Enable())
        {
            LOG_MSG(eDB_MSK_LIST_RTC, "Set RTC Time\r\n");

            sTime.time[RTC_TIMETYPE_HOUR] = 14;
            sTime.time[RTC_TIMETYPE_MINUTE] = 40;
            sTime.time[RTC_TIMETYPE_SECOND] = 0;
            sTime.time[RTC_TIMETYPE_DAYOFMONTH] = 3;
            sTime.time[RTC_TIMETYPE_MONTH] = 1;
            sTime.time[RTC_TIMETYPE_YEAR] = 2014;

            LOG_MSG(eDB_MSK_LIST_RTC, "Time: %.2d:%.2d:%.2d %.2d/%.2d/%.4d\r\n",
                    sTime.time[RTC_TIMETYPE_HOUR],
                    sTime.time[RTC_TIMETYPE_MINUTE],
                    sTime.time[RTC_TIMETYPE_SECOND],
                    sTime.time[RTC_TIMETYPE_DAYOFMONTH],
                    sTime.time[RTC_TIMETYPE_MONTH],
                    sTime.time[RTC_TIMETYPE_YEAR]);

            Board_RTC_Init();
            Board_RTC_Set_Timer(&sTime);
            utilCounterSet(eCOUNTER_TYPE_RTC, 5000);
        }
        else
        {
            LOG_MSG(eDB_MSK_LIST_RTC, "Get RTC Time\r\n");

            Board_RTC_Get_Timer(&sTime);

            LOG_MSG(eDB_MSK_LIST_RTC, "Time: %.2d:%.2d:%.2d %.2d/%.2d/%.4d\r\n",
                    sTime.time[RTC_TIMETYPE_HOUR],
                    sTime.time[RTC_TIMETYPE_MINUTE],
                    sTime.time[RTC_TIMETYPE_SECOND],
                    sTime.time[RTC_TIMETYPE_DAYOFMONTH],
                    sTime.time[RTC_TIMETYPE_MONTH],
                    sTime.time[RTC_TIMETYPE_YEAR]);
        }
    }

}
