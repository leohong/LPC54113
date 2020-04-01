// ===============================================================================
// FILE NAME: Board_ADC.c
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2015/09/10, Larry Create
// --------------------
// ===============================================================================

#include "Board.h"
#include "Board_ADC.h"

#include "utilLogAPI.h"

static LPC_ADC_T *m_psLPC_ADC[eBOARD_ADC_NUMBERS] = {LPC_ADC0, LPC_ADC1};

static const sBOARD_ADC_CFG m_sADC_Channel[eBOARD_ADC_NUMBERS] =
{
    //ADC 0
    {
        4000,
        eBOARD_ADC_10BITS,
        TRUE,
    },
    //ADC 1
    {
        4000,
        eBOARD_ADC_10BITS,
        TRUE,
    }
};

// ==============================================================================
// FUNCTION NAME: Board_ADC_Init
// DESCRIPTION:
//
//
// Params:
// eBOARD_ADC eChannel:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/09/10, Larry Create
// --------------------
// ==============================================================================
void Board_ADC_Init(eBOARD_ADC eChannel)
{
    //LPC4337 只有一組專用的ADC，
    //另一組需要設定multiple function pin
    ADC_CLOCK_SETUP_T sADCSetup;

    ASSERT(eBOARD_ADC_NUMBERS > eChannel);
    ASSERT(eBOARD_ADC_RESOLUTION_NUMBERS > m_sADC_Channel[eChannel].eADC_Bits);
    ASSERT(ADC_MAX_SAMPLE_RATE > m_sADC_Channel[eChannel].dwRate);

    if(eBOARD_ADC_NUMBERS > eChannel)
    {
        Chip_ADC_Init(m_psLPC_ADC[eChannel], &sADCSetup);

        sADCSetup.burstMode = (bool)m_sADC_Channel[eChannel].bBurstMode;

        Chip_ADC_SetSampleRate(m_psLPC_ADC[eChannel], &sADCSetup, (uint32_t)m_sADC_Channel[eChannel].dwRate);

        Chip_ADC_SetResolution(m_psLPC_ADC[eChannel], &sADCSetup, (ADC_RESOLUTION_T)m_sADC_Channel[eChannel].eADC_Bits);
    }
}

// ==============================================================================
// FUNCTION NAME: Board_ADC_Value_Read
// DESCRIPTION:
//
//
// Params:
// eBOARD_ADC eChannel:
// eBOARD_ADC_CHANNEL eADC_CH:
// WORD *pwValue:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/09/10, Larry Create
// --------------------
// ==============================================================================
eRESULT Board_ADC_Value_Read(eBOARD_ADC eChannel, eBOARD_ADC_CHANNEL eADC_CH, UINT16 *pwValue)
{
    eRESULT eResult = rcSUCCESS;

    ASSERT(eBOARD_ADC_CHANNEL_NUMBERS > eADC_CH);
    ASSERT(eBOARD_ADC_NUMBERS > eChannel);

    if(eBOARD_ADC_CHANNEL_NUMBERS > eChannel)
    {
        // Select ADC channel
        Chip_ADC_EnableChannel(m_psLPC_ADC[eChannel], (ADC_CHANNEL_T)eADC_CH, ENABLE);

        Chip_ADC_SetStartMode(m_psLPC_ADC[eChannel], ADC_START_NOW, ADC_TRIGGERMODE_RISING);

        /* Waiting for A/D conversion complete */
        if(Chip_ADC_ReadStatus(m_psLPC_ADC[eChannel], (ADC_CHANNEL_T)eADC_CH, ADC_DR_DONE_STAT) != SET)
        {
            while(Chip_ADC_ReadStatus(m_psLPC_ADC[eChannel], (ADC_CHANNEL_T)eADC_CH, ADC_DR_DONE_STAT) != SET) {}
        }

        /* Read ADC value */
        if(Chip_ADC_ReadValue(m_psLPC_ADC[eChannel], (ADC_CHANNEL_T)eADC_CH, pwValue) != ERROR)
        {
            eResult = rcSUCCESS;
        }

        Chip_ADC_EnableChannel(m_psLPC_ADC[eChannel], (ADC_CHANNEL_T)eADC_CH, DISABLE);
    }

    return eResult;
}



// ==============================================================================
// FUNCTION NAME: Board_ADC_Gpio_Get
// DESCRIPTION:
//
//
// Params:
// eBOARD_ADC eChannel:
// eBOARD_ADC_CHANNEL eADC_CH:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/09/10, Larry Create
// --------------------
// ==============================================================================
BOOL Board_ADC_Gpio_Get(eBOARD_ADC eChannel, eBOARD_ADC_CHANNEL eADC_CH)
{
    eRESULT eResult = rcSUCCESS;
    UINT16 wValue = 0;

    ASSERT(eBOARD_ADC_CHANNEL_NUMBERS > eADC_CH);

    eResult = Board_ADC_Value_Read(eChannel, eADC_CH, &wValue);

    if(eResult == rcSUCCESS && wValue > 512)
    {
        return TRUE;
    }

    return FALSE;
}





