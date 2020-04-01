/*
 * @brief LPC18xx/43xx GIMA driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licenser disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __GIMA_18XX_43XX_H_
#define __GIMA_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup GIMA_18XX_43XX CHIP: LPC18xx/43xx GIMA driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief Global Input Multiplexer Array (GIMA) register block structure
 */
typedef struct {						/*!< GIMA Structure */
	__IO uint32_t  CAP0_IN[4][4];		/*!< Timer x CAP0_y capture input multiplexer (GIMA output ((x*4)+y)) */
	__IO uint32_t  CTIN_IN[8];			/*!< SCT CTIN_x capture input multiplexer (GIMA output (16+x)) */
	__IO uint32_t  ADCHS_TRIGGER_IN;	/*!< ADCHS trigger input multiplexer (GIMA output 24) */
	__IO uint32_t  EVENTROUTER_13_IN;	/*!< Event router input 13 multiplexer (GIMA output 25) */
	__IO uint32_t  EVENTROUTER_14_IN;	/*!< Event router input 14 multiplexer (GIMA output 26) */
	__IO uint32_t  EVENTROUTER_16_IN;	/*!< Event router input 16 multiplexer (GIMA output 27) */
	__IO uint32_t  ADCSTART0_IN;		/*!< ADC start0 input multiplexer (GIMA output 28) */
	__IO uint32_t  ADCSTART1_IN;		/*!< ADC start1 input multiplexer (GIMA output 29) */
} LPC_GIMA_T;

/**
 * @}
 */

#define GIMA_INV(x)             ((x)&0x01 << 0)
#define GIMA_EDGE(x)            ((x)&0x01 << 1)
#define GIMA_SYNCH(x)           ((x)&0x01 << 2)
#define GIMA_PLUSE(x)           ((x)&0x01 << 3)
#define GIMA_SELECT_INPUT(x)    ((x << 4)&0x3F)

typedef enum GIMA_CAP_IN
{
    GIMA_CAP_IN_TIME0,
    GIMA_CAP_IN_TIME1,
    GIMA_CAP_IN_TIME2,
    GIMA_CAP_IN_TIME3,

    GIMA_CAP_IN_NUMBERS,
} GIMA_CAP_IN_T;

typedef enum GIMA_CAP_CHANNEL
{
    GIMA_CAP_CHANNEL0,
    GIMA_CAP_CHANNEL1,
    GIMA_CAP_CHANNEL2,
    GIMA_CAP_CHANNEL3,

    GIMA_CAP_CHANNEL_NUMBERS,
} GIMA_CAP_CHANNEL_T;

typedef enum GIMA_CTIN_IN
{
    GIMA_CTIN_0_IN,
    GIMA_CTIN_1_IN,
    GIMA_CTIN_2_IN,
    GIMA_CTIN_3_IN,
    GIMA_CTIN_4_IN,
    GIMA_CTIN_5_IN,
    GIMA_CTIN_6_IN,
    GIMA_CTIN_7_IN,

    GIMA_CTIN_NUMBERS,
} GIMA_CTIN_IN_T;

typedef enum GIMA_FUNC
{
    GIMA_FUNC_CAP,
    GIMA_FUNC_SCT_CTIN,
    GIMA_FUNC_ADCHS_TRIGGER,
    GIMA_FUNC_EVENTROUTER_13,
    GIMA_FUNC_EVENTROUTER_14,
    GIMA_FUNC_EVENTROUTER_16,
    GIMA_FUNC_ADCSTART0,
    GIMA_FUNC_ADCSTART1,

    GIMA_FUNC_NUMBERS,
} GIMA_FUNC_T;

STATIC INLINE void Chip_Gima_CAP_IN_PinMux(GIMA_CAP_IN_T input,
                                           GIMA_CAP_CHANNEL_T channel,
                                           FunctionalState invState,
                                           FunctionalState edgeState,
                                           FunctionalState synchState,
                                           FunctionalState pluseState,
                                           uint32_t select
                                           )
{
    LPC_GIMA->CAP0_IN[input][channel] = GIMA_INV(invState)|
                                        GIMA_EDGE(edgeState)|
                                        GIMA_SYNCH(synchState)|
                                        GIMA_PLUSE(pluseState)|
                                        GIMA_SELECT_INPUT(select);
};

