// ==============================================================================
// FILE NAME: BOARD_TIMER.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 21/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "Board.h"
#include "Board_Timer.h"
#include "utilLogAPI.h"

// ==============================================================================
// Define Section
// ==============================================================================
// For light sensor
#define TIMER0_CAPTURE_CHANNEL  3

// For IR
#define TIMER1_CAPTURE_CHANNEL  1
#define TIMER1_IR_SAMPLE_RATE   10      //0.01ms

// ==============================================================================
// Local variable Section
// ==============================================================================
static LPC_TIMER_T *m_apsBoard_Timer_Reg[eBOARD_TIMER_NUMBERS] = {LPC_TIMER0, LPC_TIMER1, LPC_TIMER2, LPC_TIMER3};
static IRQn_Type m_apsBoard_Timer_IRQ[eBOARD_TIMER_NUMBERS] = { TIMER0_IRQn, TIMER1_IRQn, TIMER2_IRQn, TIMER3_IRQn};

static fpTIMERCALLBACK m_fpTimerCallback[eBOARD_TIMER_NUMBERS];

static fpTIMER3CHANNELCALLBACK m_fpTimer3ChannleCallback[eBOARD_TIMER_CHANNLE_NUMBERS]; //For light sensor //A35LS_Larry_0003

// ==============================================================================
// Initial Tables
// ==============================================================================
sBOARD_TIMER_CFG *m_psTimerCfg[eBOARD_TIMER_NUMBERS];

// ==============================================================================
// FUNCTION NAME: TIMER_IRQHANDLER
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER eTimer:
//
// Returns:
//
//
// modification history
// --------------------
// 21/11/2013, Leo Create
// --------------------
// ==============================================================================
void TIMER_IRQHandler(eBOARD_TIMER eTimer)
{
    switch(eTimer)
    {
        case eBOARD_TIMER_TIMER0:
            break;

        case eBOARD_TIMER_TIMER1:
            if(Chip_TIMER_CapturePending(m_apsBoard_Timer_Reg[eTimer], m_psTimerCfg[eTimer]->uModeCfg.sCaptureCfg.eCaptureChannel))
            {
                DWORD dwCaptureData = 0;
                dwCaptureData = Chip_TIMER_ReadCapture(m_apsBoard_Timer_Reg[eTimer], m_psTimerCfg[eTimer]->uModeCfg.sCaptureCfg.eCaptureChannel);

                if(NULL != m_fpTimerCallback[eTimer])
                {
                    m_fpTimerCallback[eTimer](&dwCaptureData);
                }
            }

            Chip_TIMER_ClearCapture(m_apsBoard_Timer_Reg[eTimer], m_psTimerCfg[eTimer]->uModeCfg.sCaptureCfg.eCaptureChannel);
            break;

        case eBOARD_TIMER_TIMER2:
            if(NULL != m_fpTimerCallback[eTimer])
            {
                m_fpTimerCallback[eTimer](NULL);
            }

            Chip_TIMER_ClearMatch(m_apsBoard_Timer_Reg[eTimer], m_psTimerCfg[eTimer]->uModeCfg.sTimerCfg.eTimerChannel);
            break;

        case eBOARD_TIMER_TIMER3:
            // A70LH_Jonas_0001 Start, Added for getting light sensor
            // For match ISR
            if(Chip_TIMER_MatchPending(m_apsBoard_Timer_Reg[eTimer], eBOARD_TIMER_CHANNLE_0))
            {
                Chip_TIMER_ClearMatch(m_apsBoard_Timer_Reg[eTimer], eBOARD_TIMER_CHANNLE_0);

                if(NULL != m_fpTimer3ChannleCallback[eBOARD_TIMER_CHANNLE_0])
                {
                    m_fpTimer3ChannleCallback[eBOARD_TIMER_CHANNLE_0]();
                }
            }
            if(Chip_TIMER_MatchPending(m_apsBoard_Timer_Reg[eTimer], eBOARD_TIMER_CHANNLE_1))
            {
                Chip_TIMER_ClearMatch(m_apsBoard_Timer_Reg[eTimer], eBOARD_TIMER_CHANNLE_1);

                if(NULL != m_fpTimer3ChannleCallback[eBOARD_TIMER_CHANNLE_1])
                {
                    m_fpTimer3ChannleCallback[eBOARD_TIMER_CHANNLE_1]();
                }
            }
            // A70LH_Jonas_0001 End
            break;

        default:
            ASSERT_ALWAYS();
            break;
    }

    NVIC_ClearPendingIRQ(m_apsBoard_Timer_IRQ[eTimer]);
}

