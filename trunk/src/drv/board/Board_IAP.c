// ===============================================================================
// FILE NAME: Board_IAP.C
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2014/06/30, Leo Create
// --------------------
// ===============================================================================


#include "Board_IAP.h"
#include "utilLogAPI.h"

#ifdef __DEBUG__
static const char *m_apIAP_String[] =
{
    {"Command is executed successfully"},
    {"Invalid command"},
    {"Source address is not on word boundary"},
    {"Destination address is not on a correct boundary"},
    {"Source address is not mapped in the memory map"},
    {"Destination address is not mapped in the memory map"},
    {"Byte count is not multiple of 4 or is not a permitted value"},
    {"Sector number is invalid or end sector number is greater than start sector number"},
    {"Sector is not blank"},
    {"Command to prepare sector for write operation was not executed"},
    {"Source and destination data not equal"},
    {"Flash programming hardware interface is busy"},
    {"nsufficient number of parameters or invalid parameter"},
    {"Address is not on word boundary"},
    {"Address is not mapped in the memory map"},
    {"Command is locked"},
    {"Unlock code is invalid"},
    {"Invalid baud rate setting"},
    {"Invalid stop bit setting"},
    {"Code read protection enabled"},
};
#endif /* __DEBUG__ */

// ==============================================================================
// FUNCTION NAME: Board_IAP_Addr_To_Sector
// DESCRIPTION:
//
//
// Params:
// DWORD dwAddress:
// DWORD dwSize:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/02, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_IAP_Addr_To_Sector(sFLASH_INFO *psFlashInfo, DWORD dwAddress, DWORD dwSize)
{
    DWORD dwFlashStart = dwAddress & 0xFF000000;
    memset((BYTE *)psFlashInfo, 0, sizeof(sFLASH_INFO) / sizeof(BYTE));

    dwSize = dwSize - 1;

    // 檢查Address Bank and Range
    if((BANK_A_SIZE > dwSize) && (dwFlashStart == BANK_A_START))
    {
        psFlashInfo->cFlashId = eFLASH_BANK_A;
    }
    else if((BANK_B_SIZE > dwSize) && ((dwFlashStart == BANK_B_START)))
    {
        psFlashInfo->cFlashId = eFLASH_BANK_B;
    }
    else
    {
        return rcERROR;
    }

    // 取出Address
    dwAddress = 0x00FFFFFF & dwAddress;

    // 是否有跨 8K/64K Sector邊界
    if(BLOCK_8K_BOUND > dwAddress)
    {
        // 計算Start Sector
        psFlashInfo->cStartSector = (dwAddress) / (FRONT_SECTOR_SIZE);

        // 計算End Sector
        if(BLOCK_8K_BOUND > (dwAddress + dwSize))
        {
            psFlashInfo->cEndSector = (dwAddress + dwSize) / (FRONT_SECTOR_SIZE);
        }
        else
        {
            dwSize = dwAddress + dwSize - BLOCK_8K_BOUND;
            psFlashInfo->cEndSector = (dwSize / REAR_SECTOR_SIZE) + BLOCK_8K_NUMBER;
        }
    }
    else
    {
        psFlashInfo->cStartSector = (dwAddress - BLOCK_8K_BOUND) / REAR_SECTOR_SIZE + BLOCK_8K_NUMBER;
        dwSize = dwAddress + dwSize - BLOCK_8K_BOUND;
        psFlashInfo->cEndSector = (dwSize / REAR_SECTOR_SIZE) + BLOCK_8K_NUMBER;
    }

    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: Board_IAP_Initial
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
// 2014/06/30, Leo Create
// --------------------
// ==============================================================================
DWORD Board_IAP_Initial(void)
{
    DWORD command[5], result[4];
    command[0] = 49;
    iap_entry(command, result);

    return 0;
}

// ==============================================================================
// FUNCTION NAME: Board_IAP_Erase
// DESCRIPTION:
//
//
// Params:
// DWORD dwAddress:
// DWORD dwSize:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/02, Leo Create
// --------------------
// ==============================================================================
DWORD Board_IAP_Erase(DWORD dwAddress, DWORD dwSize)
{
    sFLASH_INFO sFlashInfo;
    BYTE cResult = 0;

    LOG_MSG(eDB_MSK_LIST_IAP, "Erase\r\n");
    LOG_MSG(eDB_MSK_LIST_IAP, "Address = 0x%02X, Size=%d\r\n", dwAddress, dwSize);

    if(rcSUCCESS == Board_IAP_Addr_To_Sector(&sFlashInfo, dwAddress, dwSize))
    {
        LOG_MSG(eDB_MSK_LIST_IAP, "StartSector = %d, EndSector=%d\r\n", sFlashInfo.cStartSector, sFlashInfo.cEndSector);
        cResult = Chip_IAP_PreSectorForReadWrite(sFlashInfo.cStartSector, sFlashInfo.cEndSector, sFlashInfo.cFlashId);

        LOG_MSG(eDB_MSK_LIST_IAP, "%s\r\n", m_apIAP_String[cResult]);

        if(0 == cResult)
        {
            cResult = Chip_IAP_EraseSector(sFlashInfo.cStartSector, sFlashInfo.cEndSector, sFlashInfo.cFlashId);
            LOG_MSG(eDB_MSK_LIST_IAP, "%s\r\n", m_apIAP_String[cResult]);
        }
    }

    return cResult;
}

// ==============================================================================
// FUNCTION NAME: Board_IAP_Write
// DESCRIPTION:
//
//
// Params:
// DWORD dwAddress:
// DWORD dwSize:
// BYTE *pcData:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/02, Leo Create
// --------------------
// ==============================================================================
DWORD Board_IAP_Write(DWORD dwAddress, DWORD dwSize, BYTE *pcData)
{
    sFLASH_INFO sFlashInfo;
    BYTE cResult = 0;

    LOG_MSG(eDB_MSK_LIST_IAP, "Write\r\n");
    LOG_MSG(eDB_MSK_LIST_IAP, "Address = 0x%02X, Size=%d\r\n", dwAddress, dwSize);

    if(rcSUCCESS == Board_IAP_Addr_To_Sector(&sFlashInfo, dwAddress, dwSize))
    {
        LOG_MSG(eDB_MSK_LIST_IAP, "StartSector = %d, EndSector=%d\r\n", sFlashInfo.cStartSector, sFlashInfo.cEndSector);
        cResult = Chip_IAP_PreSectorForReadWrite(sFlashInfo.cStartSector, sFlashInfo.cEndSector, sFlashInfo.cFlashId);

        LOG_MSG(eDB_MSK_LIST_IAP, "%s\r\n", m_apIAP_String[cResult]);

        if(0 == cResult)
        {
            cResult = Chip_IAP_CopyRamToFlash(dwAddress, (DWORD *)pcData, dwSize);
            LOG_MSG(eDB_MSK_LIST_IAP, "%s\r\n", m_apIAP_String[cResult]);
        }
    }

    return cResult;
}

// ==============================================================================
// FUNCTION NAME: Board_IAP_PageErase
// DESCRIPTION:
//
//
// Params:
// UINT32 dwAddress:
// UINT32 dwSize:
//
// Returns:
//
//
// Modification History
// --------------------
// 2018/03/28, Larry Create
// --------------------
// ==============================================================================
UINT32 Board_IAP_PageErase(UINT32 dwAddress, UINT32 dwSize)
{
    sFLASH_INFO sFlashInfo;
    UINT8 cResult = 0;

    LOG_MSG(eDB_MSK_LIST_IAP, "Erase\r\n");
    LOG_MSG(eDB_MSK_LIST_IAP, "Address = 0x%02X\r\n", dwAddress);

    if(rcSUCCESS == Board_IAP_Addr_To_Sector(&sFlashInfo, dwAddress, dwSize))
    {
        LOG_MSG(eDB_MSK_LIST_IAP, "StartSector = %d, EndSector = %d\r\n", sFlashInfo.cStartSector, sFlashInfo.cEndSector);

        __disable_irq();
        cResult = Chip_IAP_PreSectorForReadWrite(sFlashInfo.cStartSector, sFlashInfo.cEndSector, sFlashInfo.cFlashId);
        __enable_irq();

        LOG_MSG(eDB_MSK_LIST_IAP, "%s\r\n", m_apIAP_String[cResult]);

        if(0 == cResult)
        {
            __disable_irq();
            cResult = Chip_IAP_ErasePage(dwAddress, dwAddress);
            __enable_irq();

            LOG_MSG(eDB_MSK_LIST_IAP, "%s\r\n", m_apIAP_String[cResult]);
        }
    }

    return cResult;
}

