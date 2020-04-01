/*
 * @brief LPC18xx/43xx Motor Control PWM driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
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

#ifndef __MCPWM_18XX_43XX_H_
#define __MCPWM_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MCPWM_18XX_43XX CHIP: LPC18xx/43xx Motor Control PWM driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief Motor Control PWM register block structure
 */
typedef struct {					/*!< MCPWM Structure        */
	__I  uint32_t  CON;				/*!< PWM Control read address */
	__O  uint32_t  CON_SET;			/*!< PWM Control set address */
	__O  uint32_t  CON_CLR;			/*!< PWM Control clear address */
	__I  uint32_t  CAPCON;			/*!< Capture Control read address */
	__O  uint32_t  CAPCON_SET;		/*!< Capture Control set address */
	__O  uint32_t  CAPCON_CLR;		/*!< Event Control clear address */
	__IO uint32_t TC[3];			/*!< Timer Counter register */
	__IO uint32_t LIM[3];			/*!< Limit register         */
	__IO uint32_t MAT[3];			/*!< Match register         */
	__IO uint32_t  DT;				/*!< Dead time register     */
	__IO uint32_t  CCP;				/*!< Communication Pattern register */
	__I  uint32_t CAP[3];			/*!< Capture register       */
	__I  uint32_t  INTEN;			/*!< Interrupt Enable read address */
	__O  uint32_t  INTEN_SET;		/*!< Interrupt Enable set address */
	__O  uint32_t  INTEN_CLR;		/*!< Interrupt Enable clear address */
	__I  uint32_t  CNTCON;			/*!< Count Control read address */
	__O  uint32_t  CNTCON_SET;		/*!< Count Control set address */
	__O  uint32_t  CNTCON_CLR;		/*!< Count Control clear address */
	__I  uint32_t  INTF;			/*!< Interrupt flags read address */
	__O  uint32_t  INTF_SET;		/*!< Interrupt flags set address */
	__O  uint32_t  INTF_CLR;		/*!< Interrupt flags clear address */
	__O  uint32_t  CAP_CLR;			/*!< Capture clear address  */
} LPC_MCPWM_T;


/** @defgroup MCPWM_Private_Macros MCPWM Private Macros
 * @{
 */
/** Edge aligned mode for channel in MCPWM */
#define MCPWM_CHANNEL_EDGE_MODE			((uint32_t)(0))
/** Center aligned mode for channel in MCPWM */
#define MCPWM_CHANNEL_CENTER_MODE		((uint32_t)(1))

/** Polarity of the MCOA and MCOB pins: Passive state is LOW, active state is HIGH */
#define MCPWM_CHANNEL_PASSIVE_LO		((uint32_t)(0))
/** Polarity of the MCOA and MCOB pins: Passive state is HIGH, active state is LOW */
#define MCPWM_CHANNEL_PASSIVE_HI		((uint32_t)(1))

/* Output Patent in 3-phase DC mode, the internal MCOA0 signal is routed to any or all of
 * the six output pins under the control of the bits in this register */
#define MCPWM_PATENT_A0		((uint32_t)(1<<0))	/**< MCOA0 tracks internal MCOA0 */
#define MCPWM_PATENT_B0		((uint32_t)(1<<1))	/**< MCOB0 tracks internal MCOA0 */
#define MCPWM_PATENT_A1		((uint32_t)(1<<2))	/**< MCOA1 tracks internal MCOA0 */
#define MCPWM_PATENT_B1		((uint32_t)(1<<3))	/**< MCOB1 tracks internal MCOA0 */
#define MCPWM_PATENT_A2		((uint32_t)(1<<4))	/**< MCOA2 tracks internal MCOA0 */
#define MCPWM_PATENT_B2		((uint32_t)(1<<5))	/**< MCOB2 tracks internal MCOA0 */

