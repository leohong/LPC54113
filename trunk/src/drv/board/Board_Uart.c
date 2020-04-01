// ==============================================================================
// FILE NAME: BOARD_UART.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 09/01/2013, Leohong Create
// --------------------
// ==============================================================================

#include "Board.h"
#include "Board_uart.h"


#define UART_TX_INT_ENABLE

// Ring buffer size should be 8 16 32 64 ....
#define TX0_BUFFER_SIZE  64
#define TX1_BUFFER_SIZE  64
#define TX2_BUFFER_SIZE  64
#define TX3_BUFFER_SIZE  2048

static BYTE m_acTx0Buffer[TX0_BUFFER_SIZE];
static BYTE m_acTx1Buffer[TX1_BUFFER_SIZE];
static BYTE m_acTx2Buffer[TX2_BUFFER_SIZE];
static BYTE m_acTx3Buffer[TX3_BUFFER_SIZE];

#define RX0_BUFFER_SIZE  2048
#define RX1_BUFFER_SIZE  1024
#define RX2_BUFFER_SIZE  256//64//4//8
#define RX3_BUFFER_SIZE  512

static BYTE m_acRx0Buffer[RX0_BUFFER_SIZE];
static BYTE m_acRx1Buffer[RX1_BUFFER_SIZE];
static BYTE m_acRx2Buffer[RX2_BUFFER_SIZE];
static BYTE m_acRx3Buffer[RX3_BUFFER_SIZE];

static QueueHandle_t m_xTxQueue[eBOARD_UART_NUMBERS];
static QueueHandle_t m_xRxQueue[eBOARD_UART_NUMBERS];
StaticSemaphore_t m_axUartSemaphoreBuffer[eBOARD_UART_NUMBERS];

