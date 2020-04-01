// ==============================================================================
// FILE NAME: BOARD_MCPWM.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 26/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "Board.h"
#include "utilLogAPI.h"
#include "Board_MCPWM.h"

void (*fpMcpwmCallback[eBOARD_MCPWM_NUMBERS])(void);

// ==============================================================================
// FUNCTION NAME: MCPWM_IRQHandler
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/05/13, Leo Create
// --------------------
// ==============================================================================
void MCPWM_IRQHandler(void)
{
    /* Check whether if capture event interrupt is set */
    if(Chip_MCPWM_GetLimitIntFlag(LPC_MCPWM, eBOARD_MCPWM_CHANNEL0))
    {
        // A70_Jonas_0001 Start
        // 修正 Lens shift 操作過程出線機率性當機問題。
        // 經分析後，再現象出現時，MCPWM Timer 自己 disable，且 TC 沒有自動 RESET，
        // 導致 PWM 中斷持續被出發，佔住系統資源。
        if(Chip_MCPWM_GetRunEnable(LPC_MCPWM, eBOARD_MCPWM_CHANNEL0) == 0)  // If MCPWM CH0 disabled...
        {
            Chip_MCPWM_SetCounter(LPC_MCPWM, eBOARD_MCPWM_CHANNEL0, 0);     // Reset TC

            Chip_MCPWM_RunEnable(LPC_MCPWM, eBOARD_MCPWM_CHANNEL0);         // Forced re-start MCPWM
        }
        // A70_Jonas_0001 End

        ASSERT(NULL != fpMcpwmCallback[eBOARD_MCPWM_CHANNEL0]);

        if(NULL != fpMcpwmCallback[eBOARD_MCPWM_CHANNEL0])
        {
            fpMcpwmCallback[eBOARD_MCPWM_CHANNEL0]();
        }

        Chip_MCPWM_ClearLimitIntFlag(LPC_MCPWM, eBOARD_MCPWM_CHANNEL0);
    }

    if(Chip_MCPWM_GetLimitIntFlag(LPC_MCPWM, eBOARD_MCPWM_CHANNEL1))
    {
        // A70_Jonas_0001 Start
        // 修正 Lens shift 操作過程出線機率性當機問題。
        // 經分析後，再現象出現時，MCPWM Timer 自己 disable，且 TC 沒有自動 RESET，
        // 導致 PWM 中斷持續被出發，佔住系統資源。
        if(Chip_MCPWM_GetRunEnable(LPC_MCPWM, eBOARD_MCPWM_CHANNEL1) == 0)  // If MCPWM CH0 disabled...
        {
            Chip_MCPWM_SetCounter(LPC_MCPWM, eBOARD_MCPWM_CHANNEL1, 0);     // Reset TC

            Chip_MCPWM_RunEnable(LPC_MCPWM, eBOARD_MCPWM_CHANNEL1);         // Forced re-start MCPWM
        }
        // A70_Jonas_0001 End

        ASSERT(NULL != fpMcpwmCallback[eBOARD_MCPWM_CHANNEL1]);

        if(NULL != fpMcpwmCallback[eBOARD_MCPWM_CHANNEL1])
        {
            fpMcpwmCallback[eBOARD_MCPWM_CHANNEL1]();
        }

        Chip_MCPWM_ClearLimitIntFlag(LPC_MCPWM, eBOARD_MCPWM_CHANNEL1);
    }

    if(Chip_MCPWM_GetLimitIntFlag(LPC_MCPWM, eBOARD_MCPWM_CHANNEL2))
    {
        // A70_Jonas_0001 Start
        // 修正 Lens shift 操作過程出線機率性當機問題。
        // 經分析後，再現象出現時，MCPWM Timer 自己 disable，且 TC 沒有自動 RESET，
        // 導致 PWM 中斷持續被出發，佔住系統資源。
        if(Chip_MCPWM_GetRunEnable(LPC_MCPWM, eBOARD_MCPWM_CHANNEL2) == 0)  // If MCPWM CH0 disabled...
        {
            Chip_MCPWM_SetCounter(LPC_MCPWM, eBOARD_MCPWM_CHANNEL2, 0);     // Reset TC

            Chip_MCPWM_RunEnable(LPC_MCPWM, eBOARD_MCPWM_CHANNEL2);         // Forced re-start MCPWM
        }
        // A70_Jonas_0001 End

        ASSERT(NULL != fpMcpwmCallback[eBOARD_MCPWM_CHANNEL2]);

        if(NULL != fpMcpwmCallback[eBOARD_MCPWM_CHANNEL2])
        {
            fpMcpwmCallback[eBOARD_MCPWM_CHANNEL2]();
        }

        Chip_MCPWM_ClearLimitIntFlag(LPC_MCPWM, eBOARD_MCPWM_CHANNEL2);
    }

    NVIC_ClearPendingIRQ(MCPWM_IRQn);
}

