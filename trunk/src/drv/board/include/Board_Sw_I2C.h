#ifndef BOARD_SW_I2C_H
#define BOARD_SW_I2C_H
// ===============================================================================
// FILE NAME: Board_Sw_I2C.h
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2014/07/01, Leo Create
// --------------------
// ===============================================================================


#include "CommonDef.h"

void Board_I2C_SW_Speed(BYTE cBus, DWORD dwKhz);
void Board_I2C_SW_Reset(BYTE cBus);
eRESULT Board_I2C_SW_ReadAck(BYTE cBus);
eRESULT Board_I2C_SW_Start(BYTE cBus);
eRESULT Board_I2C_SW_SendData(BYTE cBus, BYTE cData, BOOL bAck);
eRESULT Board_I2C_SW_ReadData(BYTE cBus, BYTE *pcData, BOOL bNonAck);
eRESULT Board_I2C_SW_Stop(BYTE cBus);


#endif /* BOARD_SW_I2C_H */
