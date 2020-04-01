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

#define NXP_READ_SIZE           (sizeof(sMSG_PACKET_HEADER)/sizeof(BYTE)) + MAGICNUMBERSIZE
#define NXP_READ_ACK_SIZE       sizeof(sMSG_PACKET_ACK_FORMAT)/sizeof(BYTE)
#define NXP_MAX_RETRY_COUNT     3                                                         //A70_Larry_0328
#define NXP_ACK_DELAY_TIME      (500L)                                                        //A70_Larry_0328    //A70_Larry_0406 15L->5L
#define NXP_READ_SHORT_DELAY    (2L)
#define NXP_READ_LONG_DELAY     (50L)



#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

// Payload
typedef struct
{
    BYTE    cModule;
    BYTE    cSubCmd;
    BYTE    acData[1024 + 32];
} sPAYLOAD;


typedef enum
{
    eNXPACK_TYPE_IDLE,
    eNXPACK_TYPE_ACK,
    eNXPACK_TYPE_BADPACKET,
    eNXPACK_TYPE_TIMEOUT,
    eNXPACK_TYPE_UNKNOWN,
    eNXPACK_TYPE_FEEDBACK,
    eNXPACK_TYPE_ERROR,

    eNXPACK_TYPE_NUMBERS,
} eNXP_ACK_TYPE;


// Cmd table §Î¦¡
typedef struct
{
    WORD wCmdIndex;
    eRESULT(*pFunc)(sPAYLOAD *, sPAYLOAD *, WORD *);
} sCMDCFG;

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

typedef enum
{
    eCMD_SYSTEM_READY,
    eCMD_SYSTEM_VERSION,
    eCMD_SYSTEM_STATUS,
    eCMD_SYSTEM_STATUS_CLEAR,
    eCMD_SYSTEM_HPD_PULL,
    eCMD_SYSTEM_ERROR,
    eCMD_SYSTEM_CARD_PRESENT,
    eCMD_SYSTEM_CARD_POWER,
    eCMD_SYSTEM_SLOT_INFO,
    eCMD_SYSTEM_SLOT_READY,

    eCMD_SYSTEM_NUMBER,
} eCMD_SYSTEM;

#pragma pack(pop)   /* restore original alignment from stack */


eACK_TYPE utilCLICmd_Execute(sPAYLOAD *psInData, sPAYLOAD *psOutData, WORD *pwByteCount, BYTE cRead);


#endif /* UTILCLICMDAPI_H */