/* Interrupt type in MCPWM */
/** Limit interrupt for channel (0) */
#define MCPWM_INTFLAG_LIM0	MCPWM_INT_ILIM(0)
/** Match interrupt for channel (0) */
#define MCPWM_INTFLAG_MAT0	MCPWM_INT_IMAT(0)
/** Capture interrupt for channel (0) */
#define MCPWM_INTFLAG_CAP0	MCPWM_INT_ICAP(0)

/** Limit interrupt for channel (1) */
#define MCPWM_INTFLAG_LIM1	MCPWM_INT_ILIM(1)
/** Match interrupt for channel (1) */
#define MCPWM_INTFLAG_MAT1	MCPWM_INT_IMAT(1)
/** Capture interrupt for channel (1) */
#define MCPWM_INTFLAG_CAP1	MCPWM_INT_ICAP(1)

/** Limit interrupt for channel (2) */
#define MCPWM_INTFLAG_LIM2	MCPWM_INT_ILIM(2)
/** Match interrupt for channel (2) */
#define MCPWM_INTFLAG_MAT2	MCPWM_INT_IMAT(2)
/** Capture interrupt for channel (2) */
#define MCPWM_INTFLAG_CAP2	MCPWM_INT_ICAP(2)

/** Fast abort interrupt */
#define MCPWM_INTFLAG_ABORT	MCPWM_INT_ABORT

/*********************************************************************//**
 * Macro defines for MCPWM Control register
 **********************************************************************/
/* MCPWM Control register, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Control read address
 * - MCPWM Control set address
 * - MCPWM Control clear address
 */
/**< Stops/starts timer channel n */
#define MCPWM_CON_RUN(n)		(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*8)+0))) : (0))
/**< Edge/center aligned operation for channel n */
#define MCPWM_CON_CENTER(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+1))) : (0))
/**< Select polarity of the MCOAn and MCOBn pin */
#define MCPWM_CON_POLAR(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+2))) : (0))
/**< Control the dead-time feature for channel n */
#define MCPWM_CON_DTE(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+3))) : (0))
/**< Enable/Disable update of functional register for channel n */
#define MCPWM_CON_DISUP(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+4))) : (0))
/**< Control the polarity for all 3 channels */
#define MCPWM_CON_INVBDC		((uint32_t)(1<<29))
/**< 3-phase AC mode select */
#define MCPWM_CON_ACMODE		((uint32_t)(1<<30))
/**< 3-phase DC mode select */
#define MCPWM_CON_DCMODE		(((uint32_t)1<<31))

/*********************************************************************//**
 * Macro defines for MCPWM Capture Control register
 **********************************************************************/
/* Capture Control register, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Capture Control read address
 * - MCPWM Capture Control set address
 * - MCPWM Capture control clear address
 */
/** Enables/Disable channel (cap) capture event on a rising edge on MCI(mci) */
#define MCPWM_CAPCON_CAPMCI_RE(cap,mci)	(((cap<=2)&&(mci<=2)) ? ((uint32_t)(1<<((cap*6)+(mci*2)+0))) : (0))
/** Enables/Disable channel (cap) capture event on a falling edge on MCI(mci) */
#define MCPWM_CAPCON_CAPMCI_FE(cap,mci)	(((cap<=2)&&(mci<=2)) ? ((uint32_t)(1<<((cap*6)+(mci*2)+1))) : (0))
/** TC(n) is reset by channel (n) capture event */
#define MCPWM_CAPCON_RT(n)				(((n<=2)) ? ((uint32_t)(1<<(18+(n)))) : (0))
/** Hardware noise filter: channel (n) capture events are delayed */
#define MCPWM_CAPCON_HNFCAP(n)			(((n<=2)) ? ((uint32_t)(1<<(21+(n)))) : (0))

/*********************************************************************//**
 * Macro defines for MCPWM Interrupt register
 **********************************************************************/
/* Interrupt registers, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Interrupt Enable read address
 * - MCPWM Interrupt Enable set address
 * - MCPWM Interrupt Enable clear address
 * - MCPWM Interrupt Flags read address
 * - MCPWM Interrupt Flags set address
 * - MCPWM Interrupt Flags clear address
 */
