// ===============================================================================
// FILE NAME: Board_Watchdog.c
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2015/03/19, Leo Create
// --------------------
// ===============================================================================


#include "Board_WatchDog.h"
#include "utilDataMgrAPI.h"
#include "utilErrorLogAPI.h"

#define WWDT_TIMEOUT(X) (X * (WDT_OSC/1000))

static BYTE m_cResetEnable = FALSE;

void WDT_IRQHandler(void)
{
    if(TRUE == m_cResetEnable)
    {
#ifdef OE_JIG //A70LH_Larry_0069
#else
        utilErrorLogSet(eERROR_LOG_WATCH_DOG_RESET); //A70LH_Larry_0196
        utilDataMgrUpdate(UPDATE_ALL_NODEID); //A70LH_Larry_0196
        NVIC_SystemReset();
#endif /* OE_JIG */
    }

    Chip_WWDT_Feed(LPC_WWDT);
    Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDINT);
    NVIC_ClearPendingIRQ(WWDT_IRQn);
}

// ==============================================================================
// FUNCTION NAME: Board_WatchDog_Init
// DESCRIPTION:
//
// Watchdog啟動之後, 無法使用軟體關閉, 除非系統reset
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/04/01, Leo Create
// --------------------
// ==============================================================================
void Board_WatchDog_Init(void)
{
#ifdef CORE_M4
    Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDEN);
    Chip_WWDT_Init(LPC_WWDT);
    NVIC_EnableIRQ(WWDT_IRQn);
#endif // CORE_M4
}

// ==============================================================================
// FUNCTION NAME: Board_WatchDog_Reset_Enable
// DESCRIPTION:
//
//
// Params:
// BYTE cEnable:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/04/01, Leo Create
// --------------------
// ==============================================================================
void Board_WatchDog_Reset_Enable(BYTE cEnable)
{
#ifdef CORE_M4
    if(TRUE == cEnable)
    {
        Chip_WWDT_Feed(LPC_WWDT); //A70LH_Larry_0195
        m_cResetEnable = TRUE;
    }
    else
    {
        m_cResetEnable = FALSE;
    }
#else
    (void)cEnable;
#endif // CORE_M4
}

// ==============================================================================
// FUNCTION NAME: Board_WatchDog_Feed
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
// 2015/04/01, Leo Create
// --------------------
// ==============================================================================
void Board_WatchDog_Feed(void)
{
#ifdef CORE_M4
    Chip_WWDT_Feed(LPC_WWDT);
#endif // CORE_M4
}

// ==============================================================================
// FUNCTION NAME: Board_WatchDog_Set_Timeout
// DESCRIPTION:
//
//
// Params:
// DWORD dwMilliSecond:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/04/01, Leo Create
// --------------------
// ==============================================================================
void Board_WatchDog_Set_Timeout(DWORD dwMilliSecond)
{
#ifdef CORE_M4
    //Chip_WWDT_SetTimeOut(LPC_WWDT, WWDT_TIMEOUT(dwMilliSecond));
    Chip_WWDT_SetTimeOut(LPC_WWDT, 0xFFFFFFFF);
#endif // CORE_M4
}

