// ===============================================================================
// FILE NAME: dvCard.c
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2019/10/30, Leo Create
// --------------------
// ===============================================================================


#include "Commondef.h"
#include "Board_Spi.h"
#include "dvCard.h"
#include "utilHostAPI.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"

#define TIME_OUT        (10)
#define RETRY_DELAY     (10)
#define SLEEP_BTWN_RW   (2)
#define RETRY_COUNT     (5)

static WORD m_wSeqId = 0;
StaticSemaphore_t m_axCardSemaphoreBuffer;
SemaphoreHandle_t m_xCardSemaphore;

static sMSG_STATE_DATA  m_sCardState;

// ==============================================================================
// FUNCTION NAME: dvCard_Device_Read
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
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
static eRESULT dvCard_Device_Write(WORD wSize, BYTE *pcData)
{
    return Board_SSP_Master_Write(eBOARD_SSP_SSP0, wSize, pcData);
}

// ==============================================================================
// FUNCTION NAME: dvCard_Device_Read
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
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
static eRESULT dvCard_Device_Read(WORD wSize, BYTE *pcData)
{
    return Board_SSP_Master_Read(eBOARD_SSP_SSP0, wSize, pcData);
}

// ==============================================================================
// FUNCTION NAME: dvCard_State_Reset
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
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
static void dvCard_State_Reset(sMSG_STATE_DATA *psMsgData)
{
    memset(&psMsgData->sMsgPacket, 0, sizeof(sMSG_PACKET_FORMAT) / sizeof(BYTE));
    psMsgData->eMsgParsingState = eMSG_STATE_MAGIC_NUMBER;
    psMsgData->wRecivedByteCount = 0;
    psMsgData->wRecivedByteCRC = 0;
}

// ==============================================================================
// FUNCTION NAME: dvCard_Initial
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
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
void dvCard_Initial(void)
{
    m_xCardSemaphore = xSemaphoreCreateMutexStatic(&m_axCardSemaphoreBuffer);

    dvCard_State_Reset(&m_sCardState);

    m_sCardState.fpWriteFunc = dvCard_Device_Write;
    m_sCardState.fpReadFunc = dvCard_Device_Read;
}

