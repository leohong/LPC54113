#ifndef PLATFORMDEF_H
#define PLATFORMDEF_H
// ==============================================================================
// FILE NAME: PLATFORMDEF.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 15/03/2013, Leohong written
// --------------------
// ==============================================================================

#include "Chip.h"
#include "GenericTypeDefs.h"

typedef enum
{
    rcERROR,
    rcSUCCESS,
    rcBUSY,

    rcINVALID,
} eRESULT;

//#define API_RS232_PORT      eBOARD_UART_UART0
#define OPS_RS232_PORT      eBOARD_UART_UART1
//#define LH_C8_CONTROL       eBOARD_UART_UART2
#define API_RS232_PORT      eBOARD_UART_UART3

typedef enum
{
    eLICENSE_OPTOMA,
    eLICENSE_CALIBRE,

    eLICENSE_NUMBERS,
} eLICENSE;

#define ULONG_MAX 0xffffffffUL

#endif // PLATFORMDEF_H

