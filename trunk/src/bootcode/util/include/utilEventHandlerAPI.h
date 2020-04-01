#ifndef UTILEVENTHANDLERAPI_H
#define UTILEVENTHANDLERAPI_H
// ==============================================================================
// FILE NAME: UTILEVENTHANDLERAPI.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2014/03/30, Leo Create
// --------------------
// ==============================================================================
#include "CommonDef.h"

#define RELEASE_KEY_NOTIFY          0x01
#define RELEASE_KEY_NOTIFY_SHIFT    1

typedef enum
{
    eSYSTEM_EVENT_POWER_ON = 0,
    eSYSTEM_EVENT_RESTORE_MODE,
    eSYSTEM_EVENT_UPDATE_MODE,
    eSYSTEM_EVENT_POWER_SUPPLY_READY,
    eSYSTEM_EVENT_POWER_NORMAL,
    eSYSTEM_EVENT_POWER_OFF,
    eSYSTEM_EVENT_TEST_MODE,
    eSYSTEM_EVENT_CARD_POWER,

    ePOWER_MODE_NUMBERS,
} ePOWER_MODE;

typedef enum
{
    eVIDEO_CTRL_MAIN_INPUT_NEXT,
    eVIDEO_CTRL_MAIN_INPUT_SELECT,
    eVIDEO_CTRL_PIP_INPUT_NEXT,
    eVIDEO_CTRL_PIP_INPUT_SELECT,
    eVIDEO_CTRL_BRIGHTNESS_UP,
    eVIDEO_CTRL_BRIGHTNESS_DOWN,
    eVIDEO_CTRL_BRIGHTNESS_SELECT,
    eVIDEO_CTRL_PIP_SIZE_UP,
    eVIDEO_CTRL_PIP_SIZE_DOWN,
    eVIDEO_CTRL_PIP_SIZE_SELECT,
    eVIDEO_CTRL_PIP_POS_UP,
    eVIDEO_CTRL_PIP_POS_DOWN,
    eVIDEO_CTRL_PIP_POS_SELECT,
    eVIDEO_CTRL_PIP_ON,
    eVIDEO_CTRL_PIP_ON_SELECT,
    eVIDEO_CTRL_VOLUME_UP,
    eVIDEO_CTRL_VOLUME_DOWN,
    eVIDEO_CTRL_VOLUME_SELECT,
    eVIDEO_CTRL_AUDIO_MUTE,
    eVIDEO_CTRL_FRAME_RATE,
    eVIDEO_CTRL_PHASE,
    eVIDEO_CTRL_FACTORY_RESET,

    eVIDEO_CTRL_NUMBERS,
} eVIDEO_CTRL;

typedef enum
{
    eKEY_CTRL_TEST_MODE_POWER_ON,
    eKEY_CTRL_TEST_EVENT,

    eKEY_CTRL_NUMBERS,
} eKEY_CTRL;

typedef enum
{
    eSWITCHER_CTRL_POWER_ON,
    eSWITCHER_CTRL_RESET_DRIVER,
    eSWITCHER_CTRL_SWITCH_INPUT,

    eSWITCHER_CTRL_FUNC_TEST,

    eSWITCHER_CTRL_NUMBERS,
} eSWITCHER_CTRL;

typedef enum
{
    eEVENT_LIST_POWER,
    eEVENT_LIST_VIDEO_PROCESSOR,
    eEVENT_LIST_KEY,

    eEVENT_LIST_HDMI_SWITCHER,

    eEVENT_LIST_NUMBERS,
} eEVENT_LIST;

typedef struct
{
    eEVENT_LIST eEvent;

    union
    {
        WORD    wData;
        WORD    awData[2];
        BYTE    acEventData[16];
    } uEVET_DATA;
} sEVENT_DATA;

void utilEventHandler_Init(void);
void utilEventHandler_Process(void);
void utilEvent_Send(eEVENT_LIST eEvent, WORD wSize, void *pvData);
void utilEvent_Send_Notify(DWORD dwNotifyValue);

#endif /* UTILEVENTHANDLERAPI_H */

