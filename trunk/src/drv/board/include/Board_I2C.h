#ifndef BOARD_I2C_H
#define BOARD_I2C_H
// ==============================================================================
// FILE NAME: BOARD_I2C.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 03/01/2013, Leohong written
// --------------------
// ==============================================================================


#include "CommonDef.h"
#include "utilCounterAPI.h"

#define I2C_SLAVE_ADD   0x66

#define I2C_DEFAULT     (0x01<<0)
#define I2C_REG_NEED    (0x01<<1)
#define I2C_WORD_REG    (0x01<<2)
#define I2C_IGNORE_ACK  (0x01<<3)
#define I2C_ADD_TRAN    (0x01<<4)  //A70LH_Larry_0209 //Read only

#define I2CBD_K(x)      (x*1000L)

#define I2C_REG     0x01
#define I2C_RX_DATA 0x02
#define I2C_EXCUTE  0x03

typedef enum
{
    eBOARD_I2C_BUS0,
    eBOARD_I2C_BUS1,
    eBOARD_I2C_BUS2,

    eBOARD_I2C_NUMBERS,
} eBOARD_I2C;

typedef enum
{
    eI2C_MODE_HW,
    eI2C_MODE_SW,

    eI2C_MODE_NUMBERS,
} eI2C_MODE;

typedef struct
{
    eI2C_MODE           eMode;
    WORD                wKHz;
    BYTE                cSlaveEnable;
    BYTE                cSlaveAddress;
    LPC_I2C_T           *psI2CRegCfg;
    IRQn_Type           eI2CIrq;
    eCOUNTER_TYPE       eTimer;

    StaticSemaphore_t   *pxSemaphoreBuffer;
    SemaphoreHandle_t   xSemaphore;
} sBOARD_I2C_CFG;

void Board_I2C_Init(eBOARD_I2C eBus);
void Board_I2C_DeInit(eBOARD_I2C eBus);
eRESULT Board_I2C_Master_Write(eBOARD_I2C eBus, BYTE cDeviceAddress, WORD wRegister, WORD wDataSz, BYTE *pcData, BYTE cFlag);
eRESULT Board_I2C_Master_Read(eBOARD_I2C eBus, BYTE cDeviceAddress, WORD wRegister, WORD wDataSz, BYTE *pcData, BYTE cFlag);

eRESULT Board_I2C_Slave_Write(eBOARD_I2C eBus, WORD wDataSz, BYTE *pcTxData);
eRESULT Board_I2C_Slave_Read(eBOARD_I2C eBus, WORD wDataSz, BYTE *pcRxData);

void Board_I2C_Enable_SlaveMode(eBOARD_I2C eBus);
void Board_I2C_Disable_SlaveMode(eBOARD_I2C eBus);
void Board_I2C_Slave_Buffer_Flush(eBOARD_I2C eBus);

#endif // BOARD_I2C_H