// ==============================================================================
// FUNCTION NAME: dvCard_Command_Write
// DESCRIPTION:
//
//
// Params:
// eCARD_CMD eCmd:
// WORD wSize:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
static eRESULT dvCard_Command_Write(eCMD_MODULE eModule, BYTE cSubCmd, WORD wSize, BYTE *pcData)
{
    eRESULT eResult = rcERROR;
    BYTE cRetry = RETRY_COUNT;
    sPAYLOAD sPayload;
    WORD wOutputSize = 0;

    if(NULL != m_xCardSemaphore)
    {
        if(pdPASS == xSemaphoreTake(m_xCardSemaphore, portMAX_DELAY))
        {
            do
            {
                wOutputSize = wSize;
                dvCard_State_Reset(&m_sCardState);

                sPayload.cModule = eModule;
                wOutputSize++;
                sPayload.cSubCmd = cSubCmd;
                wOutputSize++;

                if(0 != wSize)
                {
                    memcpy(sPayload.acData, pcData, wSize);
                }

                utilHost_PacketBuild(&m_sCardState.sMsgPacket, eDEVICE_DOCKING_BOARD, eDEVICE_GIM, ePAYLOAD_TYPE_EXE_WRITE, m_wSeqId, wOutputSize, &sPayload);

                if(NULL != m_sCardState.fpWriteFunc)
                {
                    m_sCardState.fpWriteFunc((HEAD_PACK_SIZE + m_sCardState.sMsgPacket.sPacketHeader.wPacketSize), (BYTE *)&m_sCardState.sMsgPacket);
                }

                utilDelayMs(SLEEP_BTWN_RW);
                dvCard_State_Reset(&m_sCardState);

                utilCounterSet(eCOUNTER_TYPE_CARD_WRITE, TIME_OUT);

                while(eMSG_STATE_DATA_READY > m_sCardState.eMsgParsingState)
                {
                    utilHost_StateProcess(&m_sCardState, 100);

                    if(eMSG_STATE_DATA_READY == m_sCardState.eMsgParsingState)
                    {
                        if(m_wSeqId == m_sCardState.sMsgPacket.sPacketHeader.wSeqId)
                        {
                            if((ePAYLOAD_TYPE_ACK == m_sCardState.sMsgPacket.sPacketHeader.cPacketType)
                                && (eACK_TYPE_ACK == m_sCardState.sMsgPacket.uFormat.acPacketPayload[0]))
                            {
                                eResult = rcSUCCESS;
                            }
                        }
                        else
                        {
                            utilCounterSet(eCOUNTER_TYPE_CARD_WRITE, TIME_OUT);
                            dvCard_State_Reset(&m_sCardState);
                        }
                    }

                    if(0 == utilCounterGet(eCOUNTER_TYPE_CARD_WRITE))
                    {
                        m_sCardState.eMsgParsingState = eMSG_STATE_TIMEOUT;
                        break;
                    }
                }

                switch(m_sCardState.eMsgParsingState)
                {
                    case eMSG_STATE_BAD_PACKET:
                    case eMSG_STATE_TIMEOUT:
                    case eMSG_STATE_RUN_OUT_OF_MEMORY:
                    case eMSG_STATE_INITIAL_ERROR:
                        utilDelayMs(RETRY_DELAY);
                        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "\r\nWrite retry\r\n");
                        eResult = rcERROR;
                        break;

                    default:
                        break;
                }

                m_wSeqId++;
            }
            while((cRetry--) && (eResult == rcERROR));

            xSemaphoreGive(m_xCardSemaphore);
        }
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_Command_Read
// DESCRIPTION:
//
//
// Params:
// eCARD_CMD eCmd:
// WORD wSize:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
static eRESULT dvCard_Command_Read(eCMD_MODULE eModule, BYTE cSubCmd, WORD wSize, BYTE *pcData)
{
    eRESULT eResult = rcERROR;
    sPAYLOAD sPayload;
    BYTE cRetry = RETRY_COUNT;
    WORD wOutputSize = 0;

    if(NULL != m_xCardSemaphore)
    {
        if(pdPASS == xSemaphoreTake(m_xCardSemaphore, portMAX_DELAY))
        {
            do
            {
                wOutputSize = 0;
                dvCard_State_Reset(&m_sCardState);

                sPayload.cModule = eModule;
                wOutputSize++;
                sPayload.cSubCmd = cSubCmd;
                wOutputSize++;

                if(0 != wSize)
                {
                    memcpy(sPayload.acData, pcData, wSize);
                }

                utilHost_PacketBuild(&m_sCardState.sMsgPacket, eDEVICE_DOCKING_BOARD, eDEVICE_GIM, ePAYLOAD_TYPE_EXE_READ, m_wSeqId, wOutputSize, &sPayload);

                if(NULL != m_sCardState.fpWriteFunc)
                {
                    m_sCardState.fpWriteFunc((HEAD_PACK_SIZE + m_sCardState.sMsgPacket.sPacketHeader.wPacketSize), (BYTE *)&m_sCardState.sMsgPacket);
                }

                utilDelayMs(SLEEP_BTWN_RW);
                dvCard_State_Reset(&m_sCardState);

                utilCounterSet(eCOUNTER_TYPE_CARD_READ, TIME_OUT);

                while(eMSG_STATE_DATA_READY > m_sCardState.eMsgParsingState)
                {
                    utilHost_StateProcess(&m_sCardState, 100);

                    if(eMSG_STATE_DATA_READY == m_sCardState.eMsgParsingState)
                    {
                        if(m_wSeqId == m_sCardState.sMsgPacket.sPacketHeader.wSeqId)
                        {
                            if(ePAYLOAD_TYPE_REPLY == m_sCardState.sMsgPacket.sPacketHeader.cPacketType)
                            {
                                memcpy(pcData, m_sCardState.sMsgPacket.uFormat.acPacketPayload, m_sCardState.sMsgPacket.sPacketHeader.wPacketSize);
                                eResult = rcSUCCESS;
                            }
                        }
                        else
                        {
                            utilCounterSet(eCOUNTER_TYPE_CARD_READ, TIME_OUT);
                            dvCard_State_Reset(&m_sCardState);
                        }
                    }

                    if(0 == utilCounterGet(eCOUNTER_TYPE_CARD_READ))
                    {
                        m_sCardState.eMsgParsingState = eMSG_STATE_TIMEOUT;
                        break;
                    }
                }

                switch(m_sCardState.eMsgParsingState)
                {
                    case eMSG_STATE_BAD_PACKET:
                    case eMSG_STATE_TIMEOUT:
                    case eMSG_STATE_RUN_OUT_OF_MEMORY:
                    case eMSG_STATE_INITIAL_ERROR:
                        utilDelayMs(RETRY_DELAY);
                        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "\r\nRead retry\r\n");
                        eResult = rcERROR;
                        break;

                    default:
                        break;
                }

                m_wSeqId++;
            }
            while((cRetry--) && (eResult == rcERROR));

            xSemaphoreGive(m_xCardSemaphore);
        }
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_System_Ready_Get
// DESCRIPTION:
//
//
// Params:
// BYTE *pcReady:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/26, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_System_Ready_Get(BYTE *pcReady)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Read(eCMD_MODULE_SYSTEM, eCMD_SYSTEM_READY, 0, pcReady);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Card System Ready %d\r\n", *pcReady);
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "System Not Ready\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_System_Card_Present_Get
// DESCRIPTION:
//
//
// Params:
// WORD *pwPresent:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/26, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_System_Card_Present_Get(WORD *pwPresent)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Read(eCMD_MODULE_SYSTEM, eCMD_SYSTEM_CARD_PRESENT, 0, (BYTE *)pwPresent);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Card Present 0x%02X\r\n", *pwPresent);
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Read Card Present Error\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_System_Card_Power_Set
// DESCRIPTION:
//
//
// Params:
// WORD *pwPresent:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/26, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_System_Card_Power_Set(WORD wEnable)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Write(eCMD_MODULE_SYSTEM, eCMD_SYSTEM_CARD_POWER, 0, (BYTE *)&wEnable);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Card Power Pass\r\n");
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Card Power Error\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_System_Card_Power_Get
// DESCRIPTION:
//
//
// Params:
// WORD *pwPresent:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/26, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_System_Card_Power_Get(WORD *pwEnable)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Read(eCMD_MODULE_SYSTEM, eCMD_SYSTEM_CARD_POWER, 0, (BYTE *)pwEnable);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Card Power 0x%02X\r\n", *pwEnable);
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Card Power Error\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_System_Revision_Get
// DESCRIPTION:
//
//
// Params:
// eSOURCE_PORT ePort:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_System_Revision_Get(BYTE *pcVersion)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Read(eCMD_MODULE_SYSTEM, eCMD_SYSTEM_VERSION, 0, pcVersion);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Version %d,%d,%d,%d\r\n", pcVersion[0], pcVersion[1], pcVersion[2], pcVersion[3]);
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Read Version Error\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_Input_Port_Set
// DESCRIPTION:
//
//
// Params:
// eSOURCE_PORT ePort:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_Input_Port_Set(eSOURCE_PORT ePort)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Write(eCMD_MODULE_SOURCE_SWITCH, eCMD_SOURCE_PORT_SWITCH, 1, (BYTE *)&ePort);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Switch Pass\r\n");
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Switch Fail\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_Video_Info_Get
// DESCRIPTION:
//
//
// Params:
// sVIDEO_INFO *psVideoInfo:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_Video_Info_Get(sVIDEO_INFO *psVideoInfo)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Read(eCMD_MODULE_SOURCE_VIDEO, eCMD_SOURCE_VIDEO_INFO, 0, (BYTE *)psVideoInfo);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Info Read Pass\r\n");
#if 1
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Stable     = %d\r\n", psVideoInfo->cStable);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "PCLK       = %d\r\n", psVideoInfo->sTimingInfo.dwPixClock);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HTotal     = %d\r\n", psVideoInfo->sTimingInfo.wHTol);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HDEW       = %d\r\n", psVideoInfo->sTimingInfo.wHAct);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HFPH       = %d\r\n", psVideoInfo->sTimingInfo.wHFront);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HSyncW     = %d\r\n", psVideoInfo->sTimingInfo.wHSync);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HBPH       = %d\r\n", psVideoInfo->sTimingInfo.wHBack);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HSyncPol   = %d\r\n", psVideoInfo->sTimingInfo.cHPol);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VTotal     = %d\r\n", psVideoInfo->sTimingInfo.wVTol);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VDEW       = %d\r\n", psVideoInfo->sTimingInfo.wVAct);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VFPH       = %d\r\n", psVideoInfo->sTimingInfo.wVFront);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VSyncW     = %d\r\n", psVideoInfo->sTimingInfo.wVSync);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VBPH       = %d\r\n", psVideoInfo->sTimingInfo.wVBack);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VSyncPol   = %d\r\n", psVideoInfo->sTimingInfo.cVPol);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "InterLaced = %d\r\n", psVideoInfo->sTimingInfo.cScanType);

        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Asp Ratio   = %d\r\n", psVideoInfo->sFormatInfo.eAspectRatio);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "ColorDepth  = %d\r\n", psVideoInfo->sFormatInfo.eColorDepth);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "ColorRange  = %d\r\n", psVideoInfo->sFormatInfo.eColorRange);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "ColorSpace  = %d\r\n", psVideoInfo->sFormatInfo.eColorSpace);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HDCP Ver    = %d\r\n", psVideoInfo->sFormatInfo.cHdcpVersion);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HDMI Mode   = %d\r\n", psVideoInfo->sFormatInfo.cHdmiMode);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Scramble    = %d\r\n", psVideoInfo->sFormatInfo.cScrambling);
