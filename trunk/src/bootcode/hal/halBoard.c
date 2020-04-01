// ===============================================================================
// FILE NAME: halBoard.c
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2018/10/28, Leo Create
// --------------------
// ===============================================================================


#include "Board.h"
#include "Board_GPIO.h"
#include "Board_Uart.h"
#include "halBoardCtrlAPI.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"

// ==============================================================================
// FUNCTION NAME: halBoard_Init
// DESCRIPTION:
//
//
// PARAMETERS:
// void:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2018/10/28, Leo Create
// --------------------
// ==============================================================================
void halBoard_Init(void)
{
    Board_Init();

    //halBoard_Power_Enable(FALSE);
    //halBoard_Chips_Power_Enable(FALSE);
    //halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_ON, 0);
    //halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_OFF, 0);
}

// ==============================================================================
// FUNCTION NAME: halBoard_PwrSeqSet
// DESCRIPTION:
//
//
// PARAMETERS:
// BOOL bEnable:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/10/24, Leo Create
// --------------------
// ==============================================================================
void halBoard_PwrSeqSet(void)
{
    WORD wPinCount = 0;

    // 設定的Gpio Pin不能超過pBOARD_PIN_NUMBERS
    while(m_asPwrOnPwrSeq[wPinCount].wPinName < pBOARD_PIN_NUMBERS)
    {
        // 設定GPIO Pin狀態
        Board_Gpio_Set((eBOARD_PIN_TABLE)m_asPwrOnPwrSeq[wPinCount].wPinName, m_asPwrOnPwrSeq[wPinCount].cStatus);

        // 設定GPIO Pin時間
        utilDelayMs(m_asPwrOnPwrSeq[wPinCount].wDelayTime);

        // Next Pin
        wPinCount++;
    }
}

#if 0
// ==============================================================================
// FUNCTION NAME: halBoard_Power_Enable
// DESCRIPTION:
//
//
// PARAMETERS:
// BOOL bEnable:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2018/10/28, Leo Create
// --------------------
// ==============================================================================
void halBoard_Power_Enable(BYTE cEnable)
{
    if(TRUE == cEnable)
    {
        Board_Gpio_Set(eCONNECTOR_BOARD_PS_ON, TRUE);
    }
    else if(FALSE == cEnable)
    {
        Board_Gpio_Set(eCONNECTOR_BOARD_PS_ON, FALSE);
    }
}

// ==============================================================================
// FUNCTION NAME: halBoard_Red_LED_Toggle
// DESCRIPTION:
//
//
// PARAMETERS:
// void:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2018/11/16, Leo Create
// --------------------
// ==============================================================================
static void halBoard_Red_LED_Toggle(void)
{
    Board_Gpio_Toggle_Set(m_aeLED_PIN[eLED_LIST_RED]);
}

// ==============================================================================
// FUNCTION NAME: halBoard_Green_LED_Toggle
// DESCRIPTION:
//
//
// PARAMETERS:
// void:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2018/11/16, Leo Create
// --------------------
// ==============================================================================
static void halBoard_Green_LED_Toggle(void)
{
    Board_Gpio_Toggle_Set(m_aeLED_PIN[eLED_LIST_GREEN_1]);
}

// ==============================================================================
// FUNCTION NAME: halBoard_LED_Ctrl
// DESCRIPTION:
//
//
// PARAMETERS:
// eLED_LIST eLED:
// eLED_CTRL eState:
// DWORD dwOnTime:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2018/11/16, Leo Create
// --------------------
// ==============================================================================
void halBoard_LED_Ctrl(eLED_LIST eLED, eLED_CTRL eState, DWORD dwOnTime)
{
    switch(eState)
    {
        case eLED_CTRL_OFF:
            utilCounter_UnReg_TimerEvent(m_aeLED_Event[eLED]);
            Board_Gpio_Set(m_aeLED_PIN[eLED], FALSE);
            break;

        case eLED_CTRL_ON:
            utilCounter_UnReg_TimerEvent(m_aeLED_Event[eLED]);
            Board_Gpio_Set(m_aeLED_PIN[eLED], TRUE);
            break;

        case eLED_CTRL_BLINK:
            switch(eLED)
            {
                case eLED_LIST_RED:
                    utilCounter_Reg_TimerEvent(m_aeLED_Event[eLED], dwOnTime, TRUE, 0, halBoard_Red_LED_Toggle, NULL);
                    break;

                case eLED_LIST_GREEN_1:
                    utilCounter_Reg_TimerEvent(m_aeLED_Event[eLED], dwOnTime, TRUE, 0, halBoard_Green_LED_Toggle, NULL);
                    break;

                default:
                    break;
            }

            break;

        default:
            break;
    }
}

