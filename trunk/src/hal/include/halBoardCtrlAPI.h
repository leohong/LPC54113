#ifndef HALBOARDCTRLAPI_H
#define HALBOARDCTRLAPI_H
// ==============================================================================
// FILE NAME: HALBOARDCTRLAPI.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 19/12/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"

typedef enum
{
    eLED_LIST_RED,
    eLED_LIST_GREEN_1,
    eLED_LIST_GREEN_2,

    eLED_LIST_NUMBERS,
} eLED_LIST;

typedef enum
{
    eLED_CTRL_OFF,
    eLED_CTRL_ON,
    eLED_CTRL_BLINK,

    eLED_CTRL_S_NUMBERS,
} eLED_CTRL;

typedef enum
{
    eLED_STATE_STANDBY,
    eLED_STATE_WARMUP,
    eLED_STATE_UPDATE,
    eLED_STATE_RESTORE,
    eLED_STATE_REMOVE_USB,
    eLED_STATE_POWER_NORMAL,
    eLED_STATE_GOTO_STANDBY,
    eLED_STATE_ERROR,
    eLED_STATE_PROGRAMMING,

    eLED_STATE_NUMBERS,
} eLED_STATE;

void halBoard_Init(void);
void halBoard_Power_Enable(BYTE cEnable);
void halBoard_LED_Ctrl(eLED_LIST eLED, eLED_CTRL eState, DWORD dwOnTime);
void halBoard_LED_State_Set(eLED_STATE eState);
void halBoard_Chips_Power_Enable(BYTE cEnable);
void halBoard_Uart_ReDirect_Toggle(void);

void halBoard_PwrSeqSet(void);
void halBoard_OPS_Power_Rail_Enable(BYTE cEnable);
void halBoard_Slot_Power_Rail_Enable(BYTE cEnable);

#endif /* HALBOARDCTRLAPI_H */

