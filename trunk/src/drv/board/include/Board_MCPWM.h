#ifndef BOARD_MCPWM_H
#define BOARD_MCPWM_H
// ==============================================================================
// FILE NAME: BOARD_MCPWM.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 26/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"

typedef enum
{
    eBOARD_MCPWM_CHANNEL0 = 0,
    eBOARD_MCPWM_CHANNEL1,
    eBOARD_MCPWM_CHANNEL2,

    eBOARD_MCPWM_NUMBERS,
} eBOARD_MCPWM;

void Board_MCPWM_Init(eBOARD_MCPWM eChannel);
void Board_MCPWM_DeInit(void);
void Board_MCPWM_Start(eBOARD_MCPWM eChannel);
void Board_MCPWM_Stop(eBOARD_MCPWM eChannel);
void Board_Mcpwm_Register_Callback(eBOARD_MCPWM eChannel, void (*fpCallback)(void));
void Board_MCPWM_Freq_Set(eBOARD_MCPWM eChannel, DWORD dwFreq);//A70_Larry_0014

#endif /* BOARD_MCPWM_H */
