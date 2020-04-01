// ==============================================================================
// FILE NAME: DVCORE_SSP.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================


#include "CommonDef.h"
#include "Board.h"
#include "Board_Spi.h"
#include "Board_Gpio.h"


//
// LPC43xx SSP 參數初始化 Table。
//
StaticSemaphore_t m_axSpiSemaphoreBuffer[eBOARD_SSP_NUMBERS];

sBOARD_SSP_CFG m_apsBoard_Ssp_Cfg[eBOARD_SSP_NUMBERS] =
{
    {
        SSP0_IRQn,              // SPI_IRQ
        LPC_SSP0,               // SPI Port
        SSP_MODE_MASTER,        // Mode
        SSP_BITS_8,             // DatabitNumber
        SSP_FRAMEFORMAT_SPI,    // FrameFormat
        SSP_CLOCK_MODE0,        // ClkMode
        8000000,                // Clockrate
        0x01,                   // cPreemption
        0x01,                   // cSubPriority
        &m_axSpiSemaphoreBuffer[eBOARD_SSP_SSP0],
        NULL,                   // xSemaphore
    },

    {
        SSP1_IRQn,              // SPI_IRQ
        LPC_SSP1,               // SPI Port
        SSP_MODE_SLAVE,         // Mode
        SSP_BITS_8,             // DatabitNumber
        SSP_FRAMEFORMAT_SPI,    // FrameFormat
        SSP_CLOCK_MODE3,        // ClkMode
        8000000,                // Clockrate
        0x01,                   // cPreemption
        0x01,                   // cSubPriority
        &m_axSpiSemaphoreBuffer[eBOARD_SSP_SSP1],
        NULL,                   // xSemaphore
    }
};

#define SSP_BUFFER_SIZE (1024)  //64

static StaticQueue_t m_axSpiTxQueue[eBOARD_SSP_NUMBERS];
static QueueHandle_t m_axSpiTxQueueHDL[eBOARD_SSP_NUMBERS];
static BYTE m_acSpiTxBuffer[eBOARD_SSP_NUMBERS][SSP_BUFFER_SIZE];

static StaticQueue_t m_axSpiRxQueue[eBOARD_SSP_NUMBERS];
static QueueHandle_t m_axSpiRxQueueHDL[eBOARD_SSP_NUMBERS];
static BYTE m_acSpiRxBuffer[eBOARD_SSP_NUMBERS][SSP_BUFFER_SIZE];

static const eBOARD_PIN_TABLE m_aeChipSelect[eBOARD_SSP_NUMBERS] = {pCARD_SPI_CS, pPV7_SPI_CSn};

// ==============================================================================
// FUNCTION NAME: SSP0_IRQHANDLER
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
// 22/05/2013, Leohong Create
// --------------------
// ==============================================================================
void SSP0_IRQHandler(void)
{
    Board_SSP_IRQ_Handler(eBOARD_SSP_SSP0);
}

// ==============================================================================
// FUNCTION NAME: SSP1_IRQHandler
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/08/27, Leo Create
// --------------------
// ==============================================================================
void SSP1_IRQHandler(void)
{
    Board_SSP_IRQ_Handler(eBOARD_SSP_SSP1);
}

