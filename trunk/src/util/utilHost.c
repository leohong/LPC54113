// ==============================================================================
// FILE NAME: UTILHOST.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 12/07/2013, Leo written
// --------------------
// ==============================================================================


#include "Board_Spi.h"
#include "utilHostAPI.h"
#include "utilCounterAPI.h"
#include "utilLogAPI.h"
#include "utilCLICmdAPI.h"
#include "appiapprocAPI.h"

// 解封包狀態機資訊
static sMSG_STATE_DATA      m_sMsgState;     // SM of Packet Type Data

// task stack - start -
static StaticTask_t m_xHostTaskTCB;
static StackType_t m_axHostStack[EVENT_TASK_STACK_SZ];
static TaskHandle_t xHost_Handle = NULL;
// task stack - end -

// ==============================================================================
// FUNCTION NAME: UTILHOST_STATERESET
// DESCRIPTION:
//
//
// Params:
// sMSG_STATE_DATA *psMsgData:
//
// Returns:
//
//
// modification history
// --------------------
// 12/07/2013, Leo written
// --------------------
// ==============================================================================
static void utilHost_StateReset(sMSG_STATE_DATA *psMsgData)
{
    memset(&psMsgData->sMsgPacket, 0, sizeof(sMSG_PACKET_FORMAT) / sizeof(BYTE));
    psMsgData->eMsgParsingState = eMSG_STATE_MAGIC_NUMBER;
    psMsgData->wRecivedByteCount = 0;
    psMsgData->wRecivedByteCRC = 0;
}

// ==============================================================================
// FUNCTION NAME: utilHost_Package_Print
// DESCRIPTION:
//
//
// Params:
// sMSG_STATE_DATA *psMsgData:
//
// Returns:
//
//
// modification history
// --------------------
// 12/07/2013, Leo written
// --------------------
// ==============================================================================
static void utilHost_Package_Print(sMSG_STATE_DATA *psMsgData)
{
    WORD wCount = 0;
    BYTE *pcBuffer = (BYTE *)(&psMsgData->sMsgPacket);

    LOG_MSG(eDB_MSK_LIST_HOST, "\r\n");
    for(wCount = 0; wCount <= (HEAD_PACK_SIZE + psMsgData->sMsgPacket.sPacketHeader.wPacketSize); wCount++)
    {
        LOG_MSG(eDB_MSK_LIST_HOST, "0x%02X ", *(pcBuffer + wCount));
    }

    (void)pcBuffer;

    LOG_MSG(eDB_MSK_LIST_HOST, "\r\n");
}

// ==============================================================================
// FUNCTION NAME: UTILHOST_STATEPROCESS
// DESCRIPTION:
//
// |---------------------------------------------------------------------------|
// |Magic Number|                       Packet Header           |Packet Payload|
// |---------------------------------------------------------------------------|
// |            |Packet Type|Packet Seq |Packet Size|Packet CRC |   Data       |
// |---------------------------------------------------------------------------|
// |  2 bytes   |   1 byte  |   1 byte  |   2 bytes |   2 bytes |   N bytes    |
// |---------------------------------------------------------------------------|
//
// Params:
// sMSG_PACKET_FORMAT *psMsgPacket:

