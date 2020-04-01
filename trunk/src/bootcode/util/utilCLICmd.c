// ==============================================================================
// FILE NAME: UTILCLICMD.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2019/06/26, Leo Create
// --------------------
// ==============================================================================

// #define _DEBUG_I2C_CMD_

#include "Release.h"
#include "Board_Gpio.h"
#include "Board_uart.h"

#include "utilLogAPI.h"
#include "utilHostAPI.h"
#include "utilCLICmdAPI.h"
#include "utilCounterAPI.h"
#include "utilMathAPI.h"
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
    BYTE *pcData = (BYTE *)psOutData;

    LOG_MSG(eDB_MSK_LIST_CLI, "System_Version_Get\r\n");

    pcData[0] = VER_MAJOR;
    pcData[1] = VER_MINOR;
    pcData[2] = 0;
    pcData[3] = 0;
    *pwByteCount = 4;
    
    return rcSUCCESS;
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
    BYTE *pcData = (BYTE *)psOutData;
    *pcData = TRUE;
    *pwByteCount = sizeof(BYTE);

    return rcSUCCESS;
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
// Modification History
// --------------------
// 2020/03/24, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_Go_To_Bootloader(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    *pwByteCount = 0;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_AppVersion_Get
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
// Modification History
// --------------------
// 2019/06/26, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_AppVersion_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_AppCode_Version_Get((DWORD*)psOutData);
    *pwByteCount = 4;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_AppVersion_Set
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
// Modification History
// --------------------
// 2019/08/02, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_AppVersion_Set(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_AppCode_Version_Set((DWORD*)psInData->acData);
    *pwByteCount = 0;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_IAP_Enable_Set
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
// Modification History
// --------------------
// 2019/06/26, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_IAP_Enable_Set(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_IAP_Enable();
    *pwByteCount = 0;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_IAP_Enable_Set
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
// Modification History
// --------------------
// 2019/06/26, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_IAP_APP_INFO_Set(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_AppBinInfo_Set((DWORD*)&psInData->acData[0], (DWORD*)&psInData->acData[4]);
    *pwByteCount = 0;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_IAP_Bin_Address_Set
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
// Modification History
// --------------------
// 2019/06/26, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_IAP_Bin_Address_Set(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_Bin_Address((DWORD *)psInData->acData);
    *pwByteCount = 0;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_IAP_Bin_Data_Set
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
// Modification History
// --------------------
// 2019/06/26, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_IAP_Bin_Data_Set(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_Bin_Data((BYTE *)psInData->acData);
    *pwByteCount = 0;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_IAP_Bin_Data_Program_Set
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
// Modification History
// --------------------
// 2019/06/26, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_IAP_Bin_Data_Program_Set(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_Programing((DWORD*)psInData->acData);
    *pwByteCount = 0;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_IAP_Bin_Data_Checksum_Get
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
// Modification History
// --------------------
// 2019/08/02, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_IAP_Bin_Data_Checksum_Get(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_CheckSum_Get((DWORD *)psOutData);
    *pwByteCount = 4;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_IAP_Write_Tag
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
// Modification History
// --------------------
// 2019/06/26, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_IAP_Program_Finish_Set(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_Programing_Finish((DWORD*)&psInData->acData[0], (DWORD*)&psInData->acData[4]);
    *pwByteCount = 0;
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: utilCLI_IAP_Enable_Set
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
// Modification History
// --------------------
// 2019/06/26, Leo Create
// --------------------
// ==============================================================================
static eRESULT utilCLI_IAP_Run_App_Set(sPAYLOAD *psInData,  sPAYLOAD *psOutData, WORD *pwByteCount)
{
    appIapProc_Run_App_Flag_Set(TRUE);
    *pwByteCount = 0;
    return rcSUCCESS;
}

// ==============================================================================
// End Of Static Function
// ==============================================================================
#define CMD_ID(MODULE, SUBCMD, READ) (((READ << 7)|MODULE)<<8|SUBCMD)

static const sCMDCFG m_asCmdLut[] =
{
    { CMD_ID(eCMD_MODULE_SYSTEM,    eCMD_SYSTEM_VERSION,        CMD_READ),      utilCLI_System_Version_Get      },
    { CMD_ID(eCMD_MODULE_SYSTEM,    eCMD_SYSTEM_READY,          CMD_READ),      utilCLI_System_Ready_Get        },
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_GO_TO_BOOTLOADER,  CMD_WRITE),     utilCLI_Go_To_Bootloader},
    // IAP  
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_APP_VER,           CMD_READ),      utilCLI_AppVersion_Get},
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_APP_VER,           CMD_WRITE),     utilCLI_AppVersion_Set},
    
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_IAP_ENABLE,        CMD_WRITE),     utilCLI_IAP_Enable_Set},
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_APP_INFO,          CMD_WRITE),     utilCLI_IAP_APP_INFO_Set},
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_BIN_ADDRESS,       CMD_WRITE),     utilCLI_IAP_Bin_Address_Set},
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_BIN_DATA,          CMD_WRITE),     utilCLI_IAP_Bin_Data_Set},
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_BIN_CHECK_SUM,     CMD_READ),      utilCLI_IAP_Bin_Data_Checksum_Get},
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_PROGRAMING,        CMD_WRITE),     utilCLI_IAP_Bin_Data_Program_Set},
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_PROGRAMING_FINISH, CMD_WRITE),     utilCLI_IAP_Program_Finish_Set},
    { CMD_ID(eCMD_MODULE_IAP,       eIAP_CMD_RUN_APP,           CMD_WRITE),     utilCLI_IAP_Run_App_Set},
};

#define SIZE_OF_CMD_LUT sizeof(m_asCmdLut)/sizeof(m_asCmdLut[0])

// ==============================================================================
// FUNCTION NAME: UTILCLICMD_EXECUTE
// DESCRIPTION:
//
//
// Params:
// BYTE cExeWrite:
// sPAYLOAD *psInData:
// sPAYLOAD *psOutData:
// WORD *pwByteCount:
// BYTE cRead:
// Returns:
//
//
// modification history
// --------------------
// 2020/03/18, Leo Create
// --------------------
// ==============================================================================
eACK_TYPE utilCLICmd_Execute(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount, BYTE cRead)
{
    eRESULT eResult = rcERROR;
    WORD wCmdIndex = 0;
    WORD wCmd = 0;
    BYTE cModule = 0;

    cModule = ((cRead&0x01) << 7 | psInData->cModule);

    wCmd = (cModule << 8) | psInData->cSubCmd;
    LOG_MSG(eDB_MSK_LIST_CLI, "%X %X\n",psInData->cModule,  psInData->cSubCmd);

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
        LOG_MSG(eDB_MSK_LIST_CLI, "%X %X\n",psInData->cModule,  psInData->cSubCmd);
        ASSERT_ALWAYS();
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