STATIC INLINE void Chip_Gima_CTIN_IN_PinMux(GIMA_CTIN_IN_T  input,
                                            FunctionalState invState,
                                            FunctionalState edgeState,
                                            FunctionalState synchState,
                                            FunctionalState pluseState,
                                            uint32_t select
                                            )
{
    LPC_GIMA->CTIN_IN[input] = GIMA_INV(invState)|
                               GIMA_EDGE(edgeState)|
                               GIMA_SYNCH(synchState)|
                               GIMA_PLUSE(pluseState)|
                               GIMA_SELECT_INPUT(select);
};

STATIC INLINE void Chip_Gima_ADCHS_TRIGGER_IN_PinMux(FunctionalState invState,
                                                     FunctionalState edgeState,
                                                     FunctionalState synchState,
                                                     FunctionalState pluseState,
                                                     uint32_t select
                                                     )
{
    LPC_GIMA->ADCHS_TRIGGER_IN = GIMA_INV(invState)|
                                 GIMA_EDGE(edgeState)|
                                 GIMA_SYNCH(synchState)|
                                 GIMA_PLUSE(pluseState)|
                                 GIMA_SELECT_INPUT(select);
};

STATIC INLINE void Chip_Gima_EVENTROUTER_13_IN_PinMux(FunctionalState invState,
                                                     FunctionalState edgeState,
                                                     FunctionalState synchState,
                                                     FunctionalState pluseState,
                                                     uint32_t select
                                                     )
{
    LPC_GIMA->EVENTROUTER_13_IN = GIMA_INV(invState)|
                                  GIMA_EDGE(edgeState)|
                                  GIMA_SYNCH(synchState)|
                                  GIMA_PLUSE(pluseState)|
                                  GIMA_SELECT_INPUT(select);
};

STATIC INLINE void Chip_Gima_EVENTROUTER_14_IN_PinMux(FunctionalState invState,
                                                      FunctionalState edgeState,
                                                      FunctionalState synchState,
                                                      FunctionalState pluseState,
                                                      uint32_t select
                                                      )
{
    LPC_GIMA->EVENTROUTER_14_IN = GIMA_INV(invState)|
                                  GIMA_EDGE(edgeState)|
                                  GIMA_SYNCH(synchState)|
                                  GIMA_PLUSE(pluseState)|
                                  GIMA_SELECT_INPUT(select);
};

STATIC INLINE void Chip_Gima_EVENTROUTER_16_IN_PinMux(FunctionalState invState,
                                                      FunctionalState edgeState,
                                                      FunctionalState synchState,
                                                      FunctionalState pluseState,
                                                      uint32_t select
                                                      )
{
    LPC_GIMA->EVENTROUTER_16_IN = GIMA_INV(invState)|
                                  GIMA_EDGE(edgeState)|
                                  GIMA_SYNCH(synchState)|
                                  GIMA_PLUSE(pluseState)|
                                  GIMA_SELECT_INPUT(select);
};

STATIC INLINE void Chip_Gima_ADCSTART0_IN_PinMux(FunctionalState invState,
                                                 FunctionalState edgeState,
                                                 FunctionalState synchState,
                                                 FunctionalState pluseState,
                                                 uint32_t select
                                                 )
{
    LPC_GIMA->ADCSTART0_IN = GIMA_INV(invState)|
                             GIMA_EDGE(edgeState)|
                             GIMA_SYNCH(synchState)|
                             GIMA_PLUSE(pluseState)|
                             GIMA_SELECT_INPUT(select);
};

STATIC INLINE void Chip_Gima_ADCSTART1_IN_PinMux(FunctionalState invState,
                                                 FunctionalState edgeState,
                                                 FunctionalState synchState,
                                                 FunctionalState pluseState,
                                                 uint32_t select
                                                 )
{
    LPC_GIMA->ADCSTART1_IN = GIMA_INV(invState)|
                             GIMA_EDGE(edgeState)|
                             GIMA_SYNCH(synchState)|
                             GIMA_PLUSE(pluseState)|
                             GIMA_SELECT_INPUT(select);
};

#ifdef __cplusplus
}
#endif

#endif /* __GIMA_18XX_43XX_H_ */