//
// Returns:
//
//
// modification history
// --------------------
// 12/07/2013, Leo written
// --------------------
// ==============================================================================
eMSG_STATE utilHost_StateProcess(sMSG_STATE_DATA *psMsgData, DWORD dwMilliSecond)
{
    BYTE cReadBuffer = 0;

    if(NULL == psMsgData->fpReadFunc)
    {
        vTaskSuspend(NULL);
        return eMSG_STATE_INITIAL_ERROR;
    }

    if(rcSUCCESS == psMsgData->fpReadFunc(1, &cReadBuffer))
    {
        //LOG_MSG(eDB_MSK_LIST_HOST, "0x%02X ", cReadBuffer);

        switch(psMsgData->eMsgParsingState)
        {
            case eMSG_STATE_MAGIC_NUMBER:
            {
                psMsgData->sMsgPacket.cMagicNumber2 = cReadBuffer;

                if((MAGICNUMBER1 == psMsgData->sMsgPacket.cMagicNumber1) && (MAGICNUMBER2 == psMsgData->sMsgPacket.cMagicNumber2))
                {
                    psMsgData->eMsgParsingState = eMSG_STATE_PACKET_HEADER;
                    psMsgData->wRecivedByteCount = 0;
                    psMsgData->wRecivedByteCRC = 0;
                }
                else
                {
                    psMsgData->sMsgPacket.cMagicNumber1 = psMsgData->sMsgPacket.cMagicNumber2;
                }
            }
            break;

            case eMSG_STATE_PACKET_HEADER:
            {
                BYTE *pcBuffer = (BYTE *)&psMsgData->sMsgPacket.sPacketHeader;
                *(pcBuffer + psMsgData->wRecivedByteCount++) = cReadBuffer;

                if(HEADERSIZE == psMsgData->wRecivedByteCount)
                {
                    if(MAX_PACKET_SIZE >= psMsgData->sMsgPacket.sPacketHeader.wPacketSize)
                    {
                        psMsgData->wRecivedByteCount = 0;
                        psMsgData->eMsgParsingState = eMSG_STATE_PACKET_PAYLOAD;

                        // 計算CRC
                        psMsgData->wRecivedByteCRC += psMsgData->sMsgPacket.sPacketHeader.cSource;
                        psMsgData->wRecivedByteCRC += psMsgData->sMsgPacket.sPacketHeader.cDestination;
                        psMsgData->wRecivedByteCRC += psMsgData->sMsgPacket.sPacketHeader.cPacketType;
                        psMsgData->wRecivedByteCRC += ((psMsgData->sMsgPacket.sPacketHeader.wSeqId >> 8) & 0x00FF);
                        psMsgData->wRecivedByteCRC += (psMsgData->sMsgPacket.sPacketHeader.wSeqId & 0x00FF);
                        psMsgData->wRecivedByteCRC += ((psMsgData->sMsgPacket.sPacketHeader.wPacketSize >> 8) & 0x00FF) ;
                        psMsgData->wRecivedByteCRC += (psMsgData->sMsgPacket.sPacketHeader.wPacketSize & 0x00FF);
                        // 不帶Payload資料,檢查CRC
                        if(0 == psMsgData->sMsgPacket.sPacketHeader.wPacketSize)
                        {
                            if(0 == (0xFFFF & (psMsgData->sMsgPacket.sPacketHeader.wChecksum +
                                               psMsgData->wRecivedByteCRC)))
                            {
                                psMsgData->eMsgParsingState = eMSG_STATE_DATA_READY;
                            }
                            else
                            {
                                LOG_MSG(eDB_MSK_LIST_HOST, "CRC ERROR\r\n");
                                utilHost_Package_Print(psMsgData);
                                psMsgData->eMsgParsingState = eMSG_STATE_BAD_PACKET;
                            }

                            return psMsgData->eMsgParsingState;
                        }
                    }
                    else
                    {
                        LOG_MSG(eDB_MSK_LIST_HOST, "OVER SIZE\r\n");
                        //utilHost_Package_Print(psMsgData);
                        psMsgData->eMsgParsingState = eMSG_STATE_BAD_PACKET;
                        return eMSG_STATE_BAD_PACKET;
                    }
                }
            }
            break;

            case eMSG_STATE_PACKET_PAYLOAD:
            {
                psMsgData->wRecivedByteCRC += cReadBuffer;
                psMsgData->sMsgPacket.uFormat.acPacketPayload[psMsgData->wRecivedByteCount++] = cReadBuffer;

                if(psMsgData->sMsgPacket.sPacketHeader.wPacketSize == psMsgData->wRecivedByteCount)
                {
                    if(0 == (0xFFFF & (psMsgData->sMsgPacket.sPacketHeader.wChecksum + psMsgData->wRecivedByteCRC)))
                    {
                        psMsgData->eMsgParsingState = eMSG_STATE_DATA_READY;
                        LOG_MSG(eDB_MSK_LIST_HOST, "\r\n");
                    }
                    else
                    {
                        LOG_MSG(eDB_MSK_LIST_HOST, "CRC ERROR\r\n");
                        utilHost_Package_Print(psMsgData);
                        psMsgData->eMsgParsingState = eMSG_STATE_BAD_PACKET;
                    }

                    return psMsgData->eMsgParsingState;
                }
            }
            break;

            default:
                return psMsgData->eMsgParsingState;
                //break;
        }
    }

    return psMsgData->eMsgParsingState;
}

