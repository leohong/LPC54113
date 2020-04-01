#ifndef HALINPUTCTRLAPI_H
#define HALINPUTCTRLAPI_H
// ==============================================================================
// FILE NAME: HALINPUTCTRLAPI.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================


#include "CommonDef.h"

#define IR_BIT_SIZE             1       // Bit(High/Low)
#define IR_HEADER_SIZE          1       // Start pulse 9mS High 4.5mS Low
#define IR_BYTE_SIZE            8       // Total bits per data byte(High/Low)
#define IR_DATA_SIZE            32      // Total bits not counting header
#define IR_NOISE                1       // Noise

#define IR_PACKAGE_SIZE         (IR_HEADER_SIZE+IR_DATA_SIZE)
#define IR_BUFFER_SIZE          (128)

#define LEADER_MIN              (1282)   // Expecting 13.5ms +- 5%
#define LEADER_MAX              (1417)

#define BIT_1_MIN               (213)    // Expecting 2.25ms +- 5%
#define BIT_1_MAX               (236)

#define BIT_0_MIN               (106)     // Expecting 1.125ms +- 5%
#define BIT_0_MAX               (117)

#define REPEAT_MIN              (1092)    // Expecting 1.15ms +- 5%
#define REPEAT_MAX              (1207)

typedef enum
{
    eIR_STATE_HEAD,
    eIR_STATE_DATA,

    eIR_STATE_NUMBER,
} eIR_STATE;

typedef struct
{
    WORD wKeyCode;
    BYTE cConditions;
    BYTE cPressKey;
    BYTE cHoldKey;
    BYTE cReleaseKey;
    BYTE cHoldCount;    //Threshold for HOLD or REPEAT event
} sKEYLUT;

typedef enum
{
    eKEY_EVENT_PRESSED = 0x01 << 0,
    eKEY_EVENT_HOLD    = 0x01 << 1,
    eKEY_EVENT_RELEASE = 0x01 << 2,

    eKEY_EVENT_NUMBER,
} eKEY_EVENT;

typedef enum
{
    eKEYINPUT_KEYPAD,
    eKEYINPUT_IR,

    eKEYINPUT_NUMBER,
} eKEY_INPUT_TYPE;

typedef struct
{
    WORD            wKeyCode;
    eKEY_EVENT      eKeyEvent;
    eKEY_INPUT_TYPE eKeyType;
} sKEY_DATA;

WORD halInputCtrl_KeyPadGet(void);
void halInputCtrl_Proc(sKEY_DATA *psKeyData, BYTE *pcHoldCnt);
void halInputCtrl_KeyPad_Decode(void);

eRESULT halInputCtrl_Init(void);
void halInputCtrl_IR_Decode(void);
eRESULT halInputCtrl_IR_HeadGet(void);
eRESULT halInputCtrl_IR_DataGet(void);
void halInputCtrl_IR_Flush(void);
void halInputCtrl_IR_Restart(void);
void halInputCtrl_IR_CustomId_Set(BYTE cCustomIdIndex);
void halInputCtrl_Register_Callback(eRESULT(*fpCallback)(void *));
void halInputCtrl_IR_PulseInsert(void *pvBuffer);
void halInputCtrl_Detect_ISP_Key(BYTE cEnable);


#endif // HALINPUTCTRLAPI_H

