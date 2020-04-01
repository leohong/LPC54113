#ifndef BOARD_CLOCK_H
#define BOARD_CLOCK_H
// ==============================================================================
// FILE NAME: BOARD_CLOCK.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 28/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"

typedef enum
{
    eBOARD_CLOCK_12MHZ,
    eBOARD_CLOCK_50MHZ,
    eBOARD_CLOCK_204MHZ,

    eBOARD_CLOCK_NUMBERS,
} eBOARD_CLOCK;

void Board_Clock_Init(eBOARD_CLOCK eClock);

#endif /* BOARD_CLOCK_H */