// ==============================================================================
// FUNCTION NAME: UTILHOST_CHECKSUMCALC
// DESCRIPTION:
//
//
// Params:
// sMSG_PACKET_FORMAT *psPacket:
// WORD wSize:
// BYTE *pcBuffer:
//
// Returns:
//
//
// modification history
// --------------------
// 15/07/2013, Leo written
// --------------------
// ==============================================================================
static void utilHost_CheckSumCalc(sMSG_PACKET_FORMAT *psPacket)
{
    WORD wCount = 0;
    WORD wChecksum = 0;

    wChecksum = 0;
    wChecksum += psPacket->sPacketHeader.cSource;
    wChecksum += psPacket->sPacketHeader.cDestination;
    wChecksum += psPacket->sPacketHeader.cPacketType;
    wChecksum += ((psPacket->sPacketHeader.wSeqId >> 8) & 0x00FF);
    wChecksum += (psPacket->sPacketHeader.wSeqId & 0x00FF);
    wChecksum += ((psPacket->sPacketHeader.wPacketSize >> 8) & 0x00FF) ;
    wChecksum += (psPacket->sPacketHeader.wPacketSize & 0x00FF);

    for(wCount = 0; wCount < psPacket->sPacketHeader.wPacketSize; wCount++)
    {
        wChecksum += psPacket->uFormat.acPacketPayload[wCount];
    }

    psPacket->sPacketHeader.wChecksum = ((~wChecksum + 1) & 0xFFFF);
}

// ==============================================================================
// FUNCTION NAME: utilHost_Ack_Build
// DESCRIPTION:
//
// |---------------------------------------------------------------------------|
// |Magic Number|                       Packet Header           |   ACK        |
// |---------------------------------------------------------------------------|
// |            |Packet Type|Packet Seq |Packet Size|Packet CRC |   Data       |
// |---------------------------------------------------------------------------|
// |  2 bytes   |   1 byte  |   1 byte  |   2 bytes |   2 bytes |   1 bytes    |
// |---------------------------------------------------------------------------|
//
// Ack 封包大小 = Header Size + 1 Byte Ack
//
// Params:
// eMSG_TYPE eMsgType:
// eACK_TYPE eAckType:
//
// Returns:
//
//
// modification history
// --------------------
// 15/07/2013, Leo written
// --------------------
// ==============================================================================
static void utilHost_Ack_Build(sMSG_PACKET_FORMAT *psPacket, eDEVICE eSource, eDEVICE eDestination, eACK_TYPE eAckType)
{
    psPacket->cMagicNumber1 = MAGICNUMBER1;
    psPacket->cMagicNumber2 = MAGICNUMBER2;
    psPacket->sPacketHeader.cSource = eSource;
    psPacket->sPacketHeader.cDestination = eDestination;
    psPacket->sPacketHeader.cPacketType = ePAYLOAD_TYPE_ACK;
    psPacket->sPacketHeader.wPacketSize = 1;
    psPacket->uFormat.acPacketPayload[0] = eAckType;
    utilHost_CheckSumCalc(psPacket);
}

// ==============================================================================
// FUNCTION NAME: UTILHOST_CMD_EXCUTE
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/30, Leo Create
// --------------------
// ==============================================================================
static void utilHost_Cmd_Execute(sMSG_STATE_DATA *psMsgData)
{
    BYTE cRead = TRUE;
    sMSG_PACKET_FORMAT sOutPacket;
    eACK_TYPE eAckType = eACK_TYPE_NUMBERS;

    // Copy input data to sOutPacket
    sOutPacket = psMsgData->sMsgPacket;

    if(ePAYLOAD_TYPE_EXE_WRITE == psMsgData->sMsgPacket.sPacketHeader.cPacketType)
    {
        cRead = FALSE;
    }

    eAckType = utilCLICmd_Execute((sPAYLOAD *)&psMsgData->sMsgPacket.uFormat.acPacketPayload,
                                  (sPAYLOAD *)&sOutPacket.uFormat.acPacketPayload,
                                  (WORD *)&sOutPacket.sPacketHeader.wPacketSize,
                                  cRead);

    //Swap Source and Destination
    sOutPacket.sPacketHeader.cDestination = psMsgData->sMsgPacket.sPacketHeader.cSource;
    sOutPacket.sPacketHeader.cSource = psMsgData->sMsgPacket.sPacketHeader.cDestination;

    if((eAckType == eACK_TYPE_FEEDBACK)&&(NULL != psMsgData->fpWriteFunc))
    {
        sOutPacket.sPacketHeader.cPacketType = ePAYLOAD_TYPE_REPLY;

        utilHost_CheckSumCalc(&sOutPacket);

        psMsgData->fpWriteFunc((sOutPacket.sPacketHeader.wPacketSize + HEADERSIZE + MAGICNUMBERSIZE), (BYTE *)&sOutPacket);

#if 0
        {
            WORD wCount = 0;
            LOG_MSG(eDB_MSK_LIST_HOST, "CRC=0x%04x, Size = %d,(",
                    sOutPacket.sPacketHeader.wChecksum,
                    sOutPacket.sPacketHeader.wPacketSize);

            for(wCount = 0; wCount < sOutPacket.sPacketHeader.wPacketSize; wCount++)
            {
                LOG_MSG(eDB_MSK_LIST_HOST, "%x,", sOutPacket.uFormat.acPacketPayload[wCount]);
            }

            LOG_MSG(eDB_MSK_LIST_HOST, ")\r\n");
        }
#endif // 0
    }
    else
    {
#if 0
         {
            WORD wCount = 0;
            LOG_MSG(eDB_MSK_LIST_HOST, "Main = %d, Sub = %d, Size = %d,(",
                    sOutPacket.uFormat.sPayLoad.cModule,
                    sOutPacket.uFormat.sPayLoad.cSubCmd,
                    sOutPacket.sPacketHeader.wPacketSize);

            for(wCount = 0; wCount < sOutPacket.sPacketHeader.wPacketSize; wCount++)
            {
                LOG_MSG(eDB_MSK_LIST_HOST, "%x, ", sOutPacket.uFormat.sPayLoad.acData[wCount]);
            }

            LOG_MSG(eDB_MSK_LIST_HOST, ")\r\n");
        }
#endif // 0

        utilHost_Ack_Build(&psMsgData->sMsgPacket, (eDEVICE)psMsgData->sMsgPacket.sPacketHeader.cDestination, (eDEVICE)psMsgData->sMsgPacket.sPacketHeader.cSource, eAckType);

        if(NULL != psMsgData->fpWriteFunc)
        {
            psMsgData->fpWriteFunc((psMsgData->sMsgPacket.sPacketHeader.wPacketSize + HEADERSIZE + MAGICNUMBERSIZE), (BYTE *)&psMsgData->sMsgPacket);
        }
    }
}

