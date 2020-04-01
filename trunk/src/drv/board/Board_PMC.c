// ==============================================================================
// FILE NAME: BOARD_PMC.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 21/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "Board.h"
#include "Board_PMC.h"
#include "Board_WatchDog.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"

// ==============================================================================
// FUNCTION NAME: Board_Pmc_Reflash_Power_Saving_Timer
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
// 2014/12/23, Leo Create
// --------------------
// ==============================================================================
void Board_Pmc_Reflash_Power_Saving_Timer(void)
{
    utilCounterSet(eCOUNTER_TYPE_MCU_SLEEP, MCU_SLEEP_TIME);
}

// ==============================================================================
// FUNCTION NAME: EVRT_IRQHANDLER
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
// 25/11/2013, Leo Create
// --------------------
// ==============================================================================
void EVRT_IRQHandler(void)
{
    if(Chip_EVRT_IsSourceInterrupting(EVRT_SRC_WAKEUP0))
    {
        Chip_EVRT_ClrPendIntSrc(EVRT_SRC_WAKEUP0);
    }

    if(Chip_EVRT_IsSourceInterrupting(EVRT_SRC_WAKEUP1))
    {
        Chip_EVRT_ClrPendIntSrc(EVRT_SRC_WAKEUP1);
    }

    if(Chip_EVRT_IsSourceInterrupting(EVRT_SRC_WAKEUP2))
    {
        Chip_EVRT_ClrPendIntSrc(EVRT_SRC_WAKEUP2);
    }

    if(Chip_EVRT_IsSourceInterrupting(EVRT_SRC_WAKEUP3))
    {
        Chip_EVRT_ClrPendIntSrc(EVRT_SRC_WAKEUP3);
    }

    if(Chip_EVRT_IsSourceInterrupting(EVRT_SRC_RESET)) //A70LH_Larry_0095
    {
        Chip_EVRT_ClrPendIntSrc(EVRT_SRC_RESET);
    }

    Board_Pmc_Reflash_Power_Saving_Timer();

    Board_WatchDog_Feed();

    NVIC_ClearPendingIRQ(EVENTROUTER_IRQn);
}

// ==============================================================================
// FUNCTION NAME: PMC_EVRT_CONFIGURE
// DESCRIPTION:
//
//
// Params:
// CHIP_EVRT_SRC_T Evrt_Src:
//
// Returns:
//
//
// modification history
// --------------------
// 25/11/2013, Leo Create
// --------------------
// ==============================================================================
void PMC_Evrt_Configure(CHIP_EVRT_SRC_T Evrt_Src)
{
    /* Configure Interrupt signal from Evrt_Src pin to EVRT */
    Chip_EVRT_ConfigIntSrcActiveType(Evrt_Src, EVRT_SRC_ACTIVE_FALLING_EDGE);

    /* Enable interrupt signal from Evrt_Src pin to EVRT */
    Chip_EVRT_SetUpIntSrc(Evrt_Src, ENABLE);

    /* Clear any pending interrupt */
    Chip_EVRT_ClrPendIntSrc(Evrt_Src);

    /* Disable EVRT interrupt in NVIC */
    NVIC_DisableIRQ(EVENTROUTER_IRQn);

    NVIC_ClearPendingIRQ(EVENTROUTER_IRQn);

    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(EVENTROUTER_IRQn, eINT_PRIORITY_LEVEL_1);

    /* Enable Event Router interrupt in NVIC */
    NVIC_EnableIRQ(EVENTROUTER_IRQn);
}

// ==============================================================================
// FUNCTION NAME: PMC_Evrt_DeInit
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
// 2014/08/08, Leo Create
// --------------------
// ==============================================================================
void PMC_Evrt_DeInit(void)
{
    /* Disable EVRT interrupt in NVIC */
    NVIC_DisableIRQ(EVENTROUTER_IRQn);
}

// ==============================================================================
// FUNCTION NAME: BOARD_PWRSTATE_SET
// DESCRIPTION:
//
//
// Params:
// eBOARD_PWR ePwrState:
//
// Returns:
//
//
// modification history
// --------------------
// 21/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_PwrState_Set(eBOARD_PWR ePwrState)
{
#ifdef __DEBUG__
    char *pcString[4] =
    {
        "Woken up from eBOARD_PWR_DEEP_SLEEP...\r\n",
        "Woken up from eBOARD_PWR_POWER_DOWN...\r\n",
        "Woken up from eBOARD_PWR_DEEP_POWER_DOWN...\r\n",
        "Woken up from eBOARD_PWR_SLEEP...\r\n",
    };
#endif // __DEBUG__

    switch(ePwrState)
    {
        case eBOARD_PWR_SLEEP:
            Chip_PMC_Sleep();
            break;

        case eBOARD_PWR_DEEP_SLEEP:
        case eBOARD_PWR_POWER_DOWN:
        case eBOARD_PWR_DEEP_POWER_DOWN:
            Chip_Clock_StartPowerDown();
            PMC_Evrt_Configure(EVRT_SRC_WAKEUP0);
            PMC_Evrt_Configure(EVRT_SRC_WAKEUP1);
            PMC_Evrt_Configure(EVRT_SRC_WAKEUP2);
            PMC_Evrt_Configure(EVRT_SRC_WAKEUP3);
            PMC_Evrt_Configure(EVRT_SRC_RESET);     //A70LH_Larry_0095
            Chip_PMC_Set_PwrState((CHIP_PMC_PWR_STATE_T)ePwrState);
            Chip_Clock_ClearPowerDown();
            PMC_Evrt_DeInit();
            break;

        default:
            ASSERT_ALWAYS();
            break;
    }

    LOG_MSG(eDB_MSK_LIST_PWR, "%s", pcString[ePwrState]);
}