/** Limit interrupt for channel (n) */
#define MCPWM_INT_ILIM(n)	(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*4)+0))) : (0))
/** Match interrupt for channel (n) */
#define MCPWM_INT_IMAT(n)	(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*4)+1))) : (0))
/** Capture interrupt for channel (n) */
#define MCPWM_INT_ICAP(n)	(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*4)+2))) : (0))
/** Fast abort interrupt */
#define MCPWM_INT_ABORT		((uint32_t)(1<<15))

/*********************************************************************//**
 * Macro defines for MCPWM Count Control register
 **********************************************************************/
/* MCPWM Count Control register, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Count Control read address
 * - MCPWM Count Control set address
 * - MCPWM Count Control clear address
 */
/** Counter(tc) advances on a rising edge on MCI(mci) pin */
#define MCPWM_CNTCON_TCMCI_RE(tc,mci)	(((tc<=2)&&(mci<=2)) ? ((uint32_t)(1<<((6*tc)+(2*mci)+0))) : (0))
/** Counter(cnt) advances on a falling edge on MCI(mci) pin */
#define MCPWM_CNTCON_TCMCI_FE(tc,mci)	(((tc<=2)&&(mci<=2)) ? ((uint32_t)(1<<((6*tc)+(2*mci)+1))) : (0))
/** Channel (n) is in counter mode */
#define MCPWM_CNTCON_CNTR(n)			(((n<=2)) ? ((uint32_t)(1<<(29+n))) : (0))

/*********************************************************************//**
 * Macro defines for MCPWM Dead-time register
 **********************************************************************/
/** Dead time value x for channel n */
#define MCPWM_DT(n,x)		(((n<=2)) ? ((uint32_t)((x&0x3FF)<<(n*10))) : (0))

/*********************************************************************//**
 * Macro defines for MCPWM Communication Pattern register
 **********************************************************************/
#define MCPWM_CP_A0		((uint32_t)(1<<0))	/**< MCOA0 tracks internal MCOA0 */
#define MCPWM_CP_B0		((uint32_t)(1<<1))	/**< MCOB0 tracks internal MCOA0 */
#define MCPWM_CP_A1		((uint32_t)(1<<2))	/**< MCOA1 tracks internal MCOA0 */
#define MCPWM_CP_B1		((uint32_t)(1<<3))	/**< MCOB1 tracks internal MCOA0 */
#define MCPWM_CP_A2		((uint32_t)(1<<4))	/**< MCOA2 tracks internal MCOA0 */
#define MCPWM_CP_B2		((uint32_t)(1<<5))	/**< MCOB2 tracks internal MCOA0 */

/*********************************************************************//**
 * Macro defines for MCPWM Capture clear address register
 **********************************************************************/
/** Clear the MCCAP (n) register */
#define MCPWM_CAPCLR_CAP(n)		(((n<=2)) ? ((uint32_t)(1<<n)) : (0))


/**
 * @}
 */


/* Public Types --------------------------------------------------------------- */
/** @defgroup MCPWM_Public_Types MCPWM Public Types
 * @{
 */

/*********************************************************************//**
 * @brief MCPWM enumeration
 **********************************************************************/
/**
 * @brief	MCPWM channel identifier definition
 */
typedef enum CHIP_MCPWM_CHANNEL
{
	MCPWM_CHANNEL_0 = 0,		/**< MCPWM channel 0 */
	MCPWM_CHANNEL_1,			/**< MCPWM channel 1 */
	MCPWM_CHANNEL_2				/**< MCPWM channel 2 */
} MCPWM_CHANNEL_T;

// ==============================================================================
// MCPWM Control
// ==============================================================================
STATIC INLINE uint32_t Chip_MCPWM_GetCON(LPC_MCPWM_T *pMCPWM)   // A70_Jonas_0001, Added
{
	return (uint32_t) (pMCPWM->CON);
}