// ==============================================================================
// FUNCTION NAME: utilHost_Packet_UnPack
// DESCRIPTION:
//
//
// Params:
// sMSG_STATE_DATA *psMsgData:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/30, Leo Create
// --------------------
// ==============================================================================
static void utilHost_Packet_UnPack(sMSG_STATE_DATA *psMsgData)
{
    eMSG_STATE eMsgParsingState = eMSG_STATE_MAGIC_NUMBER;

    eMsgParsingState = utilHost_StateProcess(psMsgData, ONE_SECOND);

    switch(eMsgParsingState)
    {
        case eMSG_STATE_MAGIC_NUMBER:
        case eMSG_STATE_PACKET_HEADER:
        case eMSG_STATE_PACKET_PAYLOAD:
            // Packet unpacking
            break;

        case eMSG_STATE_DATA_READY:
        {
            utilHost_Cmd_Execute(psMsgData);

            // Restart parsing process
            utilHost_StateReset(psMsgData);
            taskYIELD();
        }
        break;

        case eMSG_STATE_BAD_PACKET:
            // Build Ack and push in output ring buffer
            LOG_MSG(eDB_MSK_LIST_HOST, "BAD_PACKET\r\n");

            utilHost_Ack_Build(&psMsgData->sMsgPacket, (eDEVICE)psMsgData->sMsgPacket.sPacketHeader.cDestination, (eDEVICE)psMsgData->sMsgPacket.sPacketHeader.cSource, eACK_TYPE_BADPACKET);
            psMsgData->fpWriteFunc((psMsgData->sMsgPacket.sPacketHeader.wPacketSize + HEADERSIZE + MAGICNUMBERSIZE), (BYTE *)&psMsgData->sMsgPacket);
            utilHost_StateReset(psMsgData);
            taskYIELD();
            break;

        case eMSG_STATE_TIMEOUT:
            LOG_MSG(eDB_MSK_LIST_HOST, "TIMEOUT\r\n");
            // Build Ack and push in output ring buffer
            utilHost_Ack_Build(&psMsgData->sMsgPacket, (eDEVICE)psMsgData->sMsgPacket.sPacketHeader.cDestination, (eDEVICE)psMsgData->sMsgPacket.sPacketHeader.cSource, eACK_TYPE_TIMEOUT);
            psMsgData->fpWriteFunc((psMsgData->sMsgPacket.sPacketHeader.wPacketSize + HEADERSIZE + MAGICNUMBERSIZE), (BYTE *)&psMsgData->sMsgPacket);
            utilHost_StateReset(psMsgData);
            taskYIELD();
            break;

        case eMSG_STATE_RUN_OUT_OF_MEMORY:
            LOG_MSG(eDB_MSK_LIST_HOST, "OUT OF MEMORY\r\n");
            // Build Ack and push in output ring buffer
            utilHost_Ack_Build(&psMsgData->sMsgPacket, (eDEVICE)psMsgData->sMsgPacket.sPacketHeader.cDestination, (eDEVICE)psMsgData->sMsgPacket.sPacketHeader.cSource, eACK_TYPE_BADPACKET);
            psMsgData->fpWriteFunc((psMsgData->sMsgPacket.sPacketHeader.wPacketSize + HEADERSIZE + MAGICNUMBERSIZE), (BYTE *)&psMsgData->sMsgPacket);
            utilHost_StateReset(psMsgData);
            taskYIELD();
            break;

        default:
            break;
    }
}

