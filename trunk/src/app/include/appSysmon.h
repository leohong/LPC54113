#ifndef _APPSYSMON_H_
#define _APPSYSMON_H_

#include "CommonDef.h"
#include "halHdmiSwitcher.h"

typedef enum
{
    eSYS_STATE_STANDBY,
    eSYS_STATE_GOTO_POWER_NORMAL,
    eSYS_STATE_POWER_NORMAL,
    eSYS_STATE_GOTO_STANDBY,

    eSYS_STATE_SYSTEM_ERROR,

    eSYS_STATE_NUMBERS,
} eSYS_STATE;

#define OPS_MASK_SHIT   (0x00)
#define CARD_MASK_SHIT  (0x01)
#define OPS_MASK        (0x01 << OPS_MASK_SHIT)
#define CARD_MASK       (0x01 << CARD_MASK_SHIT)
#define ALL_SLOT_MASK   (OPS_MASK | CARD_MASK)

typedef struct
{
    eSYS_STATE  eCurrentState;
    eSYS_STATE  eLastState;
    WORD        wCardPresent;
    WORD        wCardPowerOn;
    WORD        wCardPowerStatus;
} sSYS_CONFIG;

void appSysProc_Init(void);
void appSysProc_Process(void);
void appSysProc_State_Set(eSYS_STATE eState);
eSYS_STATE appSysProc_State_Get(void);
void appSysProc_Opfu_Set(void);
void appMain_Task(void *pParameters);
WORD appSysProc_Card_Present_Get(void);
WORD appSysProc_Card_Power_Get(void);
void appSysProc_Card_Power_Set(WORD wEnable);
WORD appSysProc_Card_Power_Status_Get(void);



#endif /* _APPSYSMON_H_ */