STATIC INLINE bool Chip_MCPWM_GetRunEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)  // A70_Jonas_0001, Added
{
	return (bool)((pMCPWM->CON) & MCPWM_CON_RUN(channelNum) ? 1 : 0);
}

STATIC INLINE void Chip_MCPWM_RunEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_SET = MCPWM_CON_RUN(channelNum);
}

STATIC INLINE void Chip_MCPWM_RunDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_CLR = MCPWM_CON_RUN(channelNum);
}

STATIC INLINE void Chip_MCPWM_CenteralignedEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_SET = MCPWM_CON_CENTER(channelNum);
}

STATIC INLINE void Chip_MCPWM_CenteralignedDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_CLR = MCPWM_CON_CENTER(channelNum);
}

STATIC INLINE void Chip_MCPWM_PolarityPassiveHiEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_SET = MCPWM_CON_POLAR(channelNum);
}

STATIC INLINE void Chip_MCPWM_PolarityPassiveHiDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_CLR = MCPWM_CON_POLAR(channelNum);
}

STATIC INLINE void Chip_MCPWM_DeadtimeEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_SET = MCPWM_CON_DTE(channelNum);
}

STATIC INLINE void Chip_MCPWM_DeadtimeDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_CLR = MCPWM_CON_DTE(channelNum);
}

STATIC INLINE void Chip_MCPWM_DisableUpdatingEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_SET = MCPWM_CON_DISUP(channelNum);
}

STATIC INLINE void Chip_MCPWM_DisableUpdatingDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CON_CLR = MCPWM_CON_DISUP(channelNum);
}

// ==============================================================================
// Capture Control
// ==============================================================================
STATIC INLINE void Chip_MCPWM_CaptureRisingEdgeEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum, int8_t inputChannel)
{
    pMCPWM->CAPCON_SET = MCPWM_CAPCON_CAPMCI_RE(channelNum, inputChannel);
}

STATIC INLINE void Chip_MCPWM_CaptureRisingEdgeDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum, int8_t inputChannel)
{
    pMCPWM->CAPCON_CLR = MCPWM_CAPCON_CAPMCI_RE(channelNum, inputChannel);
}

STATIC INLINE void Chip_MCPWM_CaptureRisingFallingEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum, int8_t inputChannel)
{
    pMCPWM->CAPCON_SET = MCPWM_CAPCON_CAPMCI_FE(channelNum, inputChannel);
}

STATIC INLINE void Chip_MCPWM_CaptureRisingFallingDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum, int8_t inputChannel)
{
    pMCPWM->CAPCON_CLR = MCPWM_CAPCON_CAPMCI_FE(channelNum, inputChannel);
}

STATIC INLINE void Chip_MCPWM_CaptureEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CAPCON_SET = MCPWM_CAPCON_RT(channelNum);
}

STATIC INLINE void Chip_MCPWM_CaptureDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CAPCON_CLR = MCPWM_CAPCON_RT(channelNum);
}

// ==============================================================================
// Timer Counter
// ==============================================================================
STATIC INLINE void Chip_MCPWM_SetCounter(LPC_MCPWM_T *pMCPWM, int8_t channelNum, uint32_t countVal)
{
    pMCPWM->TC[channelNum] = countVal;
}

STATIC INLINE uint32_t Chip_MCPWM_ReadCounter(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
	return pMCPWM->TC[channelNum];
}

// ==============================================================================
// Limit register
// ==============================================================================
STATIC INLINE void Chip_MCPWM_SetLimit(LPC_MCPWM_T *pMCPWM, int8_t channelNum, uint32_t limVal)
{
    pMCPWM->LIM[channelNum] = limVal;
}

STATIC INLINE uint32_t Chip_MCPWM_ReadLimit(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
	return pMCPWM->LIM[channelNum];
}

// ==============================================================================
// Match register
// ==============================================================================
STATIC INLINE void Chip_MCPWM_SetMatch(LPC_MCPWM_T *pMCPWM, int8_t channelNum, uint32_t matVal)
{
    pMCPWM->MAT[channelNum] = matVal;
}

