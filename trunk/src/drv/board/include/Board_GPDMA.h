#ifndef BOARD_GPDMA_H
#define BOARD_GPDMA_H
// ==============================================================================
// FILE NAME: Board_GPDMA.h
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2019/11/18, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"

typedef enum
{
    eDMA_CHANNEL_SSP0_TX,
    eDMA_CHANNEL_SSP0_RX,
    eDMA_CHANNEL_SSP1_TX,
    eDMA_CHANNEL_SSP1_RX,
    
    eDMA_CHANNEL_NUMBERS,
} eDMA_CHANNEL;

typedef void (*fpDMACALLBACK)(void);

typedef struct
{
    WORD                    wType;
    WORD                    wChannel;
    GPDMA_FLOW_CONTROL_T    eCtrl;
    fpDMACALLBACK           fpDmaCallback;
    StaticSemaphore_t       *pxSemaphoreBuffer;
    SemaphoreHandle_t       xSemaphore;
} sDMA_CHANNLE_CFG;

void Board_GPDMA_Init(void);
void Board_GPDMA_DeInit(void);
eRESULT Board_GPDMA_Transfer(eDMA_CHANNEL eChannel, BYTE *pcBufer, WORD wSize, WORD wTimeout);
eRESULT Board_GPDMA_Reciver(eDMA_CHANNEL eChannel, BYTE *pcBuffer, WORD wSize, WORD wTimeout);


#endif // BOARD_GPDMA_H