// ==============================================================================
// FUNCTION NAME: BOARD_TIMER_GETCLOCKRATE
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER eTimer:
//
// Returns:
//
//
// modification history
// --------------------
// 21/11/2013, Leo Create
// --------------------
// ==============================================================================
STATIC INLINE uint32_t Board_Timer_GetClockRate(eBOARD_TIMER eTimer)
{
    CHIP_CCU_CLK_T eTimer_Clk_Source[eBOARD_TIMER_NUMBERS] = {CLK_MX_TIMER0, CLK_MX_TIMER1, CLK_MX_TIMER2, CLK_MX_TIMER3};
    return Chip_Clock_GetRate(eTimer_Clk_Source[eTimer]);
}

// ==============================================================================
// FUNCTION NAME: BOARD_TIMER_INIT
// DESCRIPTION:
//
//
// Params:
// sBOARD_TIMER_CFG *psTimerCfg:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/04/29, Leo Create
// --------------------
// ==============================================================================
void Board_Timer_Init(sBOARD_TIMER_CFG *psTimerCfg)
{
    QWORD qwTimerClock = 0;

    if(NULL == psTimerCfg)
    {
        ASSERT_ALWAYS();
        return;
    }

    qwTimerClock = Board_Timer_GetClockRate(psTimerCfg->eTimerId);
    m_psTimerCfg[psTimerCfg->eTimerId] = psTimerCfg;

    Chip_TIMER_Init(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);

    if(psTimerCfg->dwPrescale > 0)
    {
        Chip_TIMER_PrescaleSet(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId], qwTimerClock / psTimerCfg->dwPrescale);
    }
    else
    {
        Chip_TIMER_PrescaleSet(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId], 0);
    }

    switch(psTimerCfg->eTimerMode)
    {
        case eBOARD_TIMER_MODE_TIMER:
            Chip_TIMER_SetMatch(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                psTimerCfg->uModeCfg.sTimerCfg.eTimerChannel,
                                psTimerCfg->uModeCfg.sTimerCfg.dwPeriod);

            Chip_TIMER_ResetOnMatchEnable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                          psTimerCfg->uModeCfg.sTimerCfg.eTimerChannel);

            Chip_TIMER_Reset(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);
            Chip_TIMER_Enable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);

            if(psTimerCfg->cEnableInt)
            {
                Chip_TIMER_MatchEnableInt(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                    psTimerCfg->uModeCfg.sTimerCfg.eTimerChannel);
            }
            break;

        case eBOARD_TIMER_MODE_COUNTER:
            Chip_TIMER_TIMER_SetCountClockSrc(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                              (TIMER_CAP_SRC_STATE_T)psTimerCfg->uModeCfg.sCounterCfg.eCounterMode,
                                              psTimerCfg->uModeCfg.sCounterCfg.eCounterChannel);

            Chip_TIMER_CaptureRisingEdgeDisable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                psTimerCfg->uModeCfg.sCounterCfg.eCounterChannel);

            Chip_TIMER_CaptureFallingEdgeDisable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                 psTimerCfg->uModeCfg.sCounterCfg.eCounterChannel);

            Chip_TIMER_CaptureDisableInt(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                         psTimerCfg->uModeCfg.sCounterCfg.eCounterChannel);

            Chip_TIMER_Reset(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);
            Chip_TIMER_Enable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);
            break;

        case eBOARD_TIMER_MODE_CAPTURE:
            switch(psTimerCfg->uModeCfg.sCaptureCfg.eCaptureMode)
            {
                case eBOARD_CAP_MODE_TIMER:
                    Chip_TIMER_CaptureRisingEdgeDisable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                        psTimerCfg->uModeCfg.sCaptureCfg.eCaptureChannel);

                    Chip_TIMER_CaptureFallingEdgeDisable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                         psTimerCfg->uModeCfg.sCaptureCfg.eCaptureChannel);
                    break;

                case eBOARD_CAP_MODE_RISE:
                    Chip_TIMER_CaptureRisingEdgeEnable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                       psTimerCfg->uModeCfg.sCaptureCfg.eCaptureChannel);

                    Chip_TIMER_CaptureFallingEdgeDisable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                         psTimerCfg->uModeCfg.sCaptureCfg.eCaptureChannel);
                    break;

                case eBOARD_CAP_MODE_FALL:
                    Chip_TIMER_CaptureRisingEdgeDisable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                        psTimerCfg->uModeCfg.sCaptureCfg.eCaptureChannel);

                    Chip_TIMER_CaptureFallingEdgeEnable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                        psTimerCfg->uModeCfg.sCaptureCfg.eCaptureChannel);
                    break;

                case eBOARD_CAP_MODE_BOTH:
                    Chip_TIMER_CaptureRisingEdgeEnable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                       psTimerCfg->uModeCfg.sCaptureCfg.eCaptureChannel);

                    Chip_TIMER_CaptureFallingEdgeEnable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                                        psTimerCfg->uModeCfg.sCaptureCfg.eCaptureChannel);
                    break;

                default:
                    ASSERT_ALWAYS();
                    break;
            } // End of Switch

            Chip_TIMER_CaptureEnableInt(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                        psTimerCfg->uModeCfg.sCaptureCfg.eCaptureChannel);
            Chip_TIMER_Reset(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);
            Chip_TIMER_Enable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);
            break;

        case eBOARD_TIMER_MODE_MATCH:
            Chip_TIMER_SetMatch(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                psTimerCfg->uModeCfg.sMatchCfg.eMatchChannel,
                                psTimerCfg->uModeCfg.sMatchCfg.dwMatchValue);

            if(psTimerCfg->uModeCfg.sMatchCfg.cIntOnMatch)
            {
                Chip_TIMER_MatchEnableInt(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                          psTimerCfg->uModeCfg.sMatchCfg.eMatchChannel);
            }
            else
            {
                Chip_TIMER_MatchDisableInt(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                           psTimerCfg->uModeCfg.sMatchCfg.eMatchChannel);
            }

            if(psTimerCfg->uModeCfg.sMatchCfg.cStopOnMatch)
            {
                Chip_TIMER_StopOnMatchEnable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                             psTimerCfg->uModeCfg.sMatchCfg.eMatchChannel);
            }
            else
            {
                Chip_TIMER_StopOnMatchDisable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                              psTimerCfg->uModeCfg.sMatchCfg.eMatchChannel);
            }

            if(psTimerCfg->uModeCfg.sMatchCfg.cResetOnMatch)
            {
                Chip_TIMER_ResetOnMatchEnable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                              psTimerCfg->uModeCfg.sMatchCfg.eMatchChannel);
            }
            else
            {
                Chip_TIMER_ResetOnMatchDisable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                               psTimerCfg->uModeCfg.sMatchCfg.eMatchChannel);
            }

            Chip_TIMER_ExtMatchControlSet(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                                          psTimerCfg->uModeCfg.sMatchCfg.cIntOnMatch,
                                          (TIMER_PIN_MATCH_STATE_T)psTimerCfg->uModeCfg.sMatchCfg.eExtMatchOutput,
                                          psTimerCfg->uModeCfg.sMatchCfg.eMatchChannel);

            Chip_TIMER_Reset(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);
            Chip_TIMER_Enable(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);
            break;

        default:
            ASSERT_ALWAYS();
            return;
    } // End of Switch

    if(psTimerCfg->cEnableInt)
    {
        /* preemption = 1, sub-priority = 1 */
        NVIC_SetPriority(m_apsBoard_Timer_IRQ[psTimerCfg->eTimerId], eINT_PRIORITY_LEVEL_3);

        NVIC_ClearPendingIRQ(m_apsBoard_Timer_IRQ[psTimerCfg->eTimerId]);

        /* Enable interrupt for timer 0 */
        NVIC_EnableIRQ(m_apsBoard_Timer_IRQ[psTimerCfg->eTimerId]);
    }
}

