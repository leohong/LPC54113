#ifndef HALCARD_H
#define HALCARD_H
// ==============================================================================
// FILE NAME: halCard.h
// DESCRIPTION:
//
//
// modification history
// --------------------
// 19/12/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"
#include "dvCard.h"

typedef enum
{
    eDETECT_STATE_CARD_IDLE,
    eDETECT_STATE_CARD_INFO,
    eDETECT_STATE_CARD_DONE,
    
    eDETECT_STATE_NUMBERS,
} eDETECT_STATE;

void halCard_Init(void);
BYTE halCard_Card_Detect(void);
void halCard_OPS_Power_Toggle(void);
BYTE halCard_OPS_Power_On_Get(void);
BYTE halCard_Card_Ready(void);
sSLOT_INFO halCard_Slot_Info_Get(void);
void halCard_Slot_Ready_Check(void);
void halCard_Slot_Check_Set(void);
BYTE halCard_Slot_Ready_Get(void);
BYTE halCard_Slot_Input_Numbers_Get(void);
void halCard_OPS_Power_Set(void);
void halCard_OPS_Power_Check(void);

#endif /* HALCARD_H */