STATIC INLINE uint32_t Chip_MCPWM_ReadMatch(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
	return pMCPWM->MAT[channelNum];
}

// ==============================================================================
// Capture register
// ==============================================================================
STATIC INLINE uint32_t Chip_MCPWM_ReadCapture(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
	return pMCPWM->CAP[channelNum];
}

// ==============================================================================
// Interrupt Enable
// ==============================================================================
STATIC INLINE void Chip_MCPWM_LimitIntSet(LPC_MCPWM_T *pMCPWM, int8_t limnum)
{
    pMCPWM->INTEN_SET = MCPWM_INT_ILIM(limnum);
}

STATIC INLINE void Chip_MCPWM_MatchIntSet(LPC_MCPWM_T *pMCPWM, int8_t matchnum)
{
    pMCPWM->INTEN_SET = MCPWM_INT_IMAT(matchnum);
}

STATIC INLINE void Chip_MCPWM_CaptureIntSet(LPC_MCPWM_T *pMCPWM, int8_t capnum)
{
    pMCPWM->INTEN_SET = MCPWM_INT_ICAP(capnum);
}

STATIC INLINE void Chip_MCPWM_AbortIntSet(LPC_MCPWM_T *pMCPWM)
{
    pMCPWM->INTEN_SET = MCPWM_INT_ABORT;
}

STATIC INLINE void Chip_MCPWM_LimitIntDisable(LPC_MCPWM_T *pMCPWM, int8_t limnum)
{
    pMCPWM->INTEN_CLR = MCPWM_INT_ILIM(limnum);
}

STATIC INLINE void Chip_MCPWM_MatchIntDisable(LPC_MCPWM_T *pMCPWM, int8_t matchnum)
{
    pMCPWM->INTEN_CLR = MCPWM_INT_IMAT(matchnum);
}

STATIC INLINE void Chip_MCPWM_CaptureIntDisable(LPC_MCPWM_T *pMCPWM, int8_t capnum)
{
    pMCPWM->INTEN_CLR = MCPWM_INT_ICAP(capnum);
}

STATIC INLINE void Chip_MCPWM_AbortIntDisable(LPC_MCPWM_T *pMCPWM)
{
    pMCPWM->INTEN_CLR = MCPWM_INT_ABORT;
}

// ==============================================================================
// Counter Control
// ==============================================================================
STATIC INLINE void Chip_MCPWM_CountRisingEdgeEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum, int8_t inputChannel)
{
    pMCPWM->CNTCON_SET = MCPWM_CNTCON_TCMCI_RE(channelNum, inputChannel);
}

STATIC INLINE void Chip_MCPWM_CountRisingEdgeDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum, int8_t inputChannel)
{
    pMCPWM->CNTCON_CLR = MCPWM_CNTCON_TCMCI_RE(channelNum, inputChannel);
}

STATIC INLINE void Chip_MCPWM_CountRisingFallingEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum, int8_t inputChannel)
{
    pMCPWM->CNTCON_SET = MCPWM_CNTCON_TCMCI_FE(channelNum, inputChannel);
}

STATIC INLINE void Chip_MCPWM_CountRisingFallingDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum, int8_t inputChannel)
{
    pMCPWM->CNTCON_CLR = MCPWM_CNTCON_TCMCI_FE(channelNum, inputChannel);
}

STATIC INLINE void Chip_MCPWM_CountEnable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CNTCON_SET = MCPWM_CNTCON_CNTR(channelNum);
}

STATIC INLINE void Chip_MCPWM_CountDisable(LPC_MCPWM_T *pMCPWM, int8_t channelNum)
{
    pMCPWM->CNTCON_CLR = MCPWM_CNTCON_CNTR(channelNum);
}