// ==============================================================================
// FUNCTION NAME: Board_Timer_DeInit
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER eTimer:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/07, Leo Create
// --------------------
// ==============================================================================
#if 0
void Board_Timer_DeInit(sBOARD_TIMER_CFG *psTimerCfg)
{
    NVIC_DisableIRQ(m_apsBoard_Timer_IRQ[psTimerCfg->eTimerId]);
    Chip_TIMER_DeInit(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId]);
}
#else
void Board_Timer_DeInit(eBOARD_TIMER eTimer)
{
    NVIC_DisableIRQ(m_apsBoard_Timer_IRQ[eTimer]);
    Chip_TIMER_DeInit(m_apsBoard_Timer_Reg[eTimer]);
}

#endif /* 0 */

// ==============================================================================
// FUNCTION NAME: Board_Timer_Reset
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER eTimer:
//
// Returns:
//
//
// Modification History
// --------------------
// 2016/07/13, Larry Create
// --------------------
// ==============================================================================
void Board_Timer_Reset(eBOARD_TIMER eTimer)
{
    Chip_TIMER_Reset(m_apsBoard_Timer_Reg[eTimer]);
    Chip_TIMER_Enable(m_apsBoard_Timer_Reg[eTimer]);
}

// ==============================================================================
// FUNCTION NAME: BOARD_TIMER_READCOUNT
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER eTimer:
//
// Returns:
//
//
// modification history
// --------------------
// 02/01/2014, Leo Create
// --------------------
// ==============================================================================
DWORD Board_Timer_ReadCount(eBOARD_TIMER eTimer)
{
    return Chip_TIMER_ReadCount(m_apsBoard_Timer_Reg[eTimer]);
}

