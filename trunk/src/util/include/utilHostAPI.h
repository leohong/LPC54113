#ifndef UTILHOSTAPI_H
#define UTILHOSTAPI_H
// ==============================================================================
// FILE NAME: UTILHOSTAPI.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 21/11/2013, Leo Create
// --------------------
// ==============================================================================

#include "Commondef.h"
#include "utilCLICmdAPI.h"

#define MAX_MSG_PKT     (8)
#define MSG_RETRY       (5)
// #define MAX_PACKET_SIZE (1024+16)
#define MAX_PACKET_SIZE (sizeof(sPAYLOAD)/sizeof(BYTE))
    
#define HEADERSIZE      (sizeof(sMSG_PACKET_HEADER)/sizeof(BYTE))
#define MAGICNUMBERSIZE (2)
#define MAGICNUMBER1    (0x55)
#define MAGICNUMBER2    (0x55)

#define HEAD_PACK_SIZE  (MAGICNUMBERSIZE + HEADERSIZE)

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

typedef enum
{
    eMSG_TYPE_UART,
	eMSG_TYPE_FOTA,

    eMSG_TYPE_NUMBERS,
} eMSG_TYPE;

typedef enum
{
    eDEVICE_APP = 0,
    eDEVICE_BOOTCODE,
    eDEVICE_FOTA,       // RK3399
    eDEVICE_HOST_NUMBERS,
    
    eDEVICE_DOCKING_BOARD = 0,
    eDEVICE_GIM,

    eDEVICE_SLOT_NUMBERS,
} eDEVICE;

typedef enum
{
    ePAYLOAD_TYPE_EXE_WRITE,
    ePAYLOAD_TYPE_EXE_READ,
    ePAYLOAD_TYPE_REPLY,
    ePAYLOAD_TYPE_ACK,

    ePAYLOAD_TYPE_NUMBERS,
} ePAYLOAD_TYPE;

typedef enum
{
    eMSG_STATE_MAGIC_NUMBER,
    eMSG_STATE_PACKET_HEADER,
    eMSG_STATE_PACKET_PAYLOAD,
    eMSG_STATE_DATA_READY,
    eMSG_STATE_BAD_PACKET,
    eMSG_STATE_TIMEOUT,
    eMSG_STATE_RUN_OUT_OF_MEMORY,
    eMSG_STATE_INITIAL_ERROR,

    eMSG_STATE_NUMBERS,
} eMSG_STATE;

typedef struct
{
    BYTE    cSource;
    BYTE    cDestination;
    BYTE    cPacketType;
    WORD    wSeqId;
    WORD    wPacketSize;
    WORD    wChecksum;
} sMSG_PACKET_HEADER;

typedef struct
{
    BYTE                cMagicNumber1;
    BYTE                cMagicNumber2;
    sMSG_PACKET_HEADER  sPacketHeader;

    union
    {
        sPAYLOAD        sPayLoad;
        BYTE            acPacketPayload[sizeof(sPAYLOAD)];
    }uFormat;

} sMSG_PACKET_FORMAT;

typedef eRESULT (*fpWRITE)(WORD wSize, BYTE *pcData);
typedef eRESULT (*fpREAD)(WORD wSize, BYTE *pcData);

typedef struct
{
    sMSG_PACKET_FORMAT  sMsgPacket;
    eMSG_STATE          eMsgParsingState;
    WORD                wRecivedByteCount;
    WORD                wRecivedByteCRC;
    fpWRITE             fpWriteFunc;
    fpREAD              fpReadFunc;

} sMSG_STATE_DATA;

#pragma pack(pop)   /* restore original alignment from stack */

void utilHost_Init(void);
void utilHost_Process(void);
void utilHost_PacketBuild(sMSG_PACKET_FORMAT *psPacket, eDEVICE eSource, eDEVICE eDestination, ePAYLOAD_TYPE ePayloadType, WORD wSeqId, WORD wDataSize, sPAYLOAD *psPayload);
eMSG_STATE utilHost_StateProcess(sMSG_STATE_DATA *psMsgData, DWORD dwMilliSecond);

#endif //UTILHOSTPROCAPI_H


