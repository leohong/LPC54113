#ifndef HALHDMISWITCHER_H
#define HALHDMISWITCHER_H
// ===============================================================================
// FILE NAME: halHdmiSwitcher.h
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2018/10/23, Leo Create
// --------------------
// ===============================================================================


#include "CommonDef.h"
#include "dvCard.h"
#include "ModeInfo.h"

#define DB_OPS_DP       2  // OPS DP
#define DB_OPS_HDMI     0  // OPS HDMI
#define DB_CARD         3  // GIM CARD

typedef enum
{
    eSW_DRIVER_CMD_RESET,
    eSW_DRIVER_CMD_SWITCH_INPUT,

    eSW_DRIVER_CMD_NUMBERS,
} eSW_DRIVER_CMD;

typedef struct
{
    eSW_DRIVER_CMD eCmd;

    union
    {
        WORD    wData;
        BYTE    acData[2];
    } uDATA;
} sSW_DRV_MSG;

typedef enum
{
    eSWITCHER_CMD_RESET,
    eSWITCHER_CMD_SWITCH_PORT,
    eSWITCHER_CMD_FUNC_TEST,

    eSWITCHER_CMD_NUMBERS,
} eSWITCHER_CMD;
typedef enum
{
    eSOURCE_LIST_OPS_HDMI,
    eSOURCE_LIST_OPS_DP,
    eSOURCE_LIST_SLOT_0,
    eSOURCE_LIST_SLOT_1,
    eSOURCE_LIST_SLOT_2,
    eSOURCE_LIST_SLOT_3,

    eSOURCE_LIST_NUMBERS,
} eSOURCE_LIST;

typedef struct
{
    eSOURCE_LIST    eLastSource;
    eSOURCE_LIST    eCurrentSource;
    
} sHAL_CONFIG;

//void halHdmiSwitcher_Task(void *pParameters);
void halHdmiSwitcher_Init(void);
void halHdmiSwitcher_Ctrl(eSWITCHER_CMD eCmd, void* pvBuffer);
sSLOT_INFO halHdmiSwitch_Slot_Info_Get(void);

#endif // HALHDMISWITCHER_H