// ==============================================================================
// Interrupt flag
// ==============================================================================
STATIC INLINE uint32_t Chip_MCPWM_GetIntFlag(LPC_MCPWM_T *pMCPWM)   // A70_Jonas_0001, Added
{
	return (uint32_t) (pMCPWM->INTF);
}

STATIC INLINE bool Chip_MCPWM_GetLimitIntFlag(LPC_MCPWM_T *pMCPWM, int8_t limnum)
{
	return (bool) ((pMCPWM->INTF & MCPWM_INT_ILIM(limnum)) != 0);
}

STATIC INLINE bool Chip_MCPWM_GetMatchIntFlag(LPC_MCPWM_T *pMCPWM, int8_t matchnum)
{
	return (bool) ((pMCPWM->INTF & MCPWM_INT_IMAT(matchnum)) != 0);
}

STATIC INLINE bool Chip_MCPWM_GetCaptureIntFlag(LPC_MCPWM_T *pMCPWM, int8_t capnum)
{
	return (bool) ((pMCPWM->INTF & MCPWM_INT_ICAP(capnum)) != 0);
}

STATIC INLINE bool Chip_MCPWM_GetAbortIntFlag(LPC_MCPWM_T *pMCPWM)
{
	return (bool) ((pMCPWM->INTF & MCPWM_INT_ABORT) != 0);
}

STATIC INLINE void Chip_MCPWM_SetLimitIntFlag(LPC_MCPWM_T *pMCPWM, int8_t limnum)
{
	pMCPWM->INTF_SET = MCPWM_INT_ILIM(limnum);
}

STATIC INLINE void Chip_MCPWM_SetMatchIntFlag(LPC_MCPWM_T *pMCPWM, int8_t matchnum)
{
	pMCPWM->INTF_SET = MCPWM_INT_IMAT(matchnum);
}

STATIC INLINE void Chip_MCPWM_SetCaptureIntFlag(LPC_MCPWM_T *pMCPWM, int8_t capnum)
{
	pMCPWM->INTF_SET = MCPWM_INT_ICAP(capnum);
}

STATIC INLINE void Chip_MCPWM_SetAbortIntFlag(LPC_MCPWM_T *pMCPWM)
{
	pMCPWM->INTF_SET = MCPWM_INT_ABORT;
}

STATIC INLINE void Chip_MCPWM_ClearLimitIntFlag(LPC_MCPWM_T *pMCPWM, int8_t limnum)
{
	pMCPWM->INTF_CLR = MCPWM_INT_ILIM(limnum);
}

STATIC INLINE void Chip_MCPWM_ClearMatchIntFlag(LPC_MCPWM_T *pMCPWM, int8_t matchnum)
{
	pMCPWM->INTF_CLR = MCPWM_INT_IMAT(matchnum);
}

STATIC INLINE void Chip_MCPWM_ClearCaptureIntFlag(LPC_MCPWM_T *pMCPWM, int8_t capnum)
{
	pMCPWM->INTF_CLR = MCPWM_INT_ICAP(capnum);
}

STATIC INLINE void Chip_MCPWM_ClearAbortIntFlag(LPC_MCPWM_T *pMCPWM)
{
	pMCPWM->INTF_CLR = MCPWM_INT_ABORT;
}

// ==============================================================================
// Capture clear
// ==============================================================================
STATIC INLINE void Chip_MCPWM_ClearCapture(LPC_MCPWM_T *pMCPWM, int8_t capnum)
{
	pMCPWM->CAP_CLR = MCPWM_CAPCLR_CAP(capnum);
}

/**
 * @brief	Initializes MCPWM
 * @param	pMCPWM	: Pointer to MCPWM peripheral block structure
 * @return	Nothing
 */
void Chip_MCPWM_Init(LPC_MCPWM_T *pMCPWM);

/**
 * @brief	DeInitializes MCPWM
 * @param	pMCPWM	: Pointer to MCPWM peripheral block structure
 * @return	Nothing
 */
void Chip_MCPWM_DeInit(LPC_MCPWM_T *pMCPWM);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __MCPWM_18XX_43XX_H_ */