//
// LPC43xx UART UART_CFG_Type 參設初始化 Table。
//
static sBOARD_UART_CFG m_asBoard_Uart_Cfg[eBOARD_UART_NUMBERS] =
{
    // UART Port #0
    {
        115200,                         // UART Baudrate
        UART_LCR_WLEN8 |                /*!< UART word length select: 8 bit data mode */
        UART_LCR_PARITY_DIS |           /*!< UART Parity Disable */
        UART_LCR_SBS_1BIT,              /*!< UART stop bit select: 1 stop bit */

        UART_FCR_FIFO_EN |              /*!< UART FIFO enable */
        UART_FCR_RX_RS |                /*!< UART RX FIFO reset */
        UART_FCR_TX_RS |                /*!< UART TX FIFO reset */
        UART_FCR_TRG_LEV3,              /*!< UART FIFO trigger level 3: 14 character */

        UART_IER_RBRINT |               /*!< RBR Interrupt enable */
        UART_IER_RLSINT,                /*!< RX line status interrupt enable */

        0x01,                           // cPreemption
        0x01,                           // cSubPriority

        m_acTx0Buffer,
        TX0_BUFFER_SIZE,
        m_acRx0Buffer,
        RX0_BUFFER_SIZE,

        NULL,   // xSemaphore
        NULL,   // xStaticTxQueue
        NULL,   // xStaticRxQueue
        &m_xTxQueue[eBOARD_UART_UART0],   // xTxQueue
        &m_xRxQueue[eBOARD_UART_UART0],   // xRxQueue
    },
    // UART Port #1
    {
        115200,                         // UART Baudrate
        UART_LCR_WLEN8 |                /*!< UART word length select: 8 bit data mode */
        UART_LCR_PARITY_DIS |           /*!< UART Parity Disable */
        UART_LCR_SBS_1BIT,              /*!< UART stop bit select: 1 stop bit */

        UART_FCR_FIFO_EN |              /*!< UART FIFO enable */
        UART_FCR_RX_RS |                /*!< UART RX FIFO reset */
        UART_FCR_TX_RS |                /*!< UART TX FIFO reset */
        UART_FCR_TRG_LEV0,              /*!< UART FIFO trigger level 0: 1 character */

        UART_IER_RBRINT |               /*!< RBR Interrupt enable */
        UART_IER_RLSINT,                /*!< RX line status interrupt enable */

        0x01,                           // cPreemption
        0x01,                           // cSubPriority

        m_acTx1Buffer,
        TX1_BUFFER_SIZE,

        m_acRx1Buffer,
        RX1_BUFFER_SIZE,

        NULL,   // xSemaphore
        NULL,   // xStaticTxQueue
        NULL,   // xStaticRxQueue
        &m_xTxQueue[eBOARD_UART_UART1],   // xTxQueue
        &m_xRxQueue[eBOARD_UART_UART1],   // xRxQueue
    },
    // UART Port #2
    {
        115200,                         // UART Baudrate
        UART_LCR_WLEN8 |                /*!< UART word length select: 8 bit data mode */
        UART_LCR_PARITY_DIS |           /*!< UART Parity Disable */
        UART_LCR_SBS_1BIT,              /*!< UART stop bit select: 1 stop bit */

        UART_FCR_FIFO_EN |              /*!< UART FIFO enable */
        UART_FCR_RX_RS |                /*!< UART RX FIFO reset */
        UART_FCR_TX_RS |                /*!< UART TX FIFO reset */
        UART_FCR_TRG_LEV0,              /*!< UART FIFO trigger level 0: 1 character */

        UART_IER_RBRINT |               /*!< RBR Interrupt enable */
        UART_IER_RLSINT,                /*!< RX line status interrupt enable */

        0x01,                           // cPreemption
        0x01,                           // cSubPriority

        m_acTx2Buffer,
        TX2_BUFFER_SIZE,

        m_acRx2Buffer,
        RX2_BUFFER_SIZE,

        NULL,   // xSemaphore
        NULL,   // xStaticTxQueue
        NULL,   // xStaticRxQueue
        &m_xTxQueue[eBOARD_UART_UART2],   // xTxQueue
        &m_xRxQueue[eBOARD_UART_UART2],   // pxRxQueue
    },
    // UART Port #3
    {
        115200,                         // UART Baudrate
        UART_LCR_WLEN8 |                /*!< UART word length select: 8 bit data mode */
        UART_LCR_PARITY_DIS |           /*!< UART Parity Disable */
        UART_LCR_SBS_1BIT,              /*!< UART stop bit select: 1 stop bit */

        UART_FCR_FIFO_EN |              /*!< UART FIFO enable */
        UART_FCR_RX_RS |                /*!< UART RX FIFO reset */
        UART_FCR_TX_RS |                /*!< UART TX FIFO reset */
        UART_FCR_TRG_LEV3,              /*!< UART FIFO trigger level 3: 14 character */

        UART_IER_RBRINT |               /*!< RBR Interrupt enable */
        UART_IER_RLSINT,                /*!< RX line status interrupt enable */

        0x01,                           // cPreemption
        0x01,                           // cSubPriority

        m_acTx3Buffer,
        TX3_BUFFER_SIZE,

        m_acRx3Buffer,
        RX3_BUFFER_SIZE,

        NULL,   // xSemaphore
        NULL,   // xStaticTxQueue
        NULL,   // xStaticRxQueue
        &m_xTxQueue[eBOARD_UART_UART3],   // xTxQueue
        &m_xRxQueue[eBOARD_UART_UART3],   // xRxQueue
    },
};

