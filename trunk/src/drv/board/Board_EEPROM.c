// ==============================================================================
// FILE NAME: BOARD_EEPROM.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 23/12/2013, Leo Create
// --------------------
// ==============================================================================


#include "Board_EEPROM.h"
#include "utilLogAPI.h"

#define EEPROM_WORD_SIZE    (4)
#define WORD_ALIGN(x)       ((((x) % 4) == 0) ? (x) : (x) + (4 - ((x) % 4)))

// ==============================================================================
// FUNCTION NAME: BOARD_EEP_INIT
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 23/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_EEPROM_Init(void)
{
    Chip_RGU_TriggerReset(RGU_EEPROM_RST);

    while(Chip_RGU_InReset(RGU_EEPROM_RST)) {}

    /* Init EEPROM */
    Chip_EEPROM_Init(LPC_EEPROM);

    /* Set Auto Programming mode */
    Chip_EEPROM_SetAutoProg(LPC_EEPROM, EEPROM_AUTOPROG_OFF);
}

// ==============================================================================
// FUNCTION NAME: BOARD_EEP_WRITE
// DESCRIPTION:
//
//
// Params:
// WORD wStartAdd:
// WORD wDataSz:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
// 23/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_EEPROM_Write(WORD wStartAdd, WORD wDataSz, BYTE *pcData)
{
    DWORD adwEeepData[32];
    WORD wFirstPage = 0;
    WORD wMidPage = 0;
    WORD wLastPage = 0;
    WORD wPagePremainder = 0;
    WORD wPageRemainder = 0;
    WORD wEndAddr = 0;

    wEndAddr = wStartAdd + wDataSz - 1;

    wFirstPage = wStartAdd / EEPROM_PAGE_SIZE;
    wLastPage = wEndAddr / EEPROM_PAGE_SIZE;

    // 大小在1 Page內
    if(wFirstPage == wLastPage)
    {
        wPagePremainder = wDataSz;
        wPageRemainder = 0;
        wMidPage = 0;
    }
    else
    {
        wPagePremainder = (wFirstPage + 1) * EEPROM_PAGE_SIZE - wStartAdd;

        if((wEndAddr + 1) % EEPROM_PAGE_SIZE)
        {
            // Remainder is the number of bytes after the last page boundary
            wPageRemainder = wEndAddr - (wLastPage * EEPROM_PAGE_SIZE) + 1;
        }
        else
        {
            // Tranfer ends at a page boundary so no remainder
            wPageRemainder = 0;
        }
    }

    wMidPage = (wDataSz - wPagePremainder - wPageRemainder) / EEPROM_PAGE_SIZE;

    if(wPagePremainder)
    {
        WORD    wNewStartAddr = (wStartAdd % EEPROM_PAGE_SIZE);
        DWORD   *pdwEepromMem = 0;
        WORD    wCount = 0;
        BYTE    *pcByte = NULL;

        pdwEepromMem = (DWORD *)EEPROM_ADDRESS(wFirstPage, 0);

        // 取出1 Page Data
        memcpy(adwEeepData, (BYTE *)pdwEepromMem, EEPROM_PAGE_SIZE);

        pcByte = (BYTE *)adwEeepData;

        // 更新有改變的byte
        for(wCount = 0; wCount <= wPagePremainder; wCount++)
        {
            pcByte[wNewStartAddr + wCount] = *pcData++;
        }

        // TO_DO
        for(wCount = (wNewStartAddr / 4); wCount <= ((wNewStartAddr + wPagePremainder) / 4); wCount++)
        {
            pdwEepromMem[wCount] = adwEeepData[wCount];
        }

        Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
    }

    if(wMidPage)
    {
        DWORD   *pdwEepromMem = 0;
        WORD    wCount = 0;
        WORD    wWordCount = 0;
        BYTE    *pcByte = NULL;

        for(wCount = 0; wCount < wMidPage; wCount++)
        {
            pdwEepromMem = (DWORD *)EEPROM_ADDRESS((wFirstPage + 1 + wCount), 0);

            // 取出1 Page Data
            memcpy(adwEeepData, (BYTE *)pdwEepromMem, EEPROM_PAGE_SIZE);

            pcByte = (BYTE *)adwEeepData;

            for(wCount = 0; wCount < EEPROM_PAGE_SIZE; wCount++)
            {
                pcByte[wCount] = *(pcData++);
            }

            for(wWordCount = 0; wWordCount < (EEPROM_PAGE_SIZE / EEPROM_WORD_SIZE); wWordCount++)
            {
                pdwEepromMem[wWordCount] = adwEeepData[wWordCount];
            }

            Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
        }
    }

    if(wPageRemainder)
    {
        WORD    wNewDataSz = wPageRemainder;
        DWORD   *pdwEepromMem = 0;
        WORD    wCount = 0;
        BYTE    *pcByte = NULL;

        pdwEepromMem = (DWORD *)EEPROM_ADDRESS(wLastPage, 0);

        // 取出1 Page Data
        memcpy(adwEeepData, (BYTE *)pdwEepromMem, EEPROM_PAGE_SIZE);

        pcByte = (BYTE *)adwEeepData;

        for(wCount = 0; wCount < wNewDataSz; wCount++)
        {
            pcByte[wCount] = *pcData++;
        }

        for(wCount = 0; wCount <= (wNewDataSz / 4); wCount++)
        {
            pdwEepromMem[wCount] = adwEeepData[wCount];
        }

        Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
    }
}


