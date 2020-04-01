#ifndef UTILCLICMDAPI_H
#define UTILCLICMDAPI_H
// ==============================================================================
// FILE NAME: UTILCLICMDAPI.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 18/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"

#define CMD_READ    1
#define CMD_WRITE   0

#define MODULE_LEN  1
#define CMD_LEN     1

#define NXP_READ_SIZE           ((sizeof(sMSG_PACKET_HEADER)/sizeof(BYTE)) + MAGICNUMBERSIZE)
#define NXP_READ_ACK_SIZE       (sizeof(sMSG_PACKET_ACK_FORMAT)/sizeof(BYTE))
#define NXP_MAX_RETRY_COUNT     (3)
#define NXP_ACK_DELAY_TIME      (500L)
#define NXP_READ_SHORT_DELAY    (2L)
#define NXP_READ_LONG_DELAY     (50L)

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

// Payload
typedef struct
{
    BYTE    cModule;
    BYTE    cSubCmd;
    BYTE    acData[64 + 32];
} sPAYLOAD;

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

// Cmd table §Î¦¡
typedef struct
{
    WORD wCmdIndex;
    eRESULT(*pFunc)(sPAYLOAD *, sPAYLOAD *, WORD *);
} sCMDCFG;

#pragma pack(pop)   /* restore original alignment from stack */

typedef enum
{
    eCMD_MODULE_SYSTEM,
    eCMD_MODULE_SOURCE_SWITCH,
    eCMD_MODULE_SOURCE_VIDEO,
    eCMD_MODULE_SOURCE_AUDIO,
    eCMD_MODULE_SOURCE_EDID,
    eCMD_MODULE_SOURCE_SDI,
    eCMD_MODULE_IAP,
    eCMD_MODULE_NUMBER,
} eCMD_MODULE;

eACK_TYPE utilCLICmd_Execute(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount, BYTE cRead);
BOOL utilCLICmd_PreExecuteCheck(sPAYLOAD *psInData);

#endif /* UTILCLICMDAPI_H */

