#ifndef BOARD_SPIFI_H
#define BOARD_SPIFI_H
// ===============================================================================
// FILE NAME: Board_Watchdog.h
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2015/03/19, Leo Create
// --------------------
// ===============================================================================


#include "CommonDef.h"

void Board_SPIFI_Init(void);
void Board_SPIFI_Read(unsigned int READ_ADDRESS, void *pvData, DWORD dwSize);
UINT32 Board_SPIFI_GetInfo_BlockNumber(void);
UINT32 Board_SPIFI_GetInfo_BlockSize(void);
UINT32 Board_SPIFI_GetInfo_PageSize(void);
UINT32 Board_SPIFI_GetInfo_Address(UINT32 ulNumberBlocks);
void Board_SPIFI_MEMMode_Set(BOOL bEnable);
UINT8 Board_SPIFI_EraseBlocks(UINT32 ulFirstBlock, UINT32 ulNumberBlocks);
UINT8 Board_SPIFI_Write(UINT32 ulAddr, UINT32 ulSize, UINT8 *pucData);

#endif /* BOARD_SPIFI_H */