// ==============================================================================
// FUNCTION NAME: BOARD_EEP_READ
// DESCRIPTION:
//
//
// Params:
// WORD wStartAdd:
// WORD wDataSz:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
// 23/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_EEPROM_Read(WORD wStartAdd, WORD wDataSz, BYTE *pcData)
{
    memcpy(pcData, (BYTE *)(EEPROM_ADDRESS(0, wStartAdd)), wDataSz);
}

// ==============================================================================
// FUNCTION NAME: BOARD_EEPROM_ERASE
// DESCRIPTION:
//
//
// Params:
// WORD wStartAdd:
// WORD wDataSz:
//
// Returns:
//
//
// modification history
// --------------------
// 23/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_EEPROM_Erase(WORD wStartAdd, WORD wDataSz)
{
    DWORD adwEeepData[32];
    WORD wFirstPage = 0;
    WORD wMidPage = 0;
    WORD wLastPage = 0;
    WORD wPagePremainder = 0;
    WORD wPageRemainder = 0;
    WORD wEndAddr = 0;

    wEndAddr = wStartAdd + wDataSz - 1;

    wFirstPage = wStartAdd / EEPROM_PAGE_SIZE;
    wLastPage = wEndAddr / EEPROM_PAGE_SIZE;

    // 大小在1 Page內
    if(wFirstPage == wLastPage)
    {
        wPagePremainder = wDataSz;
        wPageRemainder = 0;
        wMidPage = 0;
    }
    else
    {
        wPagePremainder = (wFirstPage + 1) * EEPROM_PAGE_SIZE - wStartAdd;

        if((wEndAddr + 1) % EEPROM_PAGE_SIZE)
        {
            // Remainder is the number of bytes after the last page boundary
            wPageRemainder = wEndAddr - (wLastPage * EEPROM_PAGE_SIZE) + 1;
        }
        else
        {
            // Tranfer ends at a page boundary so no remainder
            wPageRemainder = 0;
        }
    }

    wMidPage = (wDataSz - wPagePremainder - wPageRemainder) / EEPROM_PAGE_SIZE;

    if(wPagePremainder)
    {
        WORD    wNewStartAddr = (wStartAdd % EEPROM_PAGE_SIZE);
        DWORD   *pdwEepromMem = 0;
        WORD    wCount = 0;
        BYTE    *pcByte = NULL;

        pdwEepromMem = (DWORD *)EEPROM_ADDRESS(wFirstPage, 0);

        // 取出1 Page Data
        memcpy(adwEeepData, (BYTE *)pdwEepromMem, EEPROM_PAGE_SIZE);

        pcByte = (BYTE *)adwEeepData;

        // 更新有改變的byte
        for(wCount = 0; wCount <= wPagePremainder; wCount++)
        {
            pcByte[wNewStartAddr + wCount] = 0;
        }

        // TO_DO
        for(wCount = (wNewStartAddr / 4); wCount <= ((wNewStartAddr + wPagePremainder) / 4); wCount++)
        {
            pdwEepromMem[wCount] = adwEeepData[wCount];
        }

        Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
    }

    if(wMidPage)
    {
        DWORD   *pdwEepromMem = 0;
        WORD    wCount = 0;
        WORD    wWordCount = 0;
        BYTE    *pcByte = NULL;

        for(wCount = 0; wCount < wMidPage; wCount++)
        {
            pdwEepromMem = (DWORD *)EEPROM_ADDRESS((wFirstPage + 1 + wCount), 0);

            // 取出1 Page Data
            memcpy(adwEeepData, (BYTE *)pdwEepromMem, EEPROM_PAGE_SIZE);

            pcByte = (BYTE *)adwEeepData;

            for(wCount = 0; wCount < EEPROM_PAGE_SIZE; wCount++)
            {
                pcByte[wCount] = 0;
            }

            for(wWordCount = 0; wWordCount < (EEPROM_PAGE_SIZE / EEPROM_WORD_SIZE); wWordCount++)
            {
                pdwEepromMem[wWordCount] = adwEeepData[wWordCount];
            }

            Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
        }
    }

    if(wPageRemainder)
    {
        WORD    wNewDataSz = wPageRemainder;
        DWORD   *pdwEepromMem = 0;
        WORD    wCount = 0;
        BYTE    *pcByte = NULL;

        pdwEepromMem = (DWORD *)EEPROM_ADDRESS(wLastPage, 0);

        // 取出1 Page Data
        memcpy(adwEeepData, (BYTE *)pdwEepromMem, EEPROM_PAGE_SIZE);

        pcByte = (BYTE *)adwEeepData;

        for(wCount = 0; wCount < wNewDataSz; wCount++)
        {
            pcByte[wCount] = 0;
        }

        for(wCount = 0; wCount <= (wNewDataSz / 4); wCount++)
        {
            pdwEepromMem[wCount] = adwEeepData[wCount];
        }

        Chip_EEPROM_EraseProgramPage(LPC_EEPROM);
    }
}




