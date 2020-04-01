#ifndef BOARD_SPI_H
#define BOARD_SPI_H
// ==============================================================================
// FILE NAME: BOARD_SPI.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================


#include "CommonDef.h"
#include "Board_GPIO_Pin_Name.h"

typedef enum
{
    eBOARD_SSP_SSP0,
    eBOARD_SSP_SSP1,

    eBOARD_SSP_NUMBERS,
} eBOARD_SSP;

#if 0
typedef struct
{
    eBOARD_SSP              eBus;
    CHIP_SSP_MODE_T         eMode;
    CHIP_SSP_BITS_T         eDatabitNumber;
    CHIP_SSP_FRAME_FORMAT_T eFrameFormat;
    CHIP_SSP_CLOCK_MODE_T   eClkMode;
    DWORD                   dwClockrate;
    BYTE                    cPreemption;
    BYTE                    cSubPriority;
} sBOARD_SSP_CFG;
#else
typedef struct
{
    IRQn_Type               eSPI_IRQ;
    LPC_SSP_T               *psSPI_Port;
    CHIP_SSP_MODE_T         Mode;
    CHIP_SSP_BITS_T         DatabitNumber;
    CHIP_SSP_FRAME_FORMAT_T FrameFormat;
    CHIP_SSP_CLOCK_MODE_T   ClkMode;
    DWORD                   dwClockrate;
    BYTE                    cPreemption;
    BYTE                    cSubPriority;

    StaticSemaphore_t       *pxSemaphoreBuffer;
    SemaphoreHandle_t       xSemaphore;
} sBOARD_SSP_CFG;
#endif /* 1 */


void Board_SSP_IRQ_Handler(eBOARD_SSP eBus);
void Board_SSP_Init(eBOARD_SSP eBus);
void Board_SSP_DeInit(eBOARD_SSP eBus);
//eRESULT Board_SSP_Master_Write(eBOARD_SSP eBus, UINT16 uiDataSz, UINT8 *pucData);
eRESULT Board_SSP_Master_Write(eBOARD_SSP eBus, WORD wDataSz, BYTE *pcData);
eRESULT Board_SSP_Master_MultiWrite(eBOARD_SSP eBus, BYTE cChipSelectPINNumber, eBOARD_PIN_TABLE *aeChipSelectPin, WORD wDataSz, BYTE *pcData);
eRESULT Board_SSP_Master_Read(eBOARD_SSP eBus, WORD wDataSz, BYTE *pcRxData);
eRESULT Board_SSP_Slave_Write(eBOARD_SSP eBus, WORD wDataSz, BYTE *pcTxData);
eRESULT Board_SSP_Slave_Read(eBOARD_SSP eBus, WORD wDataSz, BYTE *pcRxData);

void Board_SSP_Wait_Busy(eBOARD_SSP eBus); //A70LH_Larry_0001
void Board_SSP_Slave_TxBufferClaer(eBOARD_SSP eBus); //T100_Larry_0005

#endif //BOARD_SPI_H

