// ==============================================================================
// FILE NAME: MCPWM_18XX_43XX.C
// DESCRIPTION: 
// 
// 
// modification history
// --------------------
// 29/12/2013, Leo Create
// --------------------
// ==============================================================================


#include "chip.h"

/* Initializes the MCPWM peripheral with specified parameter */
void Chip_MCPWM_Init(LPC_MCPWM_T *pMCPWM)
{
	/* Enable MCPWM clock */
	Chip_Clock_Enable(CLK_APB1_MOTOCON);
}

/* Initializes the MCPWM peripheral with specified parameter */
void Chip_MCPWM_DeInit(LPC_MCPWM_T *pMCPWM)
{
	/* Disable MCPWM clock */
	Chip_Clock_Disable(CLK_APB1_MOTOCON);
}

