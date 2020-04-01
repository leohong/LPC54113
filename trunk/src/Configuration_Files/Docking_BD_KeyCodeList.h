#ifndef LH_KEYCODELIST_H
#define LH_KEYCODELIST_H
// ==============================================================================
// FILE NAME: LH_KEYCODELIST.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong written
// --------------------
// ==============================================================================

#include "halInputCtrlAPI.h"

#define KEY_POWER       (~(0x0001<<0))
#define KEY_SOURCE      (~(0x0001<<1))
#define KEY_BRIGHT_UP   (~(0x0001<<2))
#define KEY_BRIGHT_DOWN (~(0x0001<<3))

typedef enum
{
    //Press Key
    kePOWERON,

    //Hold Key
    keHOLD_POWERON,

    //Release Key
    keRELEASE_POWERON,

    eKEY_LIST_NUMBER,
} eKEY_LIST;

#define CUSTOM_ID_COUNT     (2)
#define DEFAULT_ID_NUMBER   (0)
#define REPEAT_THRESHOLD    (0)

static const WORD m_awCustomCode[CUSTOM_ID_COUNT] =
{
    0x4F52, // New Remote Custom ID
    0x4F50, // New Remote Custom ID
};

//KeyPad code
static const sKEYLUT m_acKeyPadLUT[] =
{
    //key code
    //key Condition
    //Press key
    //Hold key
    //Release key
    //Threshold for HOLD or REPEAT event
    {
        KEY_POWER,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        kePOWERON, keHOLD_POWERON, keRELEASE_POWERON,
        REPEAT_THRESHOLD,
    },
};

#define KEYPADNUMBER sizeof(m_acKeyPadLUT)/sizeof(sKEYLUT)

//IR Key code
static const sKEYLUT m_acIRKeyLUT[] =
{
    //key code
    //key Condition
    //Press key
    //Hold key
    //Release key
    {
        0x0002,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        kePOWERON, keHOLD_POWERON, keRELEASE_POWERON,
        REPEAT_THRESHOLD,
    },
};

#define IRKEYPADNUMBER sizeof(m_acIRKeyLUT)/sizeof(sKEYLUT)

#endif // LH_KEYCODELIST_H

