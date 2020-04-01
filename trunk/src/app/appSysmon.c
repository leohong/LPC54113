// ===============================================================================
// FILE NAME: appSysmon.c
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2018/10/28, Leo Create
// --------------------
// ===============================================================================


#include "CommonDef.h"
#include "Release.h"
#include "utilMathAPI.h"
#include "Board.h"
#include "Board_Uart.h"
#include "appSysmon.h"
#include "appInputProcAPI.h"
#include "appIapProcAPI.h"
#include "utilEventHandlerAPI.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"
#include "utilFuncTesterAPI.h"
#include "utilHostAPI.h"
#include "halCard.h"


static sSYS_CONFIG m_sSystemConfig;

// ==============================================================================
// FUNCTION NAME: appSysProc_Standby_Init
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_Standby_Init(void)
{
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Standby_Process
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_Standby_Process(void)
{
    appSysProc_State_Set(eSYS_STATE_GOTO_POWER_NORMAL);
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Goto_Power_Normal_Init
// DESCRIPTION:
//
// 1. Enable PSON and Check PSOK signal
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_Goto_Power_Normal_Init(void)
{
    halBoard_PwrSeqSet();
    halCard_Init();
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Goto_Power_Normal_Process
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_Goto_Power_Normal_Process(void)
{
    // Reset System Cards Status
    m_sSystemConfig.wCardPresent = 0;
    m_sSystemConfig.wCardPowerOn = 0;

    // 1. Check Card Detect, if Card present, apply power on event.
    m_sSystemConfig.wCardPresent = halCard_Card_Detect();

    switch(m_sSystemConfig.wCardPresent)
    {
        case ALL_SLOT_MASK: // Card and OPS present
            LOG_MSG(eDB_MSK_LIST_SYSCTRL, "All Slot Present\r\n");
            break;

        case OPS_MASK: // OPS present
            LOG_MSG(eDB_MSK_LIST_SYSCTRL, "OPS Present\r\n");
            break;

        case CARD_MASK: // Card Present
            LOG_MSG(eDB_MSK_LIST_SYSCTRL, "Card Present\r\n");
            break;

        default:
            LOG_MSG(eDB_MSK_LIST_SYSCTRL, "No Card\r\n");
            break;
    }

    appSysProc_State_Set(eSYS_STATE_POWER_NORMAL);
    //appSysProc_Card_Power_Set(0xFF);
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Power_Normal_Init
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_Power_Normal_Init(void)
{
    sEVENT_DATA sEventData;

    sEventData.uEVET_DATA.wData = eSWITCHER_CTRL_POWER_ON;
    utilEvent_Send(eEVENT_LIST_HDMI_SWITCHER, 2, &sEventData.uEVET_DATA);
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Power_Normal_Process
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_Power_Normal_Process(void)
{
    if(CARD_MASK == (m_sSystemConfig.wCardPowerOn & CARD_MASK))
    {
        halCard_Slot_Ready_Check();
    }

    //Monitor the OPS Power status
    if(OPS_MASK == (m_sSystemConfig.wCardPowerOn & OPS_MASK))
    {
        if(TRUE == halCard_OPS_Power_On_Get()) {
            m_sSystemConfig.wCardPowerStatus = SETBIT(m_sSystemConfig.wCardPowerStatus, OPS_MASK);
        } else {
            m_sSystemConfig.wCardPowerStatus = CLRBIT(m_sSystemConfig.wCardPowerStatus, OPS_MASK);
        }
    }

    // Monitor OPS PON Pin
    halCard_OPS_Power_Check();
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Goto_Standby_Init
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_Goto_Standby_Init(void)
{
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Goto_Standby_Process
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_Goto_Standby_Process(void)
{
}

// ==============================================================================
// FUNCTION NAME: appSysProc_System_Error_Init
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_System_Error_Init(void)
{
}

// ==============================================================================
// FUNCTION NAME: appSysProc_System_Error_Process
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
static INLINE void appSysProc_System_Error_Process(void)
{
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Init
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
void appSysProc_Init(void)
{
    m_sSystemConfig.eCurrentState = eSYS_STATE_NUMBERS;
    m_sSystemConfig.eLastState = eSYS_STATE_NUMBERS;

    m_sSystemConfig.wCardPresent = FALSE;
    m_sSystemConfig.wCardPowerOn = FALSE;

    appSysProc_State_Set(eSYS_STATE_STANDBY);
}

// ==============================================================================
// FUNCTION NAME: appSysProc_State_Set
// DESCRIPTION:
//
//
// Params:
// eSYS_STATE eState:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
void appSysProc_State_Set(eSYS_STATE eState)
{
    if(eState != m_sSystemConfig.eCurrentState)
    {
        switch(eState)
        {
            case eSYS_STATE_STANDBY:
                appSysProc_Standby_Init();
                break;

            case eSYS_STATE_GOTO_POWER_NORMAL:
                appSysProc_Goto_Power_Normal_Init();
                break;

            case eSYS_STATE_POWER_NORMAL:
                appSysProc_Power_Normal_Init();
                break;

            case eSYS_STATE_GOTO_STANDBY:
                appSysProc_Goto_Standby_Init();
                break;

            case eSYS_STATE_SYSTEM_ERROR:
                appSysProc_System_Error_Init();
                break;

            default:
                //ASSERT_ALWAYS();
                break;
        } // End of Switch

        m_sSystemConfig.eLastState = m_sSystemConfig.eCurrentState;
        m_sSystemConfig.eCurrentState = eState;
    }
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Process
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
void appSysProc_Process(void)
{
    switch(m_sSystemConfig.eCurrentState)
    {
        case eSYS_STATE_STANDBY:
            appSysProc_Standby_Process();
            break;

        case eSYS_STATE_GOTO_POWER_NORMAL:
            appSysProc_Goto_Power_Normal_Process();
            break;

        case eSYS_STATE_POWER_NORMAL:
            appSysProc_Power_Normal_Process();
            break;

        case eSYS_STATE_GOTO_STANDBY:
            appSysProc_Goto_Standby_Process();
            break;

        case eSYS_STATE_SYSTEM_ERROR:
            appSysProc_System_Error_Process();
            break;

        default:
            //ASSERT_ALWAYS();
            break;
    } // End of Switch
}

// ==============================================================================
// FUNCTION NAME: appSysProc_State_Get
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2018/10/16, Leo Create
// --------------------
// ==============================================================================
eSYS_STATE appSysProc_State_Get(void)
{
    return m_sSystemConfig.eCurrentState;
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Card_Present_Get
// DESCRIPTION:
//
//
// Params:
// WORD:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/21, Leo Create
// --------------------
// ==============================================================================
WORD appSysProc_Card_Present_Get(void)
{
    return m_sSystemConfig.wCardPresent;
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Card_Power_Get
// DESCRIPTION:
//
//
// Params:
// WORD:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/21, Leo Create
// --------------------
// ==============================================================================
WORD appSysProc_Card_Power_Get(void)
{
    return m_sSystemConfig.wCardPowerOn;
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Card_Power_Status_Get
// DESCRIPTION:
//
//
// Params:
// WORD:
//
// Returns:
//
//
// modification history
// --------------------
// 2020/03/31, Leo Create
// --------------------
// ==============================================================================
WORD appSysProc_Card_Power_Status_Get(void)
{
    return m_sSystemConfig.wCardPowerStatus;
}

// ==============================================================================
// FUNCTION NAME: appSysProc_Card_Power_Set
// DESCRIPTION:
//
//
// Params:
// WORD wEnable:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/21, Leo Create
// --------------------
// ==============================================================================
void appSysProc_Card_Power_Set(WORD wEnable)
{
    wEnable = wEnable & ALL_SLOT_MASK;

    if(TRUE == GETBIT(wEnable, OPS_MASK))
    {
        halBoard_OPS_Power_Rail_Enable(TRUE);
        m_sSystemConfig.wCardPowerOn |= OPS_MASK;
    }
    else
    {
        halBoard_OPS_Power_Rail_Enable(FALSE);
        m_sSystemConfig.wCardPowerOn &= ~OPS_MASK;
    }

    if(TRUE == GETBIT(wEnable, CARD_MASK))
    {
        halBoard_Slot_Power_Rail_Enable(TRUE);
        halCard_Slot_Check_Set();
        m_sSystemConfig.wCardPowerOn |= CARD_MASK;
    }
    else
    {
        halBoard_Slot_Power_Rail_Enable(FALSE);
        m_sSystemConfig.wCardPowerOn &= ~CARD_MASK;
    }
}

// ==============================================================================
// FUNCTION NAME: appMain_Task
// DESCRIPTION:
//
//
// PARAMETERS:
// void *pParameters:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/03/22, Leo Create
// --------------------
// ==============================================================================
void appMain_Task(void *pParameters)
{
    halBoard_Init();
    utilLog_Init();

    //appVideoProcessor_Init();
    //utilCounter_Init();
    utilEventHandler_Init();
    utilHost_Init();
    //utilSenderHandler_Init();
    //utilRS232_CLI_Init();

    appInputProc_Init();
    appSysProc_Init();

    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "###############################################\r\n");
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, " M4 System Clock %d-MHz\r\n", SystemCoreClock / 1000000L);
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, " DB Ver: %02d.%02d\r\n", VER_MAJOR, VER_MINOR);
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, " FreeRTOS Kernel V10.2.1\r\n");
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "###############################################\r\n");

    while(1)
    {
        appInputProc_Process();
        appSysProc_Process();
        //appVideoProcessor_Process();
        //utilRS232_CLI_Process();

        utilFunc_Test_Loop();
    }

    // __BKPT(0x03);
}