// ==============================================================================
// Uart IRQHandlers
// ==============================================================================
static INLINE void Board_Uart_IRQHandler(eBOARD_UART ePort)
{
    LPC_USART_T *apsBoard_Uart_Reg[] = {LPC_USART0, LPC_UART1, LPC_USART2, LPC_USART3};
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    char cRxedChar;

    /* Handle transmit interrupt if enabled */
    if (apsBoard_Uart_Reg[ePort]->IER & UART_IER_THREINT)
    {
        if(NULL != m_asBoard_Uart_Cfg[ePort].xTxQueue)
        {
            /* Fill FIFO until full or until TX ring buffer is empty */
            while ((Chip_UART_ReadLineStatus(apsBoard_Uart_Reg[ePort]) & UART_LSR_THRE) != 0 &&
                   (xQueueReceiveFromISR(m_asBoard_Uart_Cfg[ePort].xTxQueue, (void *) &cRxedChar, &xHigherPriorityTaskWoken)))
            {
                Chip_UART_SendByte(apsBoard_Uart_Reg[ePort], cRxedChar);
            }

            /* Turn off interrupt if the ring buffer is empty */
            if(xQueueIsQueueEmptyFromISR(m_asBoard_Uart_Cfg[ePort].xTxQueue))
            {
                /* Shut down transmit */
                Chip_UART_IntDisable(apsBoard_Uart_Reg[ePort], UART_IER_THREINT);
            }
        }
        else
        {
            /* Shut down transmit */
            Chip_UART_IntDisable(apsBoard_Uart_Reg[ePort], UART_IER_THREINT);
        }
    }

    /* New data will be ignored if data not popped in time */
    while (Chip_UART_ReadLineStatus(apsBoard_Uart_Reg[ePort]) & UART_LSR_RDR)
    {
        uint8_t ch = Chip_UART_ReadByte(apsBoard_Uart_Reg[ePort]);

        if(NULL != m_asBoard_Uart_Cfg[ePort].xRxQueue)
        {
            xQueueSendFromISR(m_asBoard_Uart_Cfg[ePort].xRxQueue, &ch, &xHigherPriorityTaskWoken);
            //Board_Uart_Write(ePort, 1, &ch, 0); // loop back debug
        }
    }

    Chip_UART_ABIntHandler(apsBoard_Uart_Reg[ePort]);

    /* If xHigherPriorityTaskWoken was set to true you
    we should yield.  The actual macro used here is
    port specific. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

// ==============================================================================
// Uart IRQHandlers
// ==============================================================================
void UART0_IRQHandler(void)
{
    if(NULL != m_asBoard_Uart_Cfg[eBOARD_UART_UART0].pfInterruptCallback)
    {
        m_asBoard_Uart_Cfg[eBOARD_UART_UART0].pfInterruptCallback(eBOARD_UART_UART0);
    }
}

void UART1_IRQHandler(void)
{
    if(NULL != m_asBoard_Uart_Cfg[eBOARD_UART_UART1].pfInterruptCallback)
    {
        m_asBoard_Uart_Cfg[eBOARD_UART_UART1].pfInterruptCallback(eBOARD_UART_UART1);
    }
}

void UART2_IRQHandler(void)
{
    if(NULL != m_asBoard_Uart_Cfg[eBOARD_UART_UART2].pfInterruptCallback)
    {
        m_asBoard_Uart_Cfg[eBOARD_UART_UART2].pfInterruptCallback(eBOARD_UART_UART2);
    }
}

void UART3_IRQHandler(void)
{
    if(NULL != m_asBoard_Uart_Cfg[eBOARD_UART_UART3].pfInterruptCallback)
    {
        m_asBoard_Uart_Cfg[eBOARD_UART_UART3].pfInterruptCallback(eBOARD_UART_UART3);
    }
}

// ==============================================================================
// FUNCTION NAME: BOARD_UART_INIT
// DESCRIPTION:
//
//
// Params:
// eBOARD_UART cPort:
//
// Returns:
//
//
// modification history
// --------------------
// --------------------
// ==============================================================================
void Board_Uart_Init(eBOARD_UART ePort)
{
    LPC_USART_T *apsBoard_Uart_Reg[] = {LPC_USART0, LPC_UART1, LPC_USART2, LPC_USART3};
    IRQn_Type as_Board_Uart_IRQ[eBOARD_UART_NUMBERS] = {USART0_IRQn, UART1_IRQn, USART2_IRQn, USART3_IRQn};
    CHIP_RGU_RST_T aeBoard_Uart_RST[eBOARD_UART_NUMBERS] = {RGU_UART0_RST, RGU_UART1_RST, RGU_UART2_RST, RGU_UART3_RST};

    Chip_RGU_TriggerReset(aeBoard_Uart_RST[ePort]);

    while(Chip_RGU_InReset(aeBoard_Uart_RST[ePort]));

    // To initial LPC43xx UART peripheral...
    Chip_UART_Init(apsBoard_Uart_Reg[ePort]);

    Chip_UART_SetBaudFDR(apsBoard_Uart_Reg[ePort], m_asBoard_Uart_Cfg[ePort].dwBaudrate);

    Chip_UART_ConfigData(apsBoard_Uart_Reg[ePort], m_asBoard_Uart_Cfg[ePort].dwLCR);

    /* Reset FIFOs, Enable FIFOs and in UART */
    Chip_UART_SetupFIFOS(apsBoard_Uart_Reg[ePort],  m_asBoard_Uart_Cfg[ePort].dwFCR);

    /* Before using the ring buffers, initialize them using the ring buffer init function */
    m_asBoard_Uart_Cfg[ePort].xTxQueue = xQueueCreateStatic(m_asBoard_Uart_Cfg[ePort].dwTxBufferSize,
                                                             sizeof(char),
                                                             m_asBoard_Uart_Cfg[ePort].acTxBuffer,
                                                             &m_asBoard_Uart_Cfg[ePort].xStaticTxQueue);

    m_asBoard_Uart_Cfg[ePort].xRxQueue = xQueueCreateStatic(m_asBoard_Uart_Cfg[ePort].dwRxBufferSize,
                                                             sizeof(char),
                                                             m_asBoard_Uart_Cfg[ePort].acRxBuffer,
                                                             &m_asBoard_Uart_Cfg[ePort].xStaticRxQueue);

    // Only one task can be reading/writing at a time
    m_asBoard_Uart_Cfg[ePort].xSemaphore = xSemaphoreCreateMutexStatic(&m_axUartSemaphoreBuffer[ePort]);

    m_asBoard_Uart_Cfg[ePort].pfInterruptCallback = Board_Uart_IRQHandler;

    /* Enable UART Rx & line status interrupts */
    /*
     * Do not enable transmit interrupt here, since it is handled by
     * UART_Send() function, just to reset Tx Interrupt state for the
     * first time
     */
    Chip_UART_IntEnable(apsBoard_Uart_Reg[ePort],  m_asBoard_Uart_Cfg[ePort].dwIER);

    /* Enable UART Transmit */
    Chip_UART_TXEnable(apsBoard_Uart_Reg[ePort]);

    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(as_Board_Uart_IRQ[ePort], eINT_PRIORITY_LEVEL_3);
    NVIC_ClearPendingIRQ(as_Board_Uart_IRQ[ePort]);

    /* Enable Interrupt for UART0 channel */
    NVIC_EnableIRQ(as_Board_Uart_IRQ[ePort]);
}

