#ifndef UTILCOUNTERAPI_H
#define UTILCOUNTERAPI_H
// ==============================================================================
// FILE NAME: UTILSYSCOUNTERAPI.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 19/03/2013, Leohong written
// --------------------
// ==============================================================================


#include "commondef.h"
//#include "Board_SystemTick.h"

#define ONE_SECOND  (1000)
#define ONE_MINUTE  (60 * ONE_SECOND)
#define ONE_HOUR    (60 * ONE_MINUTE)

typedef enum
{
    /* 00 */ eCOUNTER_TYPE_I2C_TIMEOUT,
    /* 01 */ eCOUNTER_TYPE_KEY,
    /* 02 */ eCOUNTER_TYPE_IRDECODE,
    /* 03 */ eCOUNTER_TYPE_CARD_DETECT,
    /* 04 */ eCOUNTER_TYPE_FUNCTION_TEST,
    /* 05 */ eCOUNTER_TYPE_CARD_WRITE,
    /* 06 */ eCOUNTER_TYPE_CARD_READ,
    /* 07 */ eCOUNTER_TYPE_I2C_0_TIMEOUT,
    /* 08 */ eCOUNTER_TYPE_I2C_1_TIMEOUT,
    /* 09 */ eCOUNTER_TYPE_SLOT_CHECK,
    /* 10 */ eCOUNTER_TYPE_FAN_CHECK,
    /* 11 */
    /* 12 */
    /* 13 */
    /* 14 */
    /* 15 */
    /* 16 */
    /* 17 */
    /* 18 */
    /* 19 */
    /* 20 */
    /* 21 */
    /* 22 */
    /* 23 */
    /* 24 */
    /* 25 */
    /* 26 */
    /* 27 */
    /* 28 */
    /* 29 */

    eCOUNTER_TYPE_NUMBERS,
} eCOUNTER_TYPE;

typedef enum
{
    eTIMER_EVENT_TYPE_LED_RED,
    eTIMER_EVENT_TYPE_LED_GREEN_1,
    eTIMER_EVENT_TYPE_LED_GREEN_2,

    eTIMER_EVENT_TYPE_NUMBERS,
} eTIMER_EVENT_TYPE;

typedef void (*fpTimerEventCallback)(void);

typedef struct
{
    DWORD dwTimer;
    DWORD dwReloadTimer;
    BYTE  cReload;
    WORD  wRepeatCount;
    fpTimerEventCallback fpEventCallback;
    fpTimerEventCallback fpEventFinishCallback;
} sTIMER_EVENT;

void utilCounter_Init(void);
void utilCounterSet(eCOUNTER_TYPE eCounter, DWORD dwMilliSecond);
DWORD utilCounterGet(eCOUNTER_TYPE eCounter);

void utilDelayMs(DWORD dwMilliSecond);

void utilCounter_Reg_Timer_Event(eTIMER_EVENT_TYPE eTimer,
                                 DWORD dwTimer,
                                 BYTE cReloard,
                                 WORD wRepeatCount,
                                 fpTimerEventCallback fpCallback,
                                 fpTimerEventCallback fpFinishCallback);

void utilCounter_UnReg_Timer_Event(eTIMER_EVENT_TYPE eTimer);
DWORD utilCounter_Tick_Get(void);
void utilCounter_Event_Process(void);


#endif // UTILCOUNTERAPI_H

