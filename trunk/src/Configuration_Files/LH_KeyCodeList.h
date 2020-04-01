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

#define KEY_UART_REDIR  (KEY_SOURCE & KEY_BRIGHT_DOWN)
#define KEY_RESTORE     (KEY_SOURCE&KEY_BRIGHT_UP&KEY_BRIGHT_DOWN)
#define KEY_TEST_MODE   (KEY_BRIGHT_UP&KEY_BRIGHT_DOWN)

typedef enum
{
    //Press Key
    kePOWERON,
    keBRIGHTNESS_UP,
    keMAIN_INPUT_NEXT,
    kePIP_INPUT_NEXT,
    keBRIGHTNESS_DOWN,
    kePIP_SIZE_UP,
    kePIP_POS_NEXT,
    kePIP_ON,
    kePIP_POS_PRE,
    kePIP_SIZE_DOWN,
    keVOL_UP,
    keVOL_DOWN,
    keVOL_MUTE,
    keHDMI_1,
    keHDMI_2,
    keVGA,
    ke3G_SDI,
    keHDBASE_T,
    keMEDIA_PLAYER,

    keRESTORE,
    keTEST_MODE,
    keUART_REDIR,

    //Hold Key
    keHOLD_POWERON,
    keHOLD_BRIGHTNESS_UP,
    keHOLD_MAIN_INPUT_NEXT,
    keHOLD_PIP_INPUT_NEXT,
    keHOLD_BRIGHTNESS_DOWN,
    keHOLD_PIP_SIZE_UP,
    keHOLD_PIP_POS_NEXT,
    keHOLD_PIP_ON,
    keHOLD_PIP_POS_PRE,
    keHOLD_PIP_SIZE_DOWN,
    keHOLD_VOL_UP,
    keHOLD_VOL_DOWN,
    keHOLD_VOL_MUTE,
    keHOLD_HDMI_1,
    keHOLD_HDMI_2,
    keHOLD_VGA,
    keHOLD_3G_SDI,
    keHOLD_HDBASE_T,
    keHOLD_MEDIA_PLAYER,

    keHOLD_RESTORE,
    keHOLD_TEST_MODE,
    keHOLD_UART_REDIR,

    //Release Key
    keRELEASE_POWERON,
    keRELEASE_BRIGHTNESS_UP,
    keRELEASE_MAIN_INPUT_NEXT,
    keRELEASE_PIP_INPUT_NEXT,
    keRELEASE_BRIGHTNESS_DOWN,
    keRELEASE_PIP_SIZE_UP,
    keRELEASE_PIP_POS_NEXT,
    keRELEASE_PIP_ON,
    keRELEASE_PIP_POS_PRE,
    keRELEASE_PIP_SIZE_DOWN,
    keRELEASE_VOL_UP,
    keRELEASE_VOL_DOWN,
    keRELEASE_VOL_MUTE,
    keRELEASE_HDMI_1,
    keRELEASE_HDMI_2,
    keRELEASE_VGA,
    keRELEASE_3G_SDI,
    keRELEASE_HDBASE_T,
    keRELEASE_MEDIA_PLAYER,

    keRELEASE_RESTORE,
    keRELEASE_TEST_MODE,
    keRELEASE_UART_REDIR,

    eKEY_LIST_NUMBER,
} eKEY_LIST;

#define CUSTOM_ID_COUNT     (2)
#define DEFAULT_ID_NUMBER   (0)
#define NO_REPEAT_THRESHOLD (0)

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
        NO_REPEAT_THRESHOLD,
    },
    {
        KEY_SOURCE,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keMAIN_INPUT_NEXT, keHOLD_MAIN_INPUT_NEXT, keRELEASE_MAIN_INPUT_NEXT,
        NO_REPEAT_THRESHOLD,
    },
    {
        KEY_BRIGHT_UP,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keBRIGHTNESS_UP, keHOLD_BRIGHTNESS_UP, keRELEASE_BRIGHTNESS_UP,
        NO_REPEAT_THRESHOLD,
    },
    {
        KEY_BRIGHT_DOWN,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keBRIGHTNESS_DOWN, keHOLD_BRIGHTNESS_DOWN, keRELEASE_BRIGHTNESS_DOWN,
        NO_REPEAT_THRESHOLD,
    },
    {
        KEY_RESTORE,
        eKEY_EVENT_RELEASE,
        eKEY_LIST_NUMBER, eKEY_LIST_NUMBER, keRELEASE_RESTORE,
        NO_REPEAT_THRESHOLD,
    },
    {
        KEY_TEST_MODE,
        eKEY_EVENT_RELEASE,
        eKEY_LIST_NUMBER, eKEY_LIST_NUMBER, keRELEASE_TEST_MODE,
        NO_REPEAT_THRESHOLD,
    },
    {
        KEY_UART_REDIR,
        eKEY_EVENT_RELEASE,
        eKEY_LIST_NUMBER, eKEY_LIST_NUMBER, keRELEASE_UART_REDIR,
        NO_REPEAT_THRESHOLD,
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
        0x0004,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        kePIP_POS_NEXT, keHOLD_PIP_POS_NEXT, keRELEASE_PIP_POS_NEXT,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x0002,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        kePOWERON, keHOLD_POWERON, keRELEASE_POWERON,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x0003,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        kePIP_SIZE_UP, keHOLD_PIP_SIZE_UP, keRELEASE_PIP_SIZE_UP,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x0011,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keMEDIA_PLAYER, keHOLD_MEDIA_PLAYER, keRELEASE_MEDIA_PLAYER,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x0029,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        kePIP_INPUT_NEXT, keHOLD_PIP_INPUT_NEXT, keRELEASE_PIP_INPUT_NEXT,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x0013,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keBRIGHTNESS_UP, keHOLD_BRIGHTNESS_UP, keRELEASE_BRIGHTNESS_UP,
        2,
    },
    {
        0x0016,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keVOL_DOWN, keHOLD_VOL_DOWN, keRELEASE_VOL_DOWN,
        2,
    },
    {
        0x0017,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        kePIP_ON, keHOLD_PIP_ON, keRELEASE_PIP_ON,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x0015,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keVOL_UP, keHOLD_VOL_UP, keRELEASE_VOL_UP,
        2,
    },
    {
        0x0014,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keBRIGHTNESS_DOWN, keHOLD_BRIGHTNESS_DOWN, keRELEASE_BRIGHTNESS_DOWN,
        2,
    },
    {
        0x000F,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keHDMI_1, keHOLD_HDMI_1, keRELEASE_HDMI_1,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x000B,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keHDMI_2, keHOLD_HDMI_2, keRELEASE_HDMI_2,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x0008,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keVGA, keHOLD_VGA, keRELEASE_VGA,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x000C,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        ke3G_SDI, keHOLD_3G_SDI, keRELEASE_3G_SDI,
        NO_REPEAT_THRESHOLD,
    },
    {
        0x0010,
        eKEY_EVENT_PRESSED | eKEY_EVENT_HOLD | eKEY_EVENT_RELEASE,
        keHDBASE_T, keHOLD_HDBASE_T, keRELEASE_HDBASE_T,
        NO_REPEAT_THRESHOLD,
    },
};

#define IRKEYPADNUMBER sizeof(m_acIRKeyLUT)/sizeof(sKEYLUT)

#endif // LH_KEYCODELIST_H