// ==============================================================================
// FUNCTION NAME: Board_Timer_Set_Count_Src
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER eTimer:
// eBOARD_CAP_MODE eMode:
// eBOARD_TIMER_CHANNLE eChannel:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/11/02, Leo Create
// --------------------
// ==============================================================================
void Board_Timer_Set_Count_Src(eBOARD_TIMER eTimer, eBOARD_CAP_MODE eMode, eBOARD_TIMER_CHANNLE eChannel)
{
    Chip_TIMER_TIMER_SetCountClockSrc(m_apsBoard_Timer_Reg[eTimer],
                                      (TIMER_CAP_SRC_STATE_T)eMode,
                                      eChannel);
}

// ==============================================================================
// FUNCTION NAME: Board_Timer_Reset_Counter
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER eTimer:
// BYTE cEnable:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/11/02, Leo Create
//A35LS_Larry_0003 modify
// --------------------
// ==============================================================================
void Board_Timer_Reset_Counter(eBOARD_TIMER eTimer)
{
    Chip_TIMER_ResetCount(m_apsBoard_Timer_Reg[eTimer]);
}

// ==============================================================================
// FUNCTION NAME: BOARD_TIMER_REGISTER_CALLBACK
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER eTimer:
// fpTIMERCALLBACK fpTimerCallBack:
// Returns:
//
//
// modification history
// --------------------
// 2014/05/03, Leo Create
// --------------------
// ==============================================================================
void Board_Timer_Register_Callback(eBOARD_TIMER eTimer, fpTIMERCALLBACK fpTimerCallBack)
{
    m_fpTimerCallback[eTimer] = fpTimerCallBack;
}

// ==============================================================================
// FUNCTION NAME: Board_Timer_Reset
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER eTimer:
//
// Returns:
//
//
// Modification History
// --------------------
// 2016/07/13, Larry Create
//timer使用如果要複數個chaneel(ex:pwm, cap...)，只需要第一次init時reset一次即可
// --------------------
// ==============================================================================
void Board_Timer_RGU_Reset(eBOARD_TIMER eTimer)
{
    CHIP_RGU_RST_T m_aeBoard_Timer_RST[eBOARD_TIMER_NUMBERS] = {RGU_TIMER0_RST, RGU_TIMER1_RST, RGU_TIMER2_RST, RGU_TIMER3_RST};

    Chip_RGU_TriggerReset(m_aeBoard_Timer_RST[eTimer]);

    while(Chip_RGU_InReset(m_aeBoard_Timer_RST[eTimer])) {}

}

// ==============================================================================
// FUNCTION NAME: Board_Timer_Match_Set
// DESCRIPTION:
//
//
// Params:
// sBOARD_TIMER_CFG *psTimerCfg:
// DWORD dwValue:
//
// Returns:
//
//
// Modification History
// --------------------
// 2016/07/13, Larry Create
// --------------------
// ==============================================================================
void Board_Timer_Match_Set(sBOARD_TIMER_CFG *psTimerCfg, DWORD dwValue)
{
    Chip_TIMER_SetMatch(m_apsBoard_Timer_Reg[psTimerCfg->eTimerId],
                        psTimerCfg->uModeCfg.sMatchCfg.eMatchChannel,
                        dwValue);
}

// ==============================================================================
// FUNCTION NAME: Board_Timer_Timer3Channel_Callback
// DESCRIPTION:
//
//
// Params:
// eBOARD_TIMER_CHANNLE eChannel fpTIMERCALLBACK fpTimerCallBack:
//
// Returns:
//
//
// Modification History
// --------------------
// 2016/07/14, Larry Create
// --------------------
// ==============================================================================
void Board_Timer_Timer3Channel_Callback(eBOARD_TIMER_CHANNLE eChannel, fpTIMER3CHANNELCALLBACK fpTimerCallBack)
{
    m_fpTimer3ChannleCallback[eChannel] = fpTimerCallBack;
}

void TIMER0_IRQHandler(void)
{
    TIMER_IRQHandler(eBOARD_TIMER_TIMER0);
}

void TIMER1_IRQHandler(void)
{
    TIMER_IRQHandler(eBOARD_TIMER_TIMER1);
}

void TIMER2_IRQHandler(void)
{
    TIMER_IRQHandler(eBOARD_TIMER_TIMER2);
}

void TIMER3_IRQHandler(void)
{
    TIMER_IRQHandler(eBOARD_TIMER_TIMER3);
}

