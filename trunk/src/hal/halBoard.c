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
