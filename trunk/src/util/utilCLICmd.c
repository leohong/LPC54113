// ==============================================================================
// FILE NAME: UTILCLICMD.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2014/02/06, Leo Create
// --------------------
// ==============================================================================


#include "Release.h"
#include "ModeInfo.h"
#include "dvCard.h"
#include "utilLogAPI.h"
#include "utilCLICmdAPI.h"
#include "utilEventHandlerAPI.h"
#include "halBoardCtrlAPI.h"
#include "halHdmiSwitcher.h"
#include "halCard.h"
#include "appIapProcAPI.h"
#include "appSysmon.h"
#include "appIapProcAPI.h"

// ==============================================================================
// FUNCTION NAME: utilCLI_System_Version_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/11, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_System_Version_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT eResult = rcERROR;
    BYTE *pcData = (BYTE *)psOutData;

    LOG_MSG(eDB_MSK_LIST_CLI, "System_Version_Get\r\n");

    pcData[0] = VER_MAJOR;
    pcData[1] = VER_MINOR;

    pcData[2] = eINPUT_CARD_ID_GIM_STANDARD; //Card_ID for GIM detection
    pcData[3] = eINPUT_HARDWARE_VERSION_R1;  //Hardware_Version for GIM detection

    eResult = rcSUCCESS;
    *pwByteCount = 4;
    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_System_Card_Present_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/21, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_System_Card_Present_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT eResult = rcERROR;
    WORD *pwData = (WORD *)psOutData;

    LOG_MSG(eDB_MSK_LIST_CLI, "System_Card_Present_Get\r\n");

    *pwData = appSysProc_Card_Present_Get();
    *pwByteCount = sizeof(WORD);

    eResult = rcSUCCESS;
    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_System_Ready_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/21, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_System_Ready_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT eResult = rcERROR;
    BYTE *pcData = (BYTE *)psOutData;


    if(eSYS_STATE_POWER_NORMAL == appSysProc_State_Get())
    {
        *pcData = TRUE;
    }
    else
    {
        *pcData = FALSE;
    }

    LOG_MSG(eDB_MSK_LIST_CLI, "System_Ready_Get %d\r\n", *pcData);

    *pwByteCount = sizeof(BYTE);

    eResult = rcSUCCESS;
    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_System_Slot_Power_Set
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/21, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_System_Slot_Power_Set(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    WORD *pwEnable = (WORD *)psInData->acData;
    sEVENT_DATA sEventData;

    *pwEnable = (*pwEnable & ALL_SLOT_MASK);
    LOG_MSG(eDB_MSK_LIST_CLI, "System_Slot_Power_Set 0x%02X\r\n", *pwEnable);

    sEventData.uEVET_DATA.awData[0] = eSYSTEM_EVENT_CARD_POWER;
    sEventData.uEVET_DATA.awData[1] = *pwEnable;
    utilEvent_Send(eEVENT_LIST_POWER, 4, &sEventData.uEVET_DATA);

    *pwByteCount = 0;

    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_System_Card_Power_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/21, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_System_Card_Power_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT eResult = rcERROR;
    WORD *pwData = (WORD *)psOutData;

    *pwData = appSysProc_Card_Power_Get();
    LOG_MSG(eDB_MSK_LIST_CLI, "System_Card_Power_Get %d\r\n", *pwData);

    *pwByteCount = sizeof(WORD);

    eResult = rcSUCCESS;
    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_System_Slot_Info_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/12/10, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_System_Slot_Info_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT eResult = rcERROR;
    sSLOT_INFO *psInfo = (sSLOT_INFO *)psOutData;

    *psInfo = halCard_Slot_Info_Get();
    LOG_MSG(eDB_MSK_LIST_CLI, "Slot Info %d, %s\r\n", psInfo->cInputs, psInfo->acInputName);
    *pwByteCount = sizeof(sSLOT_INFO);

    eResult = rcSUCCESS;
    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_System_Slot_Ready_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/12/10, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_System_Slot_Ready_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT eResult = rcERROR;
    BYTE *pcData = (BYTE *)psOutData;

    *pcData = halCard_Slot_Ready_Get();
    LOG_MSG(eDB_MSK_LIST_CLI, "Slot Ready %d\r\n", *pcData);
    *pwByteCount = sizeof(BYTE);

    eResult = rcSUCCESS;
    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_Go_To_Bootloader
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2020/03/23, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_Go_To_Bootloader(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    sEVENT_DATA sEventData;

    LOG_MSG(eDB_MSK_LIST_CLI, "Go_To_Bootloader\r\n");

    sEventData.uEVET_DATA.awData[0] = eIAP_CMD_GO_BOOTLOADER;
    utilEvent_Send(eEVENT_LIST_IAP, 2, &sEventData.uEVET_DATA);

    *pwByteCount = 0;

    return rcSUCCESS;

}

// ==============================================================================
// FUNCTION NAME: utilCLI_Input_Source_Set
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/11, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_Input_Source_Set(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT eResult = rcERROR;

    if( eSOURCE_LIST_NUMBERS > psInData->acData[0])
    {
        sEVENT_DATA sEventData;

        sEventData.uEVET_DATA.wData = eSWITCHER_CTRL_SWITCH_INPUT;
        sEventData.uEVET_DATA.acEventData[2] = psInData->acData[0];
        utilEvent_Send(eEVENT_LIST_HDMI_SWITCHER, 3, &sEventData.uEVET_DATA);
        LOG_MSG(eDB_MSK_LIST_CLI, "Input_Source_Set %d\r\n", sEventData.uEVET_DATA.acEventData[2]);

        *pwByteCount = 0;

        eResult = rcSUCCESS;
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_Input_Video_Info_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/11, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_Input_Video_Info_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT     eResult = rcERROR;
    //sVIDEO_INFO *psVideoInfo = (sVIDEO_INFO*)psOutData;
    //*psVideoInfo = NULL;
    *pwByteCount = sizeof(sVIDEO_INFO);

    eResult = rcSUCCESS;

 
    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_Input_Video_Timing_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/19, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_Input_Video_Timing_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT         eResult = rcERROR;
    //sVIDEO_TIMING   *psVideoTiming = (sVIDEO_TIMING*)psOutData;
    //*psVideoTiming = NULL;
    *pwByteCount = sizeof(sVIDEO_TIMING);

    eResult = rcSUCCESS;

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_Input_Video_Format_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/19, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_Input_Video_Format_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT         eResult = rcERROR;
    //sVIDEO_FORMAT   *psVideoFormat = (sVIDEO_FORMAT*)psOutData;
    //*psVideoFormat = NULL;
    *pwByteCount = sizeof(sVIDEO_FORMAT);

    eResult = rcSUCCESS;

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_Input_AudioInfo_Get
// DESCRIPTION:
//
//
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/11, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_Input_AudioInfo_Get(sPAYLOAD *psInData,  sPAYLOAD *psOutData, WORD *pwByteCount)
{
    eRESULT     eResult = rcERROR;
    //sAUDIO_INFO *psAudioInfo = (sAUDIO_INFO*)psOutData;
    //*psAudioInfo = NULL;
    *pwByteCount = sizeof(sAUDIO_INFO);

    eResult = rcSUCCESS;

    return eResult;
}

static eRESULT utilCLI_OPS_Power_Set(sPAYLOAD *psInData,  sPAYLOAD *psOutData, WORD *pwByteCount)
{
    sEVENT_DATA sEventData;

    LOG_MSG(eDB_MSK_LIST_CLI, "OPS Power Toggle\n");

    sEventData.uEVET_DATA.awData[0] = eSYSTEM_EVENT_OPS_POWER;
    utilEvent_Send(eEVENT_LIST_POWER, 2, &sEventData.uEVET_DATA);

    *pwByteCount = 0;

    return rcSUCCESS;
}

static eRESULT utilCLI_OPS_Power_Get(sPAYLOAD *psInData,  sPAYLOAD *psOutData, WORD *pwByteCount)
{
    BYTE *pcData = (BYTE *)psOutData;

    *pcData = !!(appSysProc_Card_Power_Status_Get() & OPS_MASK);

    LOG_MSG(eDB_MSK_LIST_CLI, "OPS Power Get %d\r\n", *pcData);
    *pwByteCount = sizeof(BYTE);
    return rcSUCCESS;
}

// ==============================================================================
// End Of Static Function
// ==============================================================================
#define CMD_ID(MODULE, SUBCMD, READ) (((READ << 7)|MODULE)<<8|SUBCMD)

static const sCMDCFG m_asCmdLut[] =
{
    //System
    { CMD_ID(eCMD_MODULE_SYSTEM,        eCMD_SYSTEM_VERSION,            CMD_READ),  utilCLI_System_Version_Get      },
    { CMD_ID(eCMD_MODULE_SYSTEM,        eCMD_SYSTEM_CARD_PRESENT,       CMD_READ),  utilCLI_System_Card_Present_Get },
    { CMD_ID(eCMD_MODULE_SYSTEM,        eCMD_SYSTEM_READY,              CMD_READ),  utilCLI_System_Ready_Get        },
    { CMD_ID(eCMD_MODULE_SYSTEM,        eCMD_SYSTEM_CARD_POWER,         CMD_WRITE), utilCLI_System_Slot_Power_Set   },
    { CMD_ID(eCMD_MODULE_SYSTEM,        eCMD_SYSTEM_CARD_POWER,         CMD_READ),  utilCLI_System_Card_Power_Get   },
    { CMD_ID(eCMD_MODULE_SYSTEM,        eCMD_SYSTEM_SLOT_INFO,          CMD_READ),  utilCLI_System_Slot_Info_Get    },
    { CMD_ID(eCMD_MODULE_SYSTEM,        eCMD_SYSTEM_SLOT_READY,         CMD_READ),  utilCLI_System_Slot_Ready_Get   },
    { CMD_ID(eCMD_MODULE_SYSTEM,        eCMD_SYSTEM_OPS_POWER,          CMD_WRITE), utilCLI_OPS_Power_Set           },
    { CMD_ID(eCMD_MODULE_SYSTEM,        eCMD_SYSTEM_OPS_POWER,          CMD_READ),  utilCLI_OPS_Power_Get           },

    //Input Video Source
    { CMD_ID(eCMD_MODULE_SOURCE_SWITCH, eCMD_SOURCE_PORT_SWITCH,        CMD_WRITE), utilCLI_Input_Source_Set        },

    { CMD_ID(eCMD_MODULE_SOURCE_VIDEO,  eCMD_SOURCE_VIDEO_INFO,         CMD_READ),  utilCLI_Input_Video_Info_Get    },
    { CMD_ID(eCMD_MODULE_SOURCE_VIDEO,  eCMD_SOURCE_VIDEO_TIMING,       CMD_READ),  utilCLI_Input_Video_Timing_Get  },
    { CMD_ID(eCMD_MODULE_SOURCE_VIDEO,  eCMD_SOURCE_VIDEO_FORMAT,       CMD_READ),  utilCLI_Input_Video_Format_Get  },

    //Input Audio Source
    { CMD_ID(eCMD_MODULE_SOURCE_AUDIO,  eCMD_SOURCE_AUDIO_INFO,         CMD_READ),  utilCLI_Input_AudioInfo_Get     },

    // IAP
    { CMD_ID(eCMD_MODULE_IAP,           eIAP_CMD_GO_BOOTLOADER,         CMD_WRITE), utilCLI_Go_To_Bootloader        },
};

#define SIZE_OF_CMD_LUT sizeof(m_asCmdLut)/sizeof(m_asCmdLut[0])
// ==============================================================================
// FUNCTION NAME: UTILCLICMD_EXECUTE
// DESCRIPTION:
// Params:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
// BYTE cRead:
// Returns:
// modification history: 2018/06/31, chris
// ==============================================================================
eACK_TYPE utilCLICmd_Execute(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount, BYTE cRead)
{
    eRESULT eResult = rcERROR;
    WORD wCmdIndex = 0;
    WORD wCmd = 0;
    BYTE cModule = 0;

    cModule = ((cRead&0x01) << 7 | psInData->cModule);

    wCmd = (cModule << 8) | psInData->cSubCmd;

    for(wCmdIndex = 0; wCmdIndex < SIZE_OF_CMD_LUT; wCmdIndex++)
    {
        if(m_asCmdLut[wCmdIndex].wCmdIndex == wCmd)
        {
            ASSERT(m_asCmdLut[wCmdIndex].pFunc != NULL);

            if(m_asCmdLut[wCmdIndex].pFunc != NULL)
            {
                eResult = m_asCmdLut[wCmdIndex].pFunc(psInData, psOutData, pwByteCount);
                //LOG_MSG(eDB_MSK_LIST_CLI, "Module %d,SubCmd %d\r\n", (psInData->cModuleId), psInData->cSubCmd);
                break;
            }
        }
    }

    // 查無指令
    if(wCmdIndex >= SIZE_OF_CMD_LUT)
    {
        LOG_MSG(eDB_MSK_LIST_CLI, "Unknow: %d, %d\n", cModule, psInData->cSubCmd);
        return eACK_TYPE_UNKNOWN;
    }
    else
    {
        if(rcSUCCESS == eResult)
        {
            // 有資料需要回復
            if(*pwByteCount > 0)
            {

                return eACK_TYPE_FEEDBACK;
            }
            else
            {
                // 回應是否有收到
                return eACK_TYPE_ACK;
            }
        }
        else
        {
            return eACK_TYPE_ERROR;
        }
    }
}

