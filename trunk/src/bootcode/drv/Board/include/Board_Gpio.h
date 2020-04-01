#ifndef BOARD_GPIO_H
#define BOARD_GPIO_H
// ==============================================================================
// FILE NAME: BOARD_GPIO.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 23/01/2013, Leohong written
// --------------------
// ==============================================================================

#include "Commondef.h"
#include "Board_GPIO_Table.h"

void Board_Gpio_Init(void);
void Board_Gpio_PinMux(sPIN_CFG *psPinCfg, WORD wPinNumber);
void Board_Gpio_Clock_Set(sPIN_CLK_CFG *psPinCfg, WORD wPinNumber);
void Board_Gpio_Set(eBOARD_PIN_TABLE ePin, BYTE cValue);
BOOL Board_Gpio_Get(eBOARD_PIN_TABLE ePin);
void Board_Gpio_Toggle_Set(eBOARD_PIN_TABLE ePin);
void Board_Gpio_Dir_Set(eBOARD_PIN_TABLE ePin, eIO_MODE eMode);

void Board_Gpio_Gima_PinMux(sGIMA_PIN_CFG *psGimaCfg, WORD wPinNumber);
void Board_Gpio_IntPin_Select(sGPIO_INT_CFG *psIntPinTable, WORD wPinNumber);
void Board_GpioInt_Init(eBOARD_PIN_TABLE eIntPin, eINT_MODE eMode, eINT_EDGE eEdge);
void Board_GpioInt_Register_RiseCallback(eBOARD_PIN_TABLE eIntPin, void (*fpCallback)(void));
void Board_GpioInt_Register_FallCallback(eBOARD_PIN_TABLE eIntPin, void (*fpCallback)(void));
void Board_Gpio_Dump_IO_Cfg(void);
void Board_Gpio_Dump_Pin_Value(void);
void Board_GpioInt_DeInit(void);
#endif // BOARD_GPIO_H