#if 0
typedef struct
{

    BYTE cIntEnable;
} sBOARD_MCPWM_CFG;

#define MOTOR_SPEED (1000)
void Board_MCPWM_Init(eBOARD_MCPWM eChannel)
{
    DWORD dwClk = 0;

    dwClk = Chip_Clock_GetRate(CLK_APB1_MOTOCON);

    switch((BYTE)eChannel)
    {
        case eBOARD_MCPWM_CHANNEL0:
            Chip_MCPWM_CaptureDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_PolarityPassiveHiEnable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DeadtimeDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DisableUpdatingDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_SetLimit(LPC_MCPWM, eChannel, dwClk / MOTOR_SPEED);
            Chip_MCPWM_SetMatch(LPC_MCPWM, eChannel, (dwClk / MOTOR_SPEED) / 2);
            Chip_MCPWM_LimitIntSet(LPC_MCPWM, eChannel);
            break;

        case eBOARD_MCPWM_CHANNEL1:
            Chip_MCPWM_CaptureDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_PolarityPassiveHiEnable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DeadtimeDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DisableUpdatingDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_SetLimit(LPC_MCPWM, eChannel, dwClk / MOTOR_SPEED);
            Chip_MCPWM_SetMatch(LPC_MCPWM, eChannel, (dwClk / MOTOR_SPEED) / 2);
            Chip_MCPWM_LimitIntSet(LPC_MCPWM, eChannel);
            break;

        case eBOARD_MCPWM_CHANNEL2:
            Chip_MCPWM_CaptureDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_PolarityPassiveHiDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DeadtimeDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DisableUpdatingDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_SetLimit(LPC_MCPWM, eChannel, dwClk / MOTOR_SPEED);
            Chip_MCPWM_SetMatch(LPC_MCPWM, eChannel, (dwClk / MOTOR_SPEED) / 2);
            Chip_MCPWM_LimitIntSet(LPC_MCPWM, eChannel);
            break;
    }

    /* Setting MCPWM interrupt */
    NVIC_SetPriority(MCPWM_IRQn, eINT_PRIORITY_LEVEL_6);

    NVIC_ClearPendingIRQ(MCPWM_IRQn);
    NVIC_EnableIRQ(MCPWM_IRQn);

    Chip_MCPWM_Init(LPC_MCPWM);
}
#else
// ==============================================================================
// FUNCTION NAME: BOARD_MCPWM_INIT
// DESCRIPTION:
//
//
// Params:
// eBOARD_MCPWM eChannel:
//
// Returns:
//
//
// modification history
// --------------------
// 30/12/2013, Leo Create
// --------------------
// ==============================================================================
//A70LH_JS_0031 remove to CommonDef.h
#define SHUTTER_SPEED (930) //A70_Larry_0014
#define LENS_SPEED    (1000) //default speed //A70LH_Larry_0180

