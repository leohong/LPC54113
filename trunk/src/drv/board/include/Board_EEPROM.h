#ifndef BOARD_EEPROM_H
#define BOARD_EEPROM_H
// ==============================================================================
// FILE NAME: BOARD_EEPROM.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 23/12/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"

void Board_EEPROM_Init(void);
void Board_EEPROM_Write(WORD wStartAdd, WORD wDataSz, BYTE *pcData);
void Board_EEPROM_Read(WORD wStartAdd, WORD wDataSz, BYTE *pcData);
void Board_EEPROM_Erase(WORD wStartAdd, WORD wDataSz);

#endif /* BOARD_EEPROM_H */