// ==============================================================================
// FUNCTION NAME: halBoard_LED_State_Set
// DESCRIPTION:
//
//
// PARAMETERS:
// eLED_STATE eState:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2018/11/16, Leo Create
// --------------------
// ==============================================================================
void halBoard_LED_State_Set(eLED_STATE eState)
{
    switch(eState)
    {
        case eLED_STATE_STANDBY:
            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_BLINK, 10);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_OFF, 0);
            break;

        case eLED_STATE_WARMUP:
            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_OFF, 0);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_BLINK, 500);
            break;

        case eLED_STATE_UPDATE:
        case eLED_STATE_RESTORE:
            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_OFF, 0);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_ON, 0);

            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_BLINK, 100);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_BLINK, 100);
            break;

        case eLED_STATE_REMOVE_USB:
            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_OFF, 0);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_OFF, 0);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_BLINK, 50);
            break;

        case eLED_STATE_PROGRAMMING:
            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_ON, 0);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_ON, 0);
            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_BLINK, 50);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_BLINK, 50);
            break;

        case eLED_STATE_POWER_NORMAL:
            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_OFF, 0);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_ON, 0);
            break;

        case eLED_STATE_GOTO_STANDBY:
            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_BLINK, 500);
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_OFF, 0);
            break;

        case eLED_STATE_ERROR:
            halBoard_LED_Ctrl(eLED_LIST_GREEN_1, eLED_CTRL_OFF, 0);
            halBoard_LED_Ctrl(eLED_LIST_RED, eLED_CTRL_BLINK, 50);
            break;

        default:
            break;
    }
}

// ==============================================================================
// FUNCTION NAME: halBoard_Chips_Power_Enable
// DESCRIPTION:
//
//
// PARAMETERS:
// BYTE cEnable:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2018/12/11, Leo Create
// --------------------
// ==============================================================================
void halBoard_Chips_Power_Enable(BYTE cEnable)
{
    if(TRUE == cEnable)
    {
        Board_Gpio_Set(eCONNECTOR_BOARD_ITE66311_RSTN, TRUE);
        Board_Gpio_Set(eCONNECTOR_BOARD_USB_EN, TRUE);
    }
    else
    {
        Board_Gpio_Set(eCONNECTOR_BOARD_ITE66311_RSTN, FALSE);
        Board_Gpio_Set(eCONNECTOR_BOARD_USB_EN, FALSE);
    }
}

// ==============================================================================
// FUNCTION NAME: halBoard_Uart_ReDirect_Toggle
// DESCRIPTION:
//
//
// PARAMETERS:
// void:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/01/07, Leo Create
// --------------------
// ==============================================================================
void halBoard_Uart_ReDirect_Toggle(void)
{
    static BOOL bUartReDir = FALSE;

    if(FALSE == bUartReDir)
    {
        bUartReDir = TRUE;
    }
    else
    {
        bUartReDir = FALSE;
    }

    Board_Uart_Port_ReDir(bUartReDir);
    LOG_MSG(eDB_MSK_LIST_INPUT, "Uart Redir %d\r\n", bUartReDir);
}

#endif // 0

// ==============================================================================
// FUNCTION NAME: halBoard_OPS_Power_Rail_Enable
// DESCRIPTION:
//
//
// PARAMETERS:
// BYTE cEnable:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/11/21, Leo Create
// --------------------
// ==============================================================================
void halBoard_OPS_Power_Rail_Enable(BYTE cEnable)
{
    if(TRUE == cEnable)
    {
        Board_Gpio_Set(pOPS_PON, HIGH);
    }
    else
    {
        Board_Gpio_Set(pOPS_PON, LOW);
    }
}

// ==============================================================================
// FUNCTION NAME: halBoard_Slot_Power_Rail_Enable
// DESCRIPTION:
//
//
// PARAMETERS:
// BYTE cEnable:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/11/21, Leo Create
// --------------------
// ==============================================================================
void halBoard_Slot_Power_Rail_Enable(BYTE cEnable)
{
    if(TRUE == cEnable)
    {
        Board_Gpio_Set(pSLOT_PON, HIGH);
    }
    else
    {
        Board_Gpio_Set(pSLOT_PON, LOW);
    }
}
