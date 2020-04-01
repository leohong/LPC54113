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
#include "dvCard.h"
#include "halCard.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"

#define CARD_DEBOUNCE_CNT   (5)
#define CARD_COUNT          (2)

const char acInputName[] = "None";

typedef struct
{
    eDETECT_STATE   cState;
    sSLOT_INFO      sSlotInfo;
} sHAL_CARD_INFO;

sHAL_CARD_INFO m_sHalCardInfo;

// ==============================================================================
// FUNCTION NAME: halCard_Init
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
// 2020/02/05, Leo Create
// --------------------
// ==============================================================================
void halCard_Init(void)
{
    m_sHalCardInfo.sSlotInfo.cInputs = 0;
    memcpy(&m_sHalCardInfo.sSlotInfo.acInputName, acInputName, sizeof(acInputName));

    dvCard_Initial();
}

// ==============================================================================
// FUNCTION NAME: halCard_Card_Detect
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
// 2020/02/05, Leo Create
// --------------------
// ==============================================================================
BYTE halCard_Card_Detect(void)
{
    eBOARD_PIN_TABLE ecDetectPin[CARD_COUNT] = {pOPS_DET_N, pCARD_DET_N};
    BYTE acCardPresent[CARD_COUNT] = {0, 0};
    BYTE cCardCount = 0;
    BYTE cDebunce = 0;
    BYTE cResult = 0;

    for(cDebunce = 0; cDebunce < CARD_DEBOUNCE_CNT; cDebunce++)
    {
        for(cCardCount = 0; cCardCount < CARD_COUNT; cCardCount++)
        {
            if(FALSE == Board_Gpio_Get(ecDetectPin[cCardCount]))
            {
                acCardPresent[cCardCount]++;
            }
            else
            {
                acCardPresent[cCardCount] = 0;
            }
        }

        utilDelayMs(2);
    }

    for(cCardCount = 0; cCardCount < CARD_COUNT; cCardCount++)
    {
        if(0 != acCardPresent[cCardCount])
        {
            cResult = cResult | (0x01 << cCardCount);
        }
    }

    return cResult;
}

// ==============================================================================
// FUNCTION NAME: halCard_OPS_Power_Set
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
// 2020/03/31, Leo Create
// --------------------
// ==============================================================================
void halCard_OPS_Power_Set(void)
{
    Board_Gpio_Set(pOPS_PSON, FALSE);
    utilCounterSet(eCOUNTER_TYPE_OPS_PON_TIMER, 1000);
}

// ==============================================================================
// FUNCTION NAME: halCard_OPS_Power_Check
// DESCRIPTION:
//
//
// PARAMETERS:
// BYTE eEable:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2020/03/31, Leo Create
// --------------------
// ==============================================================================
void halCard_OPS_Power_Check(void)
{
    if(0 == utilCounterGet(eCOUNTER_TYPE_OPS_PON_TIMER)) {
        Board_Gpio_Set(pOPS_PSON, TRUE);
    }
}

// ==============================================================================
// FUNCTION NAME: halCard_OPS_Power_On_Get
// DESCRIPTION:
//
// Detect OPS PSOK, this function will block the task 25ms
//
// PARAMETERS:
//
// RETURNS:
// BYTE:
//
// modification history
// --------------------
// 2020/02/05, Leo Create
// --------------------
// ==============================================================================
BYTE halCard_OPS_Power_On_Get(void)
{
    static BYTE cPwrOn = 0;

    // LOW: Power on, HIGH: Standby
    if(HIGH == Board_Gpio_Get(pOPS_PSOK)) {
        if( cPwrOn > 0 ) {
            cPwrOn--;
        }
    } else {
        if( 0x0F > cPwrOn ) {
            cPwrOn++;
        }
    }
    
    return (!!cPwrOn);
}

// ==============================================================================
// FUNCTION NAME: halCard_Card_Ready
// DESCRIPTION:
//
// Detect OPS PSOK, this function will block the task 25ms
//
// PARAMETERS:
//
// RETURNS:
// BYTE:
//
// modification history
// --------------------
// 2020/02/05, Leo Create
// --------------------
// ==============================================================================
BYTE halCard_Card_Ready(void)
{
    BYTE cReady = FALSE;

    if(rcSUCCESS == dvCard_System_Ready_Get(&cReady)) {
        dvCard_Slot_Info_Get(&m_sHalCardInfo.sSlotInfo);
        cReady = TRUE;
    }

    return cReady;
}

// ==============================================================================
// FUNCTION NAME: halHdmiSwitch_Slot_Info_Get
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
// 2019/12/10, Leo Create
// --------------------
// ==============================================================================
sSLOT_INFO halCard_Slot_Info_Get(void)
{
    return m_sHalCardInfo.sSlotInfo;
}

// ==============================================================================
// FUNCTION NAME: halCard_Slot_Check_Set
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
// 2019/12/10, Leo Create
// --------------------
// ==============================================================================
void halCard_Slot_Check_Set(void)
{
    m_sHalCardInfo.cState = eDETECT_STATE_CARD_INFO;
}

// ==============================================================================
// FUNCTION NAME: halCard_Slot_Ready_Get
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
// 2019/12/10, Leo Create
// --------------------
// ==============================================================================
BYTE halCard_Slot_Ready_Get(void)
{
    BYTE cReady = FALSE;

    if(eDETECT_STATE_CARD_DONE == m_sHalCardInfo.cState)
    {
        cReady = TRUE;
    }

    return cReady;
}

// ==============================================================================
// FUNCTION NAME: halCard_Slot_Ready_Check
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
// 2019/12/10, Leo Create
// --------------------
// ==============================================================================
void halCard_Slot_Ready_Check(void)
{
    switch(m_sHalCardInfo.cState)
    {
        case eDETECT_STATE_CARD_IDLE:
            break;

        case eDETECT_STATE_CARD_INFO:
            if(0 == utilCounterGet(eCOUNTER_TYPE_SLOT_CHECK))
            {
                utilCounterSet(eCOUNTER_TYPE_SLOT_CHECK, 1000);

                if(TRUE == halCard_Card_Ready())
                {
                    m_sHalCardInfo.cState = eDETECT_STATE_CARD_DONE;
                }
            }
            break;

        case eDETECT_STATE_CARD_DONE:
            break;

        default:
            break;
    }
}

// ==============================================================================
// FUNCTION NAME: halCard_Slot_Input_Numbers_Get
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
// 2019/12/10, Leo Create
// --------------------
// ==============================================================================
BYTE halCard_Slot_Input_Numbers_Get(void)
{
    return m_sHalCardInfo.sSlotInfo.cInputs;
}