// ==============================================================================
// FUNCTION NAME: BOARD_UART_DEINIT
// DESCRIPTION:
//
//
// Params:
// eBOARD_UART ePort:
//
// Returns:
//
//
// modification history
// --------------------
// 30/10/2013, Leo Create
// --------------------
// ==============================================================================
void Board_Uart_DeInit(eBOARD_UART ePort)
{
    LPC_USART_T *apsBoard_Uart_Reg[] = {LPC_USART0, LPC_UART1, LPC_USART2, LPC_USART3};
    IRQn_Type as_Board_Uart_IRQ[eBOARD_UART_NUMBERS] = {USART0_IRQn, UART1_IRQn, USART2_IRQn, USART3_IRQn};

    /* Disable Interrupt for UART0 channel */
    NVIC_DisableIRQ(as_Board_Uart_IRQ[ePort]);
    Chip_UART_DeInit(apsBoard_Uart_Reg[ePort]);
}

// ==============================================================================
// FUNCTION NAME: BOARD_UART_BAUDRATE_CHANGE
// DESCRIPTION:
//
//
// Params:
// DWORD dwBaudrate:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/01/13, Leo Create
// --------------------
// ==============================================================================
void Board_Uart_Baudrate_Change(eBOARD_UART ePort, DWORD dwBaudrate)
{
    Board_Uart_DeInit(ePort);
    //m_asBoard_Uart_Cfg[ePort].dwBaudrate = dwBaudrate;
    m_asBoard_Uart_Cfg[ePort].dwBaudrate = 115200;
    Board_Uart_Init(ePort);
}

