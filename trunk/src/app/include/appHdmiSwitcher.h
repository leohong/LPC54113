#ifndef APPHDMISWITCHER_H
#define APPHDMISWITCHER_H
// ===============================================================================
// FILE NAME: appHdmiSwitcher.h
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2019/10/24, Leo Create
// --------------------
// ===============================================================================


#include "CommonDef.h"
#include "halHdmiSwitcher.h"

typedef enum
{
    eHDMI_SW_STATE_STANDBY,
    eHDMI_SW_STATE_POWER_ON,
    eHDMI_SW_STATE_POWER_NORMAL,

    eHDMI_SW_STATE_NUMBERS,
} eHDMI_SW_STATE;



void appHdmiSwitcher_Init(void);
void appHdmiSwitcher_Switch_Input(eSOURCE_LIST eSource);


#endif /* APPHDMISWITCHER_H */