void Board_MCPWM_Init(eBOARD_MCPWM eChannel)
{
    DWORD dwClk = 0;

    dwClk = Chip_Clock_GetRate(CLK_APB1_MOTOCON);

    switch((BYTE)eChannel)
    {
        case eBOARD_MCPWM_CHANNEL0:
            Chip_MCPWM_CaptureDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_PolarityPassiveHiEnable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DeadtimeDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DisableUpdatingDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_SetCounter(LPC_MCPWM, eChannel, 0);  // A70_Jonas_0001, 修正 Lens shift 操作過程出線機率性當機問題。
            Chip_MCPWM_SetLimit(LPC_MCPWM, eChannel, dwClk / LENS_SPEED);
            Chip_MCPWM_SetMatch(LPC_MCPWM, eChannel, (dwClk / LENS_SPEED) / 2);
            Chip_MCPWM_LimitIntSet(LPC_MCPWM, eChannel);
            break;

        case eBOARD_MCPWM_CHANNEL1:
            Chip_MCPWM_CaptureDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_PolarityPassiveHiEnable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DeadtimeDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DisableUpdatingDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_SetCounter(LPC_MCPWM, eChannel, 0);  // A70_Jonas_0001, 修正 Lens shift 操作過程出線機率性當機問題。
            Chip_MCPWM_SetLimit(LPC_MCPWM, eChannel, dwClk / SHUTTER_SPEED); //A70_Larry_0014
            Chip_MCPWM_SetMatch(LPC_MCPWM, eChannel, (dwClk / SHUTTER_SPEED) / 2); //A70_Larry_0014
            Chip_MCPWM_LimitIntSet(LPC_MCPWM, eChannel);
            break;

        case eBOARD_MCPWM_CHANNEL2:
            Chip_MCPWM_CaptureDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_PolarityPassiveHiDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DeadtimeDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_DisableUpdatingDisable(LPC_MCPWM, eChannel);
            Chip_MCPWM_SetCounter(LPC_MCPWM, eChannel, 0);  // A70_Jonas_0001, 修正 Lens shift 操作過程出線機率性當機問題。
            Chip_MCPWM_SetLimit(LPC_MCPWM, eChannel, dwClk / LENS_SPEED);
            Chip_MCPWM_SetMatch(LPC_MCPWM, eChannel, (dwClk / LENS_SPEED) / 2);
            Chip_MCPWM_LimitIntSet(LPC_MCPWM, eChannel);
            break;
    }

    /* Setting MCPWM interrupt */
    NVIC_SetPriority(MCPWM_IRQn, eINT_PRIORITY_LEVEL_6);

    NVIC_ClearPendingIRQ(MCPWM_IRQn);
    NVIC_EnableIRQ(MCPWM_IRQn);

    Chip_MCPWM_Init(LPC_MCPWM);
}
#endif /* 1 */

// ==============================================================================
// FUNCTION NAME: BOARD_MCPWM_DEINIT
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
// 30/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_MCPWM_DeInit(void)
{
    /* Disable Interrupt for MCPWM channel */
    NVIC_DisableIRQ(MCPWM_IRQn);
    Chip_MCPWM_DeInit(LPC_MCPWM);
}

// ==============================================================================
// FUNCTION NAME: BOARD_MCPWM_START
// DESCRIPTION:
//
//
// Params:
// eBOARD_MCPWM eChannel:
//
// Returns:
//
//
// modification history
// --------------------
// 30/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_MCPWM_Start(eBOARD_MCPWM eChannel)
{
    Chip_MCPWM_RunEnable(LPC_MCPWM, eChannel);
}

// ==============================================================================
// FUNCTION NAME: BOARD_MCPWM_STOP
// DESCRIPTION:
//
//
// Params:
// eBOARD_MCPWM eChannel:
//
// Returns:
//
//
// modification history
// --------------------
// 30/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_MCPWM_Stop(eBOARD_MCPWM eChannel)
{
    Chip_MCPWM_RunDisable(LPC_MCPWM, eChannel);
    Chip_MCPWM_SetCounter(LPC_MCPWM, eChannel, 0);  // A70_Jonas_0001, Reset TC
}

// ==============================================================================
// FUNCTION NAME: Board_MCPWM_Freq_Set
// DESCRIPTION:
//
//
// Params:
// eBOARD_MCPWM eChannel:
// DOWORD dwFreq:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/07, Leo Create
// --------------------
// ==============================================================================
void Board_MCPWM_Freq_Set(eBOARD_MCPWM eChannel, DWORD dwFreq)
{
    DWORD dwClk = 0;
    dwClk = Chip_Clock_GetRate(CLK_APB1_MOTOCON);

    Chip_MCPWM_SetLimit(LPC_MCPWM, eChannel, dwClk / dwFreq);
    Chip_MCPWM_SetMatch(LPC_MCPWM, eChannel, (dwClk / dwFreq) / 2);
}

// ==============================================================================
// FUNCTION NAME: BOARD_MCPWM_REGISTER_CALLBACK
// DESCRIPTION:
//
//
// Params:
// eBOARD_MCPWM eChannel:
// void (*fpCallback)(void):
//
// Returns:
//
//
// modification history
// --------------------
// 30/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_Mcpwm_Register_Callback(eBOARD_MCPWM eChannel, void (*fpCallback)(void))
{
    fpMcpwmCallback[eChannel] = fpCallback;
}


