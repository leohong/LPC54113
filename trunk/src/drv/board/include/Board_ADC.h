#ifndef BOARD_ADC_H
#define BOARD_ADC_H
// ===============================================================================
// FILE NAME: Board_ADC.h
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2015/09/10, Larry Create
// --------------------
// ===============================================================================

#include "CommonDef.h"

typedef enum
{
    eBOARD_ADC_0,
    eBOARD_ADC_1,

    eBOARD_ADC_NUMBERS,
} eBOARD_ADC;


typedef enum
{
    eBOARD_ADC_CH0 = 0,
    eBOARD_ADC_CH1,
    eBOARD_ADC_TEC_DMD_FB,
    eBOARD_ADC_CH3,
    eBOARD_ADC_LVPS_PWGD,   //12V //A70LH_Larry_0066
    eBOARD_ADC_LD_50V,      //50V //A70LH_Larry_0066
    eBOARD_ADC_KEYPAD1,     //KEY_ADC1
    eBOARD_ADC_KEYPAD2,     //KEY_ADC2

    eBOARD_ADC_CHANNEL_NUMBERS,
} eBOARD_ADC_CHANNEL;

typedef void (*fpADC_CALLBACK)(UINT16);

typedef struct
{
    eBOARD_ADC              eAdcID;
    eBOARD_ADC_CHANNEL      eAdcChn;
    UINT8                   cEnableInt;
    fpADC_CALLBACK          pfAdcCallBack;
} sADC_CH_CFG;

typedef enum
{
    eBOARD_ADC_10BITS = 0,
    eBOARD_ADC_9BITS,
    eBOARD_ADC_8BITS,
    eBOARD_ADC_7BITS,
    eBOARD_ADC_6BITS,
    eBOARD_ADC_5BITS,
    eBOARD_ADC_4BITS,
    eBOARD_ADC_3BITS,

    eBOARD_ADC_RESOLUTION_NUMBERS,
} eBOARD_ADC_RESOLUTION;

typedef struct
{
    UINT32                 dwRate;
    eBOARD_ADC_RESOLUTION eADC_Bits;
    BOOL                  bBurstMode;
} sBOARD_ADC_CFG;

void Board_ADC_Init(eBOARD_ADC eChannel);
eRESULT Board_ADC_Value_Read(eBOARD_ADC eChannel, eBOARD_ADC_CHANNEL eADC_CH, UINT16 *pwValue);
BOOL Board_ADC_Gpio_Get(eBOARD_ADC eChannel, eBOARD_ADC_CHANNEL eADC_CH);

#endif /* BOARD_ADC_H */
