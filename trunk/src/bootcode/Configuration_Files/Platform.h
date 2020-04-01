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
    eACK_TYPE_NONACK,
    eACK_TYPE_ACK,
    eACK_TYPE_BADPACKET,
    eACK_TYPE_TIMEOUT,
    eACK_TYPE_UNKNOWN,
    eACK_TYPE_FEEDBACK,
    eACK_TYPE_ERROR,
    
    eACK_TYPE_NUMBERS,
} eACK_TYPE;

typedef enum
{
    rcERROR,
    rcSUCCESS,
    rcBUSY,

    rcINVALID,
} eRESULT;

#define API_RS232_PORT      eBOARD_UART_UART3

#endif // PLATFORMDEF_H

