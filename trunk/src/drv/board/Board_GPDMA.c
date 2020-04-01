// ==============================================================================
// FILE NAME: Board_GPDMA.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 2019/11/18, Leo Create
// --------------------
// ==============================================================================


#include "Board_GPDMA.h"

StaticSemaphore_t m_axDmaSemaphoreBuffer[eDMA_CHANNEL_NUMBERS];

static sDMA_CHANNLE_CFG m_asDmaChanCgf[eDMA_CHANNEL_NUMBERS] = 
{
    {
        GPDMA_CONN_SSP0_Tx,                             // wType
        0,                                              // wChannel
        GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
        NULL,                                           // fpDmaCallback
        &m_axDmaSemaphoreBuffer[eDMA_CHANNEL_SSP0_TX],  // pxSemaphoreBuffer
        NULL,                                           // xSemaphore
    },
    {
        GPDMA_CONN_SSP0_Rx,                             // wType
        0,                                              // wChannel
        GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,      
        NULL,                                           // fpDmaCallback
        &m_axDmaSemaphoreBuffer[eDMA_CHANNEL_SSP0_RX],  // pxSemaphoreBuffer
        NULL,
    },
    {
        GPDMA_CONN_SSP1_Tx,                             // wType
        0,                                              // wChannel
        GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
        NULL,                                           // fpDmaCallback
        &m_axDmaSemaphoreBuffer[eDMA_CHANNEL_SSP1_TX],  // pxSemaphoreBuffer
        NULL,                                           // xSemaphore
    },
    {
        GPDMA_CONN_SSP1_Rx,                             // wType
        0,                                              // wChannel
        GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,      
        NULL,                                           // fpDmaCallback
        &m_axDmaSemaphoreBuffer[eDMA_CHANNEL_SSP1_RX],  // pxSemaphoreBuffer
        NULL,
    },
};

// ==============================================================================
// FUNCTION NAME: DMA_IRQHandler
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
// 2019/11/18, Leo Create
// --------------------
// ==============================================================================
void DMA_IRQHandler(void)
{
#if 0
    WORD wCount = 0;
    
    for(wCount = 0; wCount < eDMA_CHANNEL_NUMBERS; wCount++)
    {
        if (Chip_GPDMA_Interrupt(LPC_GPDMA, m_asDmaChanCgf[wCount].wChannel) == SUCCESS)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            
            if(NULL != m_asDmaChanCgf[wCount].fpDmaCallback)
            {
                m_asDmaChanCgf[wCount].fpDmaCallback();
            }
            
            xSemaphoreGiveFromISR( m_asDmaChanCgf[wCount].xSemaphore, &xHigherPriorityTaskWoken );
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }
#endif // 0
}

// ==============================================================================
// FUNCTION NAME: Board_GPDMA_Init
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
// 2019/11/18, Leo Create
// --------------------
// ==============================================================================
void Board_GPDMA_Init(void)
{
    WORD wCount = 0;
    for(wCount = 0; wCount < eDMA_CHANNEL_NUMBERS; wCount++)
    {
        m_asDmaChanCgf[wCount].wChannel = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, m_asDmaChanCgf[wCount].wType);
        m_asDmaChanCgf[wCount].xSemaphore = xSemaphoreCreateMutexStatic(m_asDmaChanCgf[wCount].pxSemaphoreBuffer);
    }

    Chip_GPDMA_Init(LPC_GPDMA);
    
    /* Setting GPDMA interrupt */
    //NVIC_DisableIRQ(DMA_IRQn);
    //NVIC_ClearPendingIRQ(DMA_IRQn);
    //NVIC_SetPriority(DMA_IRQn, 0);
    //NVIC_EnableIRQ(DMA_IRQn);
}

// ==============================================================================
// FUNCTION NAME: Board_GPDMA_DeInit
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
// 2019/11/18, Leo Create
// --------------------
// ==============================================================================
void Board_GPDMA_DeInit(void)
{
    NVIC_ClearPendingIRQ(DMA_IRQn);
    NVIC_DisableIRQ(DMA_IRQn);

    Chip_GPDMA_Init(LPC_GPDMA);
}

// ==============================================================================
// FUNCTION NAME: Board_GPDAM_Reg_Callback
// DESCRIPTION:
//
//
// Params:
// eDMA_CHANNEL eChan:
// fpDMACALLBACK fpCbFunc:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/18, Leo Create
// --------------------
// ==============================================================================
void Board_GPDAM_Reg_Callback(eDMA_CHANNEL eChan, fpDMACALLBACK fpCbFunc)
{
    m_asDmaChanCgf[eChan].fpDmaCallback = fpCbFunc;
}

// ==============================================================================
// FUNCTION NAME: Board_GPDMA_Transfer
// DESCRIPTION:
//
//
// Params:
// eDMA_CHANNEL eChannel:
// BYTE *pcSrc:
// BYTE *pcDst:
// WORD wSize:
// WORD wTimeout:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/18, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_GPDMA_Transfer(eDMA_CHANNEL eChannel, BYTE *pcBufer, WORD wSize, WORD wTimeout)
{
    eRESULT eResult = rcERROR;
    //static BYTE acBuffer[64];
    
    //memset(acBuffer, 0xFF, 64);
    //memcpy(acBuffer, pcBufer, wSize);
    
    //if(pdPASS == xSemaphoreTake(m_asDmaChanCgf[eChannel].xSemaphore, portMAX_DELAY))
    {
        eResult = (eRESULT)Chip_GPDMA_Transfer( LPC_GPDMA,
                                                m_asDmaChanCgf[eChannel].wChannel,
                                                (DWORD)pcBufer,
                                                m_asDmaChanCgf[eChannel].wType,
                                                m_asDmaChanCgf[eChannel].eCtrl,
                                                wSize);
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: Board_GPDMA_Reciver
// DESCRIPTION:
//
//
// Params:
// eDMA_CHANNEL eChannel:
// BYTE *pcSrc:
// BYTE *pcDst:
// WORD wSize:
// WORD wTimeout:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/18, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_GPDMA_Reciver(eDMA_CHANNEL eChannel, BYTE *pcBuffer, WORD wSize, WORD wTimeout)
{
    eRESULT eResult = rcERROR;

    //if(pdPASS == xSemaphoreTake(m_asDmaChanCgf[eChannel].xSemaphore, portMAX_DELAY))
    {
        eResult = (eRESULT)Chip_GPDMA_Transfer( LPC_GPDMA,
                                                m_asDmaChanCgf[eChannel].wChannel,
                                                m_asDmaChanCgf[eChannel].wType,
                                                (DWORD)pcBuffer,
                                                m_asDmaChanCgf[eChannel].eCtrl,
                                                wSize);
    }

    //if(pdPASS == xSemaphoreTake(m_asDmaChanCgf[eChannel].xSemaphore, portMAX_DELAY))
    {
    //    xSemaphoreGive(m_asDmaChanCgf[eChannel].xSemaphore);
    }
   
    return eResult;
}
