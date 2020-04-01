#ifndef DVCARD_H
#define DVCARD_H
// ===============================================================================
// FILE NAME: dvCard.h
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2019/10/30, Leo Create
// --------------------
// ===============================================================================


#include "CommonDef.h"
#include "utilCLICmdAPI.h"
#include "ModeInfo.h"

//sub Cmd for eCMD_MODULE_SYSTEM
typedef enum
{
    eCMD_SYSTEM_READY,
    eCMD_SYSTEM_VERSION,
    eCMD_SYSTEM_STATUS,
    eCMD_SYSTEM_STATUS_CLEAR,
    eCMD_SYSTEM_HPD_PULL,
    eCMD_SYSTEM_ERROR,
    eCMD_SYSTEM_CARD_PRESENT,
    eCMD_SYSTEM_CARD_POWER,
    eCMD_SYSTEM_SLOT_INFO,
    eCMD_SYSTEM_SLOT_READY,
    eCMD_SYSTEM_OPS_POWER,

    eCMD_SYSTEM_NUMBER,
} eCMD_SYSTEM;

//sub Cmd for eCMD_MODULE_SOURCE_SWITCH
typedef enum
{
    eCMD_SOURCE_PORT_SWITCH,
    eCMD_SOURCE_HDMI_DHCP,
    eCMD_SOURCE_DP_HDCP,
    eCMD_SOURCE_PORT_STATUS,
    eCMD_SOURCE_SCRAMBLING_STATUS,

    eCMD_SOURCE_SWITCH_NUMBER,
} eCMD_SOURCE_SWITCH;


//sub Cmd for eCMD_MODULE_SOURCE_VIDEO
typedef enum
{
    eCMD_SOURCE_VIDEO_TIMING,
    eCMD_SOURCE_VIDEO_INTERLACE,
    eCMD_SOURCE_VIDEO_COLOR_DEPTH,
    eCMD_SOURCE_VIDEO_COLOR_SAMPLING,
    eCMD_SOURCE_VIDEO_COLOR_RANGE,
    eCMD_SOURCE_VIDEO_FORMAT,
    eCMD_SOURCE_VIDEO_INFO,

    eCMD_SOURCE_VIDEO_NUMBER,
} eCMD_SOURCE_VIDEO;

//sub Cmd for eCMD_MODULE_SOURCE_AUDIO
typedef enum
{
    eCMD_SOURCE_AUDIO_INFO,
    eCMD_SOURCE_AUDIO_CHANNEL_COUNT,
    eCMD_SOURCE_AUDIO_SAMPLING_SIZE,
    eCMD_SOURCE_AUDIO_SAMPLING_FREQUENCY,

    eCMD_SOURCE_AUDIO_NUMBER,
} eCMD_SOURCE_AUDIO;

//sub Cmd for eCMD_MODULE_SOURCE_EDID
typedef enum
{
    eCMD_EDID_HDMI_RESOLUTION,
    eCMD_EDID_DP_RESOLUTION,
    eCMD_EDID_HDMI_FRAME_RATE,
    eCMD_EDID_DP_FRAME_RATE,
    eCMD_EDID_HDMI_DEEP_COLOR,
    eCMD_EDID_DP_DEEP_COLOR,
    eCMD_EDID_AUDIO_SUPPORT,
    eCMD_EDID_HDMI_TIMING,
    eCMD_EDID_DP_TIMING,
    eCMD_EDID_HDMI_OSD_SETTINGS,
    eCMD_EDID_DP_OSD_SETTINGS,
    eCMD_EDID_HDMI_WRITE,
    eCMD_EDID_DP_WRITE,
    eCMD_EDID_HDMI_READ,
    eCMD_EDID_DP_READ,

    eCMD_EDID_NUMBER,
} eCMD_EDID;

//sub Cmd for eCMD_MODULE_SOURCE_SDI
typedef enum
{
    eCMD_SDI_AUDIO_MAP,
    eCMD_SDI_LEVEL_STREAM,

    eCMD_SDI_NUMBER,
} eCMD_SDI;

//sub Cmd for eCMD_MODULE_IAP
typedef enum
{
    eCMD_IAP_BOOT,
    eCMD_IAP_ERASE,
    eCMD_IAP_WRITE,
    eCMD_IAP_FINISH,
    eCMD_IAP_STATUS,

    eCMD_IAP_NUMBER,
} eCMD_IAP;

//Status
typedef enum
{
    eSOURCE_PORT_HDMI,
    eSOURCE_PORT_DP,
    eSOURCE_PORT_SDI,
    eSOURCE_PORT_NUMBER,

} eSOURCE_PORT;

typedef enum
{
    eSDI_AUDIO_MAP_CHANNEL1_2,
    eSDI_AUDIO_MAP_CHANNEL3_4,
    eSDI_AUDIO_MAP_CHANNEL5_6,
    eSDI_AUDIO_MAP_CHANNEL7_8,
    eSDI_AUDIO_MAP_CHANNEL_ALL,
    eSDI_AUDIO_MAP_CHANNEL_NUMBER,
} eSDI_AUDIO_MAP_STATUS;

typedef enum
{
    eINPUT_CARD_ID_GIM_STANDARD,
    //...TBD
    eMAX_INPUT_CARD_ID,
} eINPUT_CARD_ID;

typedef enum
{
    eINPUT_HARDWARE_VERSION_R1,
    //...TBD
    eMAX_INPUT_HARDWARE_VERSION,
} eINPUT_HARDWARE_VERSION;

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

typedef struct
{
    BYTE cInputs;
    BYTE acInputName[32];
} sSLOT_INFO;

#pragma pack(pop)   /* restore original alignment from stack */

void dvCard_Initial(void);
eRESULT dvCard_Command_Write(eCMD_MODULE eModule, BYTE cSubCmd, WORD wSize, BYTE *pcData);
eRESULT dvCard_Command_Read(eCMD_MODULE eModule, BYTE cSubCmd, WORD wSize, BYTE *pcData);
eRESULT dvCard_System_Revision_Get(BYTE *pcVersion);
eRESULT dvCard_System_Ready_Get(BYTE *pcReady);
eRESULT dvCard_System_Card_Present_Get(WORD *pwPresent);
eRESULT dvCard_System_Card_Power_Set(WORD wEnable);
eRESULT dvCard_System_Card_Power_Get(WORD *pwEnable);

eRESULT dvCard_Input_Port_Set(eSOURCE_PORT ePort);
eRESULT dvCard_Video_Info_Get(sVIDEO_INFO *psVideoInfo);
eRESULT dvCard_Video_Timing_Get(sVIDEO_TIMING *psVideoTiming);
eRESULT dvCard_Audio_Info_Get(sAUDIO_INFO *psAudioInfo);
eRESULT dvCard_Video_Format_Get(sVIDEO_FORMAT *psVideoFormat);
eRESULT dvCard_Slot_Info_Get(sSLOT_INFO *psSlotInfo);


#endif // DVCARD_H

