#ifndef APPIAPPROCAPI_H
#define APPIAPPROCAPI_H
// ===============================================================================
// FILE NAME: appIAPProcAPI.h
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2014/07/01, Leo Create
// --------------------
// ===============================================================================
//LPC54113_JS_0048 modify//LPC54113_JS_0046 modify

#include "CommonDef.h"

typedef void (*BOOT_CODE_ENTRY)();

// 宣告一個固定位址的變數,存放Jump form App Tag
extern DWORD g_dwStayInBootLoaderFlag;

typedef enum
{
// Common Command
/* 00 */eIAP_CMD_IDLE           = 0,    // Check Busy
/* 01 */eIAP_CMD_VERSION        = 1,
/* 02 */eIAP_CMD_GO_BOOTLOADER  = 2,

/* 03 */eIAP_CMD_BL_VER         = 3,   // Bootloader Version
/* 04 */eIAP_CMD_APP_VER,              // App Code Version
/* 05 */eIAP_CMD_RUN_APP,              // Run App Code
/* 06 */eIAP_CMD_IAP_ENABLE,           // Enable IAP Process
/* 07 */eIAP_CMD_APP_INFO,             // Start Address and Size of Bin File
/* 08 */eIAP_CMD_BIN_ADDRESS,          // Address of 1K buffer of Bin File
/* 09 */eIAP_CMD_BIN_DATA,             // Send Bin Data
/* 10 */eIAP_CMD_PROGRAMING_FINISH,    // Programming Finish
/* 11 */eIAP_CMD_BIN_CHECK_SUM,        // Bin File Check Sum
/* 12 */eIAP_CMD_PROGRAMING,           // Program Bin Data to Flash
/* 13 */eIAP_CMD_APP_CODE_READBACK,    // Read App code
/* 14 */eIAP_CMD_LOG,                  // Enable debug log

        eIAP_CMD_NUMBERS,
} eIAP_CMD;

typedef enum
{
// Common Command
    eIAP_UPG_OFF   = 0,
    eIAP_UPG_START = 1,
    eIAP_UPG_IAP   = 2,
    eIAP_UPG_BL    = 3,

    eIAP_UPG_NUMBERS,
} eIAP_UPG_STATUS;

#define BIN_PAGE_BYTE_SIZE  (1024)
#define BIN_PAGE_SIZE       (BIN_PAGE_BYTE_SIZE/4)

typedef struct
{
    BYTE    cNotEmpty;
    DWORD   dwAddress;
    DWORD   dwPageData[BIN_PAGE_SIZE];
    DWORD   dwChecksum;
} sPAGE_DATA;

//void appIapProc_Run_BL_Set(BYTE cEnable);
//BYTE appIapProc_Run_BL_Get(void);
//void appIapProc_GoTo_BL_Check(void);
void appIapProc_Run_BootCode_Set(void);



#endif /* APPIAPPROCAPI_H */

