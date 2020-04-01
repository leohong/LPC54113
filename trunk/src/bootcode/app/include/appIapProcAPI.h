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


#include "CommonDef.h"

typedef void (*APP_CODE_ENTRY)();

typedef enum
{
// Common Command
/* 00 */eIAP_CMD_IDLE               = 0,    // Check Busy
/* 01 */eIAP_CMD_VERSION            = 1,
/* 02 */eIAP_CMD_GO_TO_BOOTLOADER   = 2,

/* 03 */eIAP_CMD_BL_VER             = 3,    // Bootloader Version
/* 04 */eIAP_CMD_APP_VER,                   // App Code Version
/* 05 */eIAP_CMD_RUN_APP,                   // Run App Code
/* 06 */eIAP_CMD_IAP_ENABLE,                // Enable IAP Process
/* 07 */eIAP_CMD_APP_INFO,                  // Start Address and Size of Bin File
/* 08 */eIAP_CMD_BIN_ADDRESS,               // Address of 1K buffer of Bin File
/* 09 */eIAP_CMD_BIN_DATA,                  // Send Bin Data
/* 10 */eIAP_CMD_PROGRAMING_FINISH,         // Programming Finish
/* 11 */eIAP_CMD_BIN_CHECK_SUM,             // Bin File Check Sum
/* 12 */eIAP_CMD_PROGRAMING,                // Program Bin Data to Flash
/* 13 */eIAP_CMD_APP_CODE_READBACK,         // Read App code
/* 14 */eIAP_CMD_LOG,                       // Enable debug log

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

typedef enum
{
    eIAP_APP_CHK_PASS = 0,
    eIAP_APP_UPG,
    eIAP_APP_CHK_FAIL,
    eIAP_APP_RECHK_FAIL,

    eIAP_APP_NUMBERS,
} eIAP_APP_STATUS;

#define BIN_PAGE_BYTE_SIZE  (1024)
#define BIN_PAGE_SIZE       (BIN_PAGE_BYTE_SIZE/4)

typedef struct
{
    BYTE    cNotEmpty;
    DWORD   dwAddress;
    DWORD   adwPageData[BIN_PAGE_SIZE];
    DWORD   dwChecksum;
} sPAGE_DATA;

void appIapProc_Init(void);
void appIapProc_Task(void *pParameters);
eRESULT appIapProc_IAP_Enable(void);
BYTE appIapProc_IAP_Enable_Get(void);
void appIapProc_AppCode_Version_Get(DWORD *pdwVersion);
void appIapProc_AppCode_Version_Set(DWORD *pdwVersion);

eRESULT appIapProc_AppBinInfo_Set(DWORD *pdwStartAdd, DWORD *pdwSize);
eRESULT appIapProc_Bin_Address(DWORD *pdwAddress);
void appIapProc_Bin_Address_Get(DWORD *pdwAddress);

eRESULT appIapProc_Bin_Data(BYTE *pcData);

void appIapProc_CheckSum_Get(DWORD *pdwCheckSum);

eRESULT appIapProc_Programing(DWORD *pdwAddress);
eRESULT appIapProc_Programing_Finish(DWORD *pdwVersion, DWORD *pwdCompany);

void appIapProc_Run_App_Flag_Set(BYTE cEnable);
void appIapProc_Run_AppCode_Set(void);
void appIapProc_GoTo_App_Check(void);
void appIapProc_Task(void *pParameters);


#endif /* APPIAPPROCAPI_H */

