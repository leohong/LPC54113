#ifndef BOARD_TIMER_H
#define BOARD_TIMER_H
// ==============================================================================
// FILE NAME: BOARD_TIMER.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 21/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"


typedef void (*fpTIMERCALLBACK)(void *);
typedef void (*fpTIMER3CHANNELCALLBACK)(void); //A35LS_Larry_0003

typedef enum
{
    eBOARD_TIMER_TIMER0,
    eBOARD_TIMER_TIMER1,
    eBOARD_TIMER_TIMER2,
    eBOARD_TIMER_TIMER3,

    eBOARD_TIMER_NUMBERS,
} eBOARD_TIMER;

typedef enum
{
    eBOARD_TIMER_MODE_TIMER,
    eBOARD_TIMER_MODE_COUNTER,
    eBOARD_TIMER_MODE_CAPTURE,
    eBOARD_TIMER_MODE_MATCH,

    eBOARD_TIMER_MODE_NUMBERS,
} eBOARD_TIMER_MODE;

typedef enum
{
    eBOARD_CAP_MODE_TIMER = TIMER_CAPSRC_RISING_PCLK,
    eBOARD_CAP_MODE_RISE =  TIMER_CAPSRC_RISING_CAPN,
    eBOARD_CAP_MODE_FALL =  TIMER_CAPSRC_FALLING_CAPN,
    eBOARD_CAP_MODE_BOTH =  TIMER_CAPSRC_BOTH_CAPN,

    eBOARD_CAP_MODE_NUMBERS,
} eBOARD_CAP_MODE;

typedef enum
{
    eBOARD_PIN_MAT_STATE_NONE   = TIMER_EXTMATCH_DO_NOTHING,
    eBOARD_PIN_MAT_STATE_CLEAR  = TIMER_EXTMATCH_CLEAR,
    eBOARD_PIN_MAT_STATE_SET    = TIMER_EXTMATCH_SET,
    eBOARD_PIN_MAT_STATE_TOGGLE = TIMER_EXTMATCH_TOGGLE,

    eBOARD_PIN_MAT_STATE_NUMBERS,
} eBOARD_PIN_MAT_STATE;

typedef enum
{
    eBOARD_TIMER_CHANNLE_0,
    eBOARD_TIMER_CHANNLE_1,
    eBOARD_TIMER_CHANNLE_2,
    eBOARD_TIMER_CHANNLE_3,

    eBOARD_TIMER_CHANNLE_NUMBERS,
} eBOARD_TIMER_CHANNLE;

typedef struct
{
    eBOARD_TIMER_CHANNLE    eTimerChannel;      // Timer Input Channel
    DWORD                   dwPeriod;           // Timer Period
} sBOARD_TIMER_TIMER_CFG;

typedef struct
{
    eBOARD_TIMER_CHANNLE    eCounterChannel;    // Timer Input Channel
    eBOARD_CAP_MODE         eCounterMode;       // Counter Mode
} sBOARD_TIMER_COUNTER_CFG;

typedef struct
{
    eBOARD_TIMER_CHANNLE    eMatchChannel;      // Timer Input Channel
    BYTE                    cIntOnMatch;        // Enable Interrupt
    DWORD                   dwMatchValue;	    // Match value
    BYTE                    cStopOnMatch;       // Stop On match
    BYTE                    cResetOnMatch;      // Reset On match
    eBOARD_PIN_MAT_STATE    eExtMatchOutput;    // External Pin Match Output
} sBOARD_TIMER_MATCH_CFG;

typedef struct
{
    eBOARD_TIMER_CHANNLE    eCaptureChannel;	// Capture Channel
    TIMER_CAP_SRC_STATE_T   eCaptureMode;       // Capture Mode
} sBOARD_TIMER_CAPTURE_CFG;

typedef struct
{
    eBOARD_TIMER        eTimerId;               // Timer Id
    eBOARD_TIMER_MODE   eTimerMode;             // Timer Mode
    DWORD               dwPrescale;             // Pre-scale value
    BYTE                cEnableInt;             // Enable Interrupt
    BYTE                cPreemption;
    BYTE                cSubPriority;

    union
    {
        sBOARD_TIMER_TIMER_CFG      sTimerCfg;
        sBOARD_TIMER_COUNTER_CFG    sCounterCfg;
        sBOARD_TIMER_CAPTURE_CFG    sCaptureCfg;
        sBOARD_TIMER_MATCH_CFG      sMatchCfg;
    } uModeCfg;
} sBOARD_TIMER_CFG;

void Board_Timer_Init(sBOARD_TIMER_CFG *psTimerCfg);
eRESULT Board_Timer_Read(eBOARD_TIMER eTimer, DWORD *pdwValue);
DWORD Board_Timer_ReadCount(eBOARD_TIMER eTimer);
void Board_Timer_Register_Callback(eBOARD_TIMER eTimer, fpTIMERCALLBACK fpTimerCallBack);
//void Board_Timer_DeInit(sBOARD_TIMER_CFG *psTimerCfg);
void Board_Timer_Reset(eBOARD_TIMER eTimer);
void Board_Timer_DeInit(eBOARD_TIMER eTimer);
void Board_Timer_Set_Count_Src(eBOARD_TIMER eTimer, eBOARD_CAP_MODE eMode, eBOARD_TIMER_CHANNLE eChannel);
void Board_Timer_Reset_Counter(eBOARD_TIMER eTimer);
void Board_Timer_RGU_Reset(eBOARD_TIMER eTimer);
void Board_Timer_Match_Set(sBOARD_TIMER_CFG *psTimerCfg, DWORD dwValue);
void Board_Timer_Timer3Channel_Callback(eBOARD_TIMER_CHANNLE eChannel, fpTIMER3CHANNELCALLBACK fpTimerCallBack);

#endif /* BOARD_TIMER_H */