// ==============================================================================
// FUNCTION NAME: BOARD_UART_WRITE
// DESCRIPTION:
//
//
// Params:
// eBOARD_UART ePort:
// WORD wSize:
// BYTE *pOutBuffer:
//
// Returns:
//
//
// modification history
// --------------------
// 30/10/2013, Leo Create
// --------------------
// ==============================================================================
#ifdef UART_TX_INT_ENABLE
eRESULT Board_Uart_Write(eBOARD_UART ePort, WORD wSize, BYTE *pOutBuffer, DWORD dwTimeout)
{
    LPC_USART_T *apsBoard_Uart_Reg[] = {LPC_USART0, LPC_UART1, LPC_USART2, LPC_USART3};
    eRESULT eResult = rcERROR;
    BYTE cChar = 0;

    if((NULL != m_asBoard_Uart_Cfg[ePort].xSemaphore) && (NULL != m_asBoard_Uart_Cfg[ePort].xTxQueue))
    {
        if(pdPASS == xSemaphoreTake(m_asBoard_Uart_Cfg[ePort].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        {
            /* Move as much data as possible into transmit ring buffer */
            while(wSize)
            {
                if(pdFALSE == xQueueSend(m_asBoard_Uart_Cfg[ePort].xTxQueue, (void *)pOutBuffer, (TickType_t) 0))
                //if(pdFALSE == xQueueSend(m_asBoard_Uart_Cfg[ePort].xTxQueue, (void *)pOutBuffer, pdMS_TO_TICKS(10)))
                {
                    // Queue full, drop data.
                    break;
                }

                pOutBuffer++;
                wSize--;
            }

            /* Don't let UART transmit ring buffer change in the UART IRQ handler */
            Chip_UART_IntDisable(apsBoard_Uart_Reg[ePort], UART_IER_THREINT);

            /* Fill FIFO until full or until TX ring buffer is empty */
            while ((Chip_UART_ReadLineStatus(apsBoard_Uart_Reg[ePort]) & UART_LSR_THRE) != 0 &&
                   (pdTRUE == xQueueReceive(m_asBoard_Uart_Cfg[ePort].xTxQueue, &cChar, (TickType_t) 0)))
            {
                Chip_UART_SendByte(apsBoard_Uart_Reg[ePort], cChar);
            }

            if(0 == wSize)
            {
                eResult = rcSUCCESS;
            }

            /* Enable UART transmit interrupt */
            Chip_UART_IntEnable(apsBoard_Uart_Reg[ePort], UART_IER_THREINT);

            xSemaphoreGive(m_asBoard_Uart_Cfg[ePort].xSemaphore);
        }
    }

    return eResult;
}
#else
eRESULT Board_Uart_Write(eBOARD_UART ePort, WORD wSize, BYTE *pOutBuffer, DWORD dwTimeout)
{
    LPC_USART_T *apsBoard_Uart_Reg[] = {LPC_USART0, LPC_UART1, LPC_USART2, LPC_USART3};
    eRESULT eResult = rcERROR;

    if((NULL != m_asBoard_Uart_Cfg[ePort].xSemaphore) && (NULL != m_asBoard_Uart_Cfg[ePort].xTxQueue))
    {
        if(pdPASS == xSemaphoreTake(m_asBoard_Uart_Cfg[ePort].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        {
            Chip_UART_SendBlocking(apsBoard_Uart_Reg[ePort], pOutBuffer, wSize);
            xSemaphoreGive(m_asBoard_Uart_Cfg[ePort].xSemaphore);
        }
    }

    return eResult;
}
#endif // 0
// ==============================================================================
// FUNCTION NAME: BOARD_UART_READ
// DESCRIPTION:
//
//
// Params:
// eBOARD_UART ePort:
// WORD wSize:
// BYTE *pOutBuffer:
//
// Returns:
//
//
// modification history
// --------------------
// 14/11/2013, Leo Create
// --------------------
// ==============================================================================
WORD Board_Uart_Read(eBOARD_UART ePort, WORD wSize, BYTE *pOutBuffer, DWORD dwTimeout)
{
    WORD wReadCount = 0;

    while(wReadCount < wSize)
    {
        if((NULL == m_asBoard_Uart_Cfg[ePort].xRxQueue) || (pdFALSE == xQueueReceive(m_asBoard_Uart_Cfg[ePort].xRxQueue, pOutBuffer, pdMS_TO_TICKS(dwTimeout))))
        {
            break;
        }

        wReadCount++;
        pOutBuffer++;
    }

    return wReadCount;
}

// ==============================================================================
// FUNCTION NAME: BOARD_UART_RX_FIFOCOUNT
// DESCRIPTION:
//
//
// Params:
// eBOARD_UART ePort:
//
// Returns:
//
//
// modification history
// --------------------
// 14/11/2013, Leo Create
// --------------------
// ==============================================================================
BYTE Board_Uart_Rx_FifoCount(eBOARD_UART ePort)
{
    return uxQueueSpacesAvailable(m_asBoard_Uart_Cfg[ePort].xRxQueue);
}

// ==============================================================================
// FUNCTION NAME: BOARD_UART_RX_FIFOFLUSH
// DESCRIPTION:
//
//
// Params:
// eBOARD_UART ePort:
//
// Returns:
//
//
// modification history
// --------------------
// 14/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_Uart_Rx_FifoFlush(eBOARD_UART ePort)
{
    xQueueReset(m_asBoard_Uart_Cfg[ePort].xRxQueue);
}

// ==============================================================================
// FUNCTION NAME: Board_Uart_Interrupt_Register
// DESCRIPTION:
//
//
// Params:
// eBOARD_UART ePort:
// fpUART_INT_CALLBACK fpCallback:
//
// Returns:
//
//
// modification history
// --------------------
// 14/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_Uart_Interrupt_Register(eBOARD_UART ePort, fpUART_INT_CALLBACK fpCallback)
{
    m_asBoard_Uart_Cfg[ePort].pfInterruptCallback = fpCallback;
}