// ==============================================================================
// FUNCTION NAME: BOARD_SSP_IRQ_HANDLER
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 13/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_SSP_IRQ_Handler(eBOARD_SSP eBus)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Disable all interrupt */
    // Chip_SSP_Int_Disable(apsBoard_Ssp_Reg[eBus], SSP_INT_MASK_BITMASK);

    // 檢查Rx buffer是否已經overrun
    if(Chip_SSP_GetRawIntStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_RORRIS) == SET)
    {
        // 清除Rx buffer中資料
        while(Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_RNE))
        {
            Chip_SSP_ReceiveFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port);
        }
    }

    // 檢查 Rx Buffer中是否有資料等待接收
    while(Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_RNE))
    {
        // 寫入Rx Ring buffer
        if(m_apsBoard_Ssp_Cfg[eBus].DatabitNumber > SSP_BITS_8)
        {
            //WORD wData = 0;
            //wData = Chip_SSP_ReceiveFrame(pSSP_Reg);
            //xQueueSendFromISR(m_xSpiRxQueueHDL, (void *)&wData, &xHigherPriorityTaskWoken);
        }
        else
        {
            BYTE cData = 0;
            cData = (uint8_t)Chip_SSP_ReceiveFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port);

            if(NULL != m_axSpiRxQueueHDL[eBus])
            {
                xQueueSendFromISR(m_axSpiRxQueueHDL[eBus], (void *)&cData, &xHigherPriorityTaskWoken);
            }
        }
    }

    // Tx Ring buffer中是否有資料要傳送,  Output FIFO是否仍有空間?
    if((0 != uxQueueMessagesWaitingFromISR(m_axSpiTxQueueHDL[eBus])) && (NULL != m_axSpiTxQueueHDL[eBus]))
    {
        while((SET == Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_TNF)))
        {
            // 將Ring Buffer資料搬入Output fifo
            // 16 bit 傳輸
            if(m_apsBoard_Ssp_Cfg[eBus].DatabitNumber > SSP_BITS_8)
            {
                WORD wData;
                xQueueReceiveFromISR(m_axSpiTxQueueHDL[eBus], (void *)&wData, &xHigherPriorityTaskWoken);
                //Chip_SSP_SendFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, wData);
            }
            else
            {
                BYTE cData = 0xFF;
                xQueueReceiveFromISR(m_axSpiTxQueueHDL[eBus], (void *)&cData, &xHigherPriorityTaskWoken);
                Chip_SSP_SendFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, cData);
            }

            // 檢查Rx buffer是否已經overrun
            if(Chip_SSP_GetRawIntStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_RORRIS) == SET)
            {
                // 清除Rx buffer中資料
                while(Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_RNE))
                {
                    Chip_SSP_ReceiveFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port);
                }
            }

            // 檢查 Rx Buffer中是否有資料
            while(Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_RNE))
            {
                // 寫入rx ring buffer
                if(m_apsBoard_Ssp_Cfg[eBus].DatabitNumber > SSP_BITS_8)
                {
                    //WORD wData = 0;
                    //wData = Chip_SSP_ReceiveFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port);
                    //xQueueSendFromISR(m_xSpiRxQueueHDL, (void *)&wData, &xHigherPriorityTaskWoken);
                }
                else
                {
                    BYTE cData = 0;
                    cData = (uint8_t)Chip_SSP_ReceiveFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port);

                    if(NULL != m_axSpiRxQueueHDL[eBus])
                    {
                        xQueueSendFromISR(m_axSpiRxQueueHDL[eBus], (void *)&cData, &xHigherPriorityTaskWoken);
                    }
                }
            }
        }
    }
    else
    {
        Chip_SSP_SendFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, 0xFF);

        if(m_apsBoard_Ssp_Cfg[eBus].Mode != SSP_MODE_SLAVE)
        {
            Chip_SSP_Int_Disable(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_TXIM);
            //xSemaphoreGiveFromISR(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, &xHigherPriorityTaskWoken);
            //Board_Gpio_Set(m_aeChipSelect[eBus], HIGH) ;
        }
    }

    Chip_SSP_ClearIntPending(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_INT_CLEAR_BITMASK);

    /* If xHigherPriorityTaskWoken was set to true we should yield */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

