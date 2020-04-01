#ifndef BOARD_IAP_H
#define BOARD_IAP_H

// ===============================================================================
// FILE NAME: Board_IAP.h
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2014/06/30, Leo Create
// --------------------
// ===============================================================================


#include "CommonDef.h"

#define FRONT_SECTOR_SIZE   (8*1024)
#define REAR_SECTOR_SIZE    (64*1024)

#define BANK_A_START        0x1A000000
#define BANK_A_SIZE         0x80000

#define BANK_B_START        0x1B000000
#define BANK_B_SIZE         0x80000

#define BLOCK_8K_BOUND      (64*1024)

#define BLOCK_8K_NUMBER     8
#define BLOCK_64K_NUMBER    7

typedef struct
{
    BYTE cFlashId;
    BYTE cStartSector;
    BYTE cEndSector;
} sFLASH_INFO;

typedef enum
{
    eFLASH_BANK_A,
    eFLASH_BANK_B,

    eFLASH_BANK_NUMBERS,
} eFLASH_BANK;

DWORD Board_IAP_Initial(void);
DWORD Board_IAP_Erase(DWORD dwAddress, DWORD dwSize);
DWORD Board_IAP_Write(DWORD dwAddress, DWORD dwSize, BYTE *pcData);
eRESULT Board_IAP_Addr_To_Sector(sFLASH_INFO *psFlashInfo, DWORD dwAddress, DWORD dwSize);
UINT32 Board_IAP_PageErase(UINT32 dwAddress, UINT32 dwSize);

#endif /* BOARD_IAP_H */

