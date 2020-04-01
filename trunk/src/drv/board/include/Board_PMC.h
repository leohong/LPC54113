#ifndef BOARD_PMC_H
#define BOARD_PMC_H
// ==============================================================================
// FILE NAME: BOARD_PMC.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 21/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"

#define MCU_SLEEP_TIME (5000)   //5s

typedef enum
{
    // follow system power state, do not change this list
    eBOARD_PWR_DEEP_SLEEP,          /*!< Deep Sleep state */
    eBOARD_PWR_POWER_DOWN,          /*!< Power Down state */
    eBOARD_PWR_DEEP_POWER_DOWN,     /*!< Power Down state */

    // add a sleep power state
    eBOARD_PWR_SLEEP,               /*!< Sleep state */

    eBOARD_PWR_NUMBERS,
} eBOARD_PWR;

void PMC_Evrt_Configure(CHIP_EVRT_SRC_T Evrt_Src);
void Board_PwrState_Set(eBOARD_PWR ePwrState);
void PMC_Evrt_DeInit(void);
void Board_Pmc_Reflash_Power_Saving_Timer(void);

#endif /* BOARD_PMC_H */