#endif // 0
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Info Read Fail\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_Video_Timing_Get
// DESCRIPTION:
//
//
// Params:
// sVIDEO_TIMING *psVideoTiming:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/19, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_Video_Timing_Get(sVIDEO_TIMING *psVideoTiming)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Read(eCMD_MODULE_SOURCE_VIDEO, eCMD_SOURCE_VIDEO_TIMING, 0, (BYTE *)psVideoTiming);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Timing Pass\r\n");
#if 0
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "PCLK       = %d\r\n", psVideoTiming->dwPixClock);

        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HTotal     = %d\r\n", psVideoTiming->wHTol);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HDEW       = %d\r\n", psVideoTiming->wHAct);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HFPH       = %d\r\n", psVideoTiming->wHFront);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HSyncW     = %d\r\n", psVideoTiming->wHSync);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HBPH       = %d\r\n", psVideoTiming->wHBack);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HSyncPol   = %d\r\n", psVideoTiming->cHPol);

        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VTotal     = %d\r\n", psVideoTiming->wVTol);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VDEW       = %d\r\n", psVideoTiming->wVAct);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VFPH       = %d\r\n", psVideoTiming->wVFront);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VSyncW     = %d\r\n", psVideoTiming->wVSync);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VBPH       = %d\r\n", psVideoTiming->wVBack);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "VSyncPol   = %d\r\n", psVideoTiming->cVPol);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "InterLaced = %d\r\n", psVideoTiming->cScanType);
