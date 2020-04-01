// ===============================================================================
// FILE NAME: appHdmiSwitcher.c
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2019/10/24, Leo Create
// --------------------
// ===============================================================================


#include "utilLogAPI.h"
#include "appHdmiSwitcher.h"

// ==============================================================================
// FUNCTION NAME: appHdmiSwitcher_Init
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
// 2019/10/24, Leo Create
// --------------------
// ==============================================================================
void appHdmiSwitcher_Init(void)
{
    halHdmiSwitcher_Init();
}

// ==============================================================================
// FUNCTION NAME: appHdmiSwitcher_Switch_Input
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
// 2019/10/28, Leo Create
// --------------------
// ==============================================================================
void appHdmiSwitcher_Switch_Input(eSOURCE_LIST eSource)
{
    halHdmiSwitcher_Ctrl(eSWITCHER_CMD_SWITCH_PORT, (void*)&eSource);
}