// ==============================================================================
// FUNCTION NAME: BOARD_SSP_INIT
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 13/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_SSP_Init(eBOARD_SSP eBus)
{
    Chip_SSP_Init(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port);

    Chip_SSP_Set_Mode(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, m_apsBoard_Ssp_Cfg[eBus].Mode);

    Chip_SSP_SetFormat(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port,
                       m_apsBoard_Ssp_Cfg[eBus].DatabitNumber,
                       m_apsBoard_Ssp_Cfg[eBus].FrameFormat,
                       m_apsBoard_Ssp_Cfg[eBus].ClkMode);

    Chip_SSP_SetBitRate(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, m_apsBoard_Ssp_Cfg[eBus].dwClockrate);
    Chip_SSP_Enable(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port);

    m_axSpiTxQueueHDL[eBus] = xQueueCreateStatic(  SSP_BUFFER_SIZE,
                                            sizeof(char),
                                            m_acSpiTxBuffer[eBus],
                                            &m_axSpiTxQueue[eBus]);

    m_axSpiRxQueueHDL[eBus] = xQueueCreateStatic(  SSP_BUFFER_SIZE,
                                            sizeof(char),
                                            m_acSpiRxBuffer[eBus],
                                            &m_axSpiRxQueue[eBus]);

    if(m_apsBoard_Ssp_Cfg[eBus].Mode == SSP_MODE_SLAVE)
    {
        Chip_SSP_Int_Enable(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, (SSP_INTMASK_T)(SSP_RORIM | SSP_RTIM | SSP_RXIM));
    }

    /* Setting SSP interrupt */
    NVIC_SetPriority(m_apsBoard_Ssp_Cfg[eBus].eSPI_IRQ, 1 /* eINT_PRIORITY_LEVEL_1*/);
    NVIC_ClearPendingIRQ(m_apsBoard_Ssp_Cfg[eBus].eSPI_IRQ);
    NVIC_EnableIRQ(m_apsBoard_Ssp_Cfg[eBus].eSPI_IRQ);

    m_apsBoard_Ssp_Cfg[eBus].xSemaphore = xSemaphoreCreateMutexStatic(m_apsBoard_Ssp_Cfg[eBus].pxSemaphoreBuffer);
}

// ==============================================================================
// FUNCTION NAME: BOARD_SSP_DEINIT
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 13/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_SSP_DeInit(eBOARD_SSP eBus)
{
    /* Disable Interrupt for SSP channel */
    NVIC_DisableIRQ(m_apsBoard_Ssp_Cfg[eBus].eSPI_IRQ);
    Chip_SSP_DeInit(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port);
}

// ==============================================================================
// FUNCTION NAME: Board_SSP_Master_Write
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
// WORD wDataSz:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/10/29, Leo Create
// SPI does not read when write.
// --------------------
// ==============================================================================
#if 0
eRESULT Board_SSP_Master_Write(eBOARD_SSP eBus, WORD wDataSz, BYTE *pcData)
{
    eRESULT eResult = rcERROR;

    if((NULL != m_apsBoard_Ssp_Cfg[eBus].xSemaphore)&&(NULL != m_axSpiTxQueueHDL[eBus]))
    {
        //if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, portMAX_DELAY))
        {
            Board_Gpio_Set(m_aeChipSelect[eBus], LOW) ;

            if (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY))
            {
                while (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY)) {};
            }

            while(wDataSz)
            {
                //if(pdFALSE == xQueueSend(m_asBoard_Uart_Cfg[ePort].pxTxQueue, (void *)pOutBuffer, (TickType_t) 0))
                //if(pdFALSE == xQueueSend(m_axSpiTxQueueHDL[eBus], (void *)pcData, pdMS_TO_TICKS(1000)))
                if(pdFALSE == xQueueSend(m_axSpiTxQueueHDL[eBus], (void *)pcData, portMAX_DELAY))
                {
                    // Queue full, drop data.
                    break;
                }

                pcData++;
                wDataSz--;
            }

            while(SET == Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_TNF))
            {
                // 將Ring Buffer資料搬入Output fifo
                // 16 bit 傳輸
                if(m_apsBoard_Ssp_Cfg[eBus].DatabitNumber > SSP_BITS_8)
                {
                    //uint16_t wData;
                    //xQueueReceiveFromISR(m_xSpiTxQueueHDL, (void *)&wData, &xHigherPriorityTaskWoken);
                    //Chip_SSP_SendFrame(pSSP_Reg, wData);
                }
                else
                {
                    uint8_t cData;

                    if(pdTRUE == xQueueReceive(m_axSpiTxQueueHDL[eBus], (void *)&cData, 0))
                    {
                        Chip_SSP_SendFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, cData);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            Chip_SSP_Int_Enable(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_TXIM);
            eResult = rcSUCCESS;
            //xSemaphoreGive(m_apsBoard_Ssp_Cfg[eBus].xSemaphore);
        }
    }

    return eResult;
}
#endif // 0

