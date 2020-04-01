#ifndef BOARD_UART_H
#define BOARD_UART_H
// ==============================================================================
// FILE NAME: BOARD_UART.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 09/01/2013, Leohong written
// --------------------
// ==============================================================================

#include "CommonDef.h"

#define uaHDUART            eBOARD_UART_UART0
#define uaSCALER_MCU_UART   eBOARD_UART_UART1
#define uaMST9U13UART       eBOARD_UART_UART2
#define uaNXPUART           eBOARD_UART_UART3

typedef enum
{
    eBOARD_UART_UART0,   //
    eBOARD_UART_UART1,   // OPS
    eBOARD_UART_UART2,   //
    eBOARD_UART_UART3,   // (T100) to PC

    eBOARD_UART_NUMBERS,
} eBOARD_UART;

typedef enum
{
    /* UART Data Bits Definitions */
    eBOARD_UART_DATA_BITS_5 = UART_LCR_WLEN5,
    eBOARD_UART_DATA_BITS_6 = UART_LCR_WLEN6,
    eBOARD_UART_DATA_BITS_7 = UART_LCR_WLEN7,
    eBOARD_UART_DATA_BITS_8 = UART_LCR_WLEN8,

    eBOARD_UART_DATA_BITS_NUMBERS
} eBOARD_UART_DATA_BITS;

typedef enum URT_StopBits
{
    /* UART Stop Bits Definitions */
    eBOARD_UART_STOP_BITS_1 = UART_LCR_SBS_1BIT,
    eBOARD_UART_STOP_BITS_2 = UART_LCR_SBS_2BIT,

    eBOARD_UART_STOP_BITS_NUMBERS
} eBOARD_UART_STOP_BITS;

typedef enum URT_Parity
{
    /* UART Parity Definitions */
    eBOARD_UART_PARITY_NONE = UART_LCR_PARITY_DIS,
    eBOARD_UART_PARITY_EVEN = UART_LCR_PARITY_EVEN | UART_LCR_PARITY_EN,
    eBOARD_UART_PARITY_ODD  = UART_LCR_PARITY_ODD | UART_LCR_PARITY_EN,

    eBOARD_UART_PARITY_NUMBERS
} eBOARD_UART_PARITY;

typedef enum URT_FlowControl
{
    /* UART Flow Control Definitions */
    eBOARD_UART_FLOW_CTR_OFF,
    eBOARD_UART_FLOW_CTR_HW,

    eBOARD_UART_FLOW_CTR_NUMBERS
} eBOARD_UART_FLOW_CTR;

typedef enum
{
    /* UART RX FIFO Interrupt Trigger Level Definitions    */
    /* Interrupt occurs as FIFO fills and the FIFO pointer */
    /* passes through the selected trigger value.          */
    eBOARD_UART_RX_TRIGGER_ONE_CHAR,
    eBOARD_UART_RX_TRIGGER_FOUR_CHAR,
    eBOARD_UART_RX_TRIGGER_EIGHT_CHAR,
    eBOARD_UART_RX_TRIGGER_FOURTEEN_CHAR,

    eBOARD_UART_RX_TRIGGER_NUMBERS
} eBOARD_UART_RX_TRIGGER;

typedef enum
{
    /* UART RXD Input Polarity */
    eBOARD_UART_RX_POLARITY_NON_INVERTED,   /**< Supply non-inverted version of UART_RXD input */
    eBOARD_UART_RX_POLARITY_INVERTED,       /**< Supply inverted version of UART_RXD input */

    eBOARD_UART_RX_POLARITY_NUMBERS,
} eBOARD_UART_RX_POLARITY;

/* Macros / constants */

typedef void (*fpUART_INT_CALLBACK)(eBOARD_UART cPort);

/** UART Intitialization Structure */
typedef struct
{
    DWORD                   dwBaudrate;      // UART Baudrate
    DWORD                   dwLCR;           // UART Line Control Register
    DWORD                   dwFCR;           // UART FIFO Control Register
    DWORD                   dwIER;           // UART Interrupt Enable Register
    BYTE                    cPreemption;
    BYTE                    cSubPriority;

    BYTE                    *acTxBuffer;
    DWORD                   dwTxBufferSize;

    BYTE                    *acRxBuffer;
    DWORD                   dwRxBufferSize;

    SemaphoreHandle_t       xSemaphore;
    StaticQueue_t           xStaticTxQueue;
    StaticQueue_t           xStaticRxQueue;
    QueueHandle_t           xTxQueue;
    QueueHandle_t           xRxQueue;

    fpUART_INT_CALLBACK     pfInterruptCallback;
} sBOARD_UART_CFG;

#if 0
void Board_Uart_Init(BOARD_UART_CFG *psUartCfg);
#else
void Board_Uart_Init(eBOARD_UART cPort);
#endif /* 1 */
void Board_Uart_DeInit(eBOARD_UART ePort);
void Board_Uart_Baudrate_Change(eBOARD_UART ePort, DWORD dwBaudrate);
eRESULT Board_Uart_Write(eBOARD_UART ePort, WORD wSize, BYTE *pOutBuffer, DWORD dwTimeout);
WORD Board_Uart_Read(eBOARD_UART ePort, WORD wSize, BYTE *pOutBuffer, DWORD dwTimeout);
void Board_Uart_Rx_FifoFlush(eBOARD_UART ePort);
BYTE Board_Uart_Rx_FifoCount(eBOARD_UART ePort);
void Board_Uart_Interrupt_Register(eBOARD_UART ePort, fpUART_INT_CALLBACK fpCallback);


//Leo 20180823 start
/* Sends a character on the UART */
static INLINE void Board_UARTPutChar(char ch)
{
#ifdef __DEBUG__
    Board_Uart_Write(API_RS232_PORT, 1, (BYTE *)&ch, 0);
#else
    (void)ch;
#endif // __DEBUG__
}

/* Gets a character from the UART, returns EOF if no character is ready */
static INLINE int Board_UARTGetChar(void)
{
#ifdef __DEBUG__
    BYTE cBuffer = 0;

    if(Board_Uart_Read(API_RS232_PORT, 1, &cBuffer, 0))
    {
        return cBuffer;
    }
#endif // __DEBUG__
    return EOF;
}

/* Outputs a string on the debug UART */
static INLINE void Board_UARTPutSTR(const char *str)
{
#ifdef __DEBUG__
    Board_Uart_Write(API_RS232_PORT, strlen(str), (BYTE *)str, 0);
#else
    (void)str;
#endif // __DEBUG__
}
//Leo 20180823 end


#endif // BOARD_UART_H

