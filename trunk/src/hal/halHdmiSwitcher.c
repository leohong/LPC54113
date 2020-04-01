// ===============================================================================
// FILE NAME: halHdmiSwitcher.c
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2018/10/23, Leo Create
// --------------------
// ===============================================================================


#include "utilCounterAPI.h"
#include "utilLogAPI.h"
#include "utilEventHandlerAPI.h"
#include "halBoardCtrlAPI.h"
#include "halHdmiSwitcher.h"
#include "halCard.h"
#include "Board_Gpio.h"

#include "dvCard.h"

//StaticSemaphore_t   m_axHdmiSwitcherSemaphoreBuffer;
//SemaphoreHandle_t   m_xHdmiSwitcherSemaphore;

static sHAL_CONFIG m_sHalConfig;

// ==============================================================================
// FUNCTION NAME: halHdmiSwitcher_LED_Ctrl
// DESCRIPTION:
//
//
// PARAMETERS:
// eSOURCE_LIST eSource:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/10/25, Leo Create
// --------------------
// ==============================================================================
static void halHdmiSwitcher_LED_Ctrl(eSOURCE_LIST eSource)
{
    BYTE LED1_Value = HIGH;
    BYTE LED2_Value = HIGH;

    switch(eSource)
    {
        case eSOURCE_LIST_OPS_HDMI:
            LED1_Value = LOW;
            LED2_Value = HIGH;
            break;


        case eSOURCE_LIST_OPS_DP:
            LED1_Value = HIGH;
            LED2_Value = LOW;
            break;

        case eSOURCE_LIST_SLOT_0:
        case eSOURCE_LIST_SLOT_1:
        case eSOURCE_LIST_SLOT_2:
        case eSOURCE_LIST_SLOT_3:
            LED1_Value = LOW;
            LED2_Value = LOW;
            break;

        default:
            // m_cCurrentRXPort = InRxPort1;
            break;
    }

    Board_Gpio_Set(pLED1_SINK, (BYTE)LED1_Value);
    Board_Gpio_Set(pLED2_SINK, (BYTE)LED2_Value);
}

// ==============================================================================
// FUNCTION NAME: halHdmiSwitcher_Init
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
// 2019/10/25, Leo Create
// --------------------
// ==============================================================================
void halHdmiSwitcher_Init(void)
{
    halHdmiSwitcher_LED_Ctrl(m_sHalConfig.eCurrentSource);
}

// ==============================================================================
// FUNCTION NAME: halHdmiSwitcher_Ctrl
// DESCRIPTION:
//
//
// PARAMETERS:
// eSWITCHER_CMD eCmd:
// void* pvBuffer:
//
// RETURNS:
//
//
// modification history
// --------------------
// 2019/10/25, Leo Create
// --------------------
// ==============================================================================
void halHdmiSwitcher_Ctrl(eSWITCHER_CMD eCmd, void* pvBuffer)
{
    switch(eCmd)
    {
        case eSWITCHER_CMD_RESET:
            break;

        case eSWITCHER_CMD_SWITCH_PORT:
        {
            eSOURCE_LIST *peNewSource = (eSOURCE_LIST*)pvBuffer;

            if(*peNewSource != m_sHalConfig.eCurrentSource)
            {
                BYTE cData = 0;

                switch(*peNewSource)
                {
                    case eSOURCE_LIST_OPS_HDMI:
                        cData = DB_OPS_HDMI;
                        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Switch to OPS HDMI\r\n");
                        break;

                    case eSOURCE_LIST_OPS_DP:
                        cData = DB_OPS_DP;
                        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Switch to OPS DP\r\n");
                        break;

                    case eSOURCE_LIST_SLOT_0:
                    case eSOURCE_LIST_SLOT_1:
                    case eSOURCE_LIST_SLOT_2:
                    case eSOURCE_LIST_SLOT_3:
                    {
                        eSOURCE_PORT ePort = (eSOURCE_PORT)(*peNewSource - eSOURCE_LIST_SLOT_0);

                        if(ePort > halCard_Slot_Input_Numbers_Get())
                        {
                            break;
                        }

                        switch(m_sHalConfig.eCurrentSource)
                        {
                            case eSOURCE_LIST_OPS_HDMI:
                            case eSOURCE_LIST_OPS_DP:
                                cData = DB_CARD;
                                LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Switch to Card\r\n");
                                break;

                            default:
                                break;
                        }

                        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Switch Card Input to %d\r\n", ePort);
                        dvCard_Input_Port_Set(ePort);
                    }
                    break;

                    default:
                        break;
                }

                (void)cData;
                halHdmiSwitcher_LED_Ctrl(*peNewSource);
                m_sHalConfig.eCurrentSource = *peNewSource;
            }
        }
        break;

        case eSWITCHER_CMD_FUNC_TEST:
        {
            BYTE            acVersion[4];
            sVIDEO_INFO     sVideoInfo;
            sAUDIO_INFO     sAudioInfo;
            sVIDEO_TIMING   sVideoTiming;
            sVIDEO_FORMAT   sVideoFormat;
            sSLOT_INFO      sSlotInfo;

            dvCard_System_Revision_Get(acVersion);
            dvCard_Video_Info_Get(&sVideoInfo);
            dvCard_Audio_Info_Get(&sAudioInfo);
            dvCard_Video_Timing_Get(&sVideoTiming);
            dvCard_Video_Format_Get(&sVideoFormat);
            dvCard_Slot_Info_Get(&sSlotInfo);
        }
        break;

        default:
            break;
    }
}