#if 1
eRESULT Board_SSP_Master_Write(eBOARD_SSP eBus, WORD wDataSz, BYTE *pcData)
{
    eRESULT eResult = rcERROR;

    if(NULL != m_apsBoard_Ssp_Cfg[eBus].xSemaphore)
    {
        //if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, portMAX_DELAY))
        {
            if (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY))
            {
                while (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY)) {};
            }

            Board_Gpio_Set(m_aeChipSelect[eBus], LOW) ;

            //if(xf_setup.length != Chip_SSP_RWFrames_Blocking(apsBoard_Ssp_Reg[eBus], &xf_setup))
            if(wDataSz != Chip_SSP_WriteFrames_Blocking(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, pcData, wDataSz))
            {
                goto SSP_MW_END; // rcERROR
            }

            eResult = rcSUCCESS;
SSP_MW_END:
            Board_Gpio_Set(m_aeChipSelect[eBus], HIGH) ;
            xSemaphoreGive(m_apsBoard_Ssp_Cfg[eBus].xSemaphore);
        }
    }

    return eResult;
}
#endif // 0

// ==============================================================================
// FUNCTION NAME: Board_SSP_Master_MultiWrite
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
// BYTE cChipSelectPINNumber:
// eBOARD_PIN_TABLE *aeChipSelectPin:
// WORD wDataSz:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
//
// --------------------
// ==============================================================================
eRESULT Board_SSP_Master_MultiWrite(eBOARD_SSP eBus, BYTE cChipSelectPINNumber, eBOARD_PIN_TABLE *aeChipSelectPin, WORD wDataSz, BYTE *pcData)
{
    eRESULT eResult = rcERROR;
    BYTE cIndex = 0 ;

    if(NULL != m_apsBoard_Ssp_Cfg[eBus].xSemaphore)
    {
        //if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, portMAX_DELAY))
        {
            if (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY))
            {
                while (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY)) {};
            }

            for(cIndex = 0 ; cIndex < cChipSelectPINNumber ; cIndex++)
            {
                Board_Gpio_Set(aeChipSelectPin[cIndex], LOW) ;
            }

            if(wDataSz != Chip_SSP_WriteFrames_Blocking(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, pcData, wDataSz))
            {
                goto SSP_MMW_END; // rcERROR
            }

            eResult = rcSUCCESS;
SSP_MMW_END:
            for(cIndex = 0 ; cIndex < cChipSelectPINNumber ; cIndex++)
            {
                Board_Gpio_Set(aeChipSelectPin[cIndex], HIGH) ;
            }

            xSemaphoreGive(m_apsBoard_Ssp_Cfg[eBus].xSemaphore);
        }
    }

    return eResult;
}


