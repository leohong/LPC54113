// ===============================================================================
// FILE NAME: Board_RITimer.c
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2014/10/31, Leo Create
// --------------------
// ===============================================================================


#include "Board.h"
#include "Board_RITimer.h"

volatile static WORD m_wTimerHighWord = 0;

#ifdef CORE_M4
#define RIT_IRQ_Handler RIT_IRQHandler
#endif // CORE_M4

#ifdef CORE_M0
#define RIT_IRQ_Handler RIT_WDT_IRQHandler
#endif // CORE_M0

void RIT_IRQ_Handler(void)
{
    m_wTimerHighWord++;

    /* Clearn interrupt */
    Chip_RIT_ClearInt(LPC_RITIMER);
    NVIC_ClearPendingIRQ(RITIMER_IRQn);
}

// ==============================================================================
// FUNCTION NAME: Board_RITimer_Init
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
// 2014/10/31, Leo Create
// --------------------
// ==============================================================================
void Board_RITimer_Init(void)
{
    m_wTimerHighWord = 0;

    /* Initialize RITimer */
    Chip_RIT_Init(LPC_RITIMER);

    /* Configure RIT for Free Run */
    // Chip_RIT_SetTimerInterval(LPC_RITIMER, 1);

    Chip_RIT_ClearInt(LPC_RITIMER);

    NVIC_ClearPendingIRQ(RITIMER_IRQn);

    NVIC_SetPriority(RITIMER_IRQn, eINT_PRIORITY_LEVEL_7);

    NVIC_EnableIRQ(RITIMER_IRQn);
}

// ==============================================================================
// FUNCTION NAME: Board_RITimer_DeInit
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
// 2014/10/31, Leo Create
// --------------------
// ==============================================================================
void Board_RITimer_DeInit(void)
{
    Chip_RIT_ClearInt(LPC_RITIMER);
    NVIC_DisableIRQ(RITIMER_IRQn);
    Chip_RIT_DeInit(LPC_RITIMER);
}

// ==============================================================================
// FUNCTION NAME: Board_RITimer_Count_Get
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
// 2014/10/31, Leo Create
// --------------------
// ==============================================================================
QWORD Board_RITimer_Count_Get(void)
{
    QWORD qTimer = m_wTimerHighWord << 16;
    qTimer = qTimer << 16;
    return (qTimer + Chip_RIT_GetCounter(LPC_RITIMER));
}