// ==============================================================================
// FUNCTION NAME: utilHost_PacketBuild
// DESCRIPTION:
//
//
// Params:
// sMSG_PACKET_FORMAT *psPacket:
// eDEVICE eSource:
// eDEVICE eDestination:
// ePAYLOAD_TYPE ePayloadType:
// WORD wSeqId:
// WORD wDataSize:
// sPAYLOAD *psPayload:
//
// Returns:
//
//
// modification history
// --------------------
// 12/07/2013, Leo written
// --------------------
// ==============================================================================
void utilHost_PacketBuild(sMSG_PACKET_FORMAT *psPacket, eDEVICE eSource, eDEVICE eDestination, ePAYLOAD_TYPE ePayloadType, WORD wSeqId, WORD wDataSize, sPAYLOAD *psPayload)
{
    psPacket->cMagicNumber1 = MAGICNUMBER1;
    psPacket->cMagicNumber2 = MAGICNUMBER2;
    psPacket->sPacketHeader.cSource = eSource;
    psPacket->sPacketHeader.cDestination = eDestination;
    psPacket->sPacketHeader.cPacketType = ePayloadType;
    psPacket->sPacketHeader.wSeqId = wSeqId;
    psPacket->sPacketHeader.wPacketSize = wDataSize;
    psPacket->uFormat.sPayLoad = *psPayload;

    utilHost_CheckSumCalc(psPacket);
}

// ==============================================================================
// FUNCTION NAME: utilHost_Device_Write
// DESCRIPTION:
//
//
// Params:
// WORD wSize:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
// 12/07/2013, Leo written
// --------------------
// ==============================================================================
static eRESULT utilHost_Device_Write(WORD wSize, BYTE *pcData)
{
    return Board_SSP_Slave_Write(eBOARD_SSP_SSP1, wSize, pcData);
}

// ==============================================================================
// FUNCTION NAME: utilHost_Device_Read
// DESCRIPTION:
//
//
// Params:
// WORD wSize:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
// 12/07/2013, Leo written
// --------------------
// ==============================================================================
static eRESULT utilHost_Device_Read(WORD wSize, BYTE *pcData)
{
    return Board_SSP_Slave_Read(eBOARD_SSP_SSP1, wSize, pcData);
}

// ==============================================================================
// FUNCTION NAME: UTILHOST_PROCESS
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 12/07/2013, Leo written
// --------------------
// ==============================================================================
void utilHostHandlerTask(void *pParameters)
{
    while(1)
    {
        utilHost_Packet_UnPack(&m_sMsgState);
    }
}

// ==============================================================================
// FUNCTION NAME: UTILHOST_INIT
// DESCRIPTION:
//
// 初始化Host 介面
//
// Params:
// eMSG_TYPE eMsgType:
//
// Returns:
//
//
// modification history
// --------------------
// 12/07/2013, Leo written
// --------------------
// ==============================================================================
void utilHost_Init(void)
{
    utilHost_StateReset(&m_sMsgState);
    m_sMsgState.fpWriteFunc = utilHost_Device_Write;
    m_sMsgState.fpReadFunc = utilHost_Device_Read;

    /* Create the task without using any dynamic memory allocation. */
    xHost_Handle = xTaskCreateStatic(   utilHostHandlerTask,       /* Function that implements the task. */
                                        "Host_Handler_Task",       /* Text name for the task. */
                                        HOST_TASK_STACK_SZ,        /* Number of indexes in the xStack array. */
                                        NULL,                       /* Parameter passed into the task. */
                                        HOST_TASK_PRIORITY,        /* Priority at which the task is created. */
                                        m_axHostStack,             /* Array to use as the task's stack. */
                                        &m_xHostTaskTCB);          /* Variable to hold the task's data structure. */

    if(NULL == xHost_Handle)
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "Host Handler Task Create Fail\r\n");
    }
}