// ==============================================================================
// FUNCTION NAME: Board_SSP_Master_Read
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
// eBOARD_PIN_TABLE eChipSelectPin:
// WORD wDataSz:
// BYTE *pcTxData:
// BYTE *pcRxData:
//
// Returns:
//
//
// modification history
// --------------------
// 13/11/2013, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_SSP_Master_Read(eBOARD_SSP eBus, WORD wDataSz, BYTE *pcRxData)
{
    eRESULT eResult = rcERROR;

    if(NULL != m_apsBoard_Ssp_Cfg[eBus].xSemaphore)
    {
        //if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, portMAX_DELAY))
        {
            if (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY))
            {
                while (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY)) {};
            }

            Board_Gpio_Set(m_aeChipSelect[eBus], LOW) ;

            if(wDataSz != Chip_SSP_ReadFrames_Blocking(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, pcRxData, wDataSz))
            {
                goto SSP_MR_END; // rcERROR
            }

            eResult = rcSUCCESS;
SSP_MR_END:
            Board_Gpio_Set(m_aeChipSelect[eBus], HIGH) ;
            xSemaphoreGive(m_apsBoard_Ssp_Cfg[eBus].xSemaphore);
        }
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_SSP_SLAVE_SEND
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
// WORD wDataSz:
// BYTE *pcTxData:
//
// Returns:
//
//
// modification history
// --------------------
// 18/11/2013, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_SSP_Slave_Write(eBOARD_SSP eBus, WORD wDataSz, BYTE *pcTxData)
{
    eRESULT eResult = rcERROR;

    if((NULL != m_apsBoard_Ssp_Cfg[eBus].xSemaphore)&&(NULL != m_axSpiTxQueueHDL[eBus]))
    {
        //if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, portMAX_DELAY))
        {
            while(wDataSz)
            {
                if(pdFALSE == xQueueSend(m_axSpiTxQueueHDL[eBus], (void *)pcTxData, pdMS_TO_TICKS(1000)))
                {
                    break;
                }

                pcTxData++;
                wDataSz--;
            }

            /* Disable all interrupt */
            Chip_SSP_Int_Disable(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, (SSP_INTMASK_T)(SSP_RORIM | SSP_RTIM | SSP_RXIM));

            while(  (0 != uxQueueMessagesWaiting(m_axSpiTxQueueHDL[eBus]))
                    && (SET == Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_TNF)))
            {
                // 將Ring Buffer資料搬入Output fifo
                if(m_apsBoard_Ssp_Cfg[eBus].DatabitNumber > SSP_BITS_8)
                {
                    //Chip_SSP_SendFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, wData);
                }
                else
                {
                    BYTE cChar = 0xFF;

                    if(pdTRUE == xQueueReceive(m_axSpiTxQueueHDL[eBus], &cChar, 0))
                    {
                        Chip_SSP_SendFrame(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, cChar);
                    }
                }
            }

            eResult = rcSUCCESS;

            // 開啟Rx中斷
            Chip_SSP_Int_Enable(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, (SSP_INTMASK_T)(SSP_RORIM | SSP_RTIM | SSP_RXIM));
            xSemaphoreGive(m_apsBoard_Ssp_Cfg[eBus].xSemaphore);
        }
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_SSP_SLAVE_READ
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
// UINT16 uiDataSz:
// UINT8 *pucRxData:
//
// Returns:
//
//
// modification history
// --------------------
// 18/11/2013, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_SSP_Slave_Read(eBOARD_SSP eBus, WORD wDataSz, BYTE *pcRxData)
{
    eRESULT eResult = rcERROR;
    WORD wCount = 0;

    if((NULL != m_apsBoard_Ssp_Cfg[eBus].xSemaphore)&&(NULL != m_axSpiRxQueueHDL[eBus]))
    {
        //if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        if(pdPASS == xSemaphoreTake(m_apsBoard_Ssp_Cfg[eBus].xSemaphore, portMAX_DELAY))
        {
            for( wCount = 0; wCount < wDataSz; wCount++)
            {
                if(pdFALSE == xQueueReceive(m_axSpiRxQueueHDL[eBus], &pcRxData[wCount], portMAX_DELAY))
                {
                    break;
                }
            }

            eResult = rcSUCCESS;
            xSemaphoreGive(m_apsBoard_Ssp_Cfg[eBus].xSemaphore);
       }
    }

    return eResult;
}


// ==============================================================================
// FUNCTION NAME: Board_SSP_Wait_Busy
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/08/03, Larry Create
// --------------------
// ==============================================================================
void Board_SSP_Wait_Busy(eBOARD_SSP eBus)
{
    if (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY))
    {
        while (Chip_SSP_GetStatus(m_apsBoard_Ssp_Cfg[eBus].psSPI_Port, SSP_STAT_BSY)) {}
    }
}

// ==============================================================================
// FUNCTION NAME: Board_SSP_Slave_BufferClaer
// DESCRIPTION:
//
//
// Params:
// eBOARD_SSP eBus:
//
// Returns:
//
//
// Modification History
// --------------------
// 2018/04/18, Larry Create
// --------------------
// ==============================================================================
void Board_SSP_Slave_TxBufferClaer(eBOARD_SSP eBus)
{
    if(NULL != m_axSpiTxQueueHDL[eBus])
    {
        xQueueReset(m_axSpiTxQueueHDL[eBus]);
    }
}