#endif // 0
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Timing Fail\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_Video_Format_Get
// DESCRIPTION:
//
//
// Params:
// sVIDEO_FORMAT *psVideoFormat:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/20, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_Video_Format_Get(sVIDEO_FORMAT *psVideoFormat)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Read(eCMD_MODULE_SOURCE_VIDEO, eCMD_SOURCE_VIDEO_FORMAT, 0, (BYTE *)psVideoFormat);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Format Pass\r\n");
#if 1
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Asp Ratio   = %d\r\n", psVideoFormat->eAspectRatio);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "ColorDepth  = %d\r\n", psVideoFormat->eColorDepth);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "ColorRange  = %d\r\n", psVideoFormat->eColorRange);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "ColorSpace  = %d\r\n", psVideoFormat->eColorSpace);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HDCP Ver    = %d\r\n", psVideoFormat->cHdcpVersion);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "HDMI Mode   = %d\r\n", psVideoFormat->cHdmiMode);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Scramble    = %d\r\n", psVideoFormat->cScrambling);
#endif // 0
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Format Fail\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_Audio_Info_Get
// DESCRIPTION:
//
//
// Params:
// sAUDIO_INFO *psAudioInfo:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/10/30, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_Audio_Info_Get(sAUDIO_INFO *psAudioInfo)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Read(eCMD_MODULE_SOURCE_AUDIO, eCMD_SOURCE_AUDIO_INFO, 0, (BYTE *)psAudioInfo);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Audio Pass\r\n");
#if 0
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Audio      = %d\r\n", psAudioInfo->cIsAudio);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Type       = %d\r\n", psAudioInfo->cAudioType);

        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "ChCnt      = %d\r\n", psAudioInfo->cChannelCount);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Freq       = %d\r\n", psAudioInfo->cSampleFreq);
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Size       = %d\r\n", psAudioInfo->cSampleSize);
#endif //0
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Audio Fail\r\n");
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: dvCard_Slot_Info_Get
// DESCRIPTION:
//
//
// Params:
// sCARD_INFO *psCardInfo:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/12/10, Leo Create
// --------------------
// ==============================================================================
eRESULT dvCard_Slot_Info_Get(sSLOT_INFO *psSlotInfo)
{
    eRESULT eResult = rcERROR;

    eResult = dvCard_Command_Read(eCMD_MODULE_SYSTEM, eCMD_SYSTEM_SLOT_INFO, 0, (BYTE *)psSlotInfo);

    if(rcSUCCESS == eResult)
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Slot Info %d, %s\r\n", psSlotInfo->cInputs, psSlotInfo->acInputName);
    }
    else
    {
        LOG_MSG(eDB_MSK_LIST_VIDEO_SWITCHER, "Read Slot Info Error\r\n");
    }

    return eResult;
}


