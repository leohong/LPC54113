#ifndef MEMORYMAP_H
#define MEMORYMAP_H
// ===============================================================================
// FILE NAME: APP_MEMORY_MAP.H
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2019/10/07, Leo Create
// --------------------
// ===============================================================================

#include "Commondef.h"

#pragma pack(push, 1)     /* set alignment to 1 byte boundary */
typedef struct
{
    DWORD dwCodeVersion;
    DWORD dwCodeStartupAddress;
    DWORD dwCodeSize;
    DWORD dwCodeChecksum;
} sMEM_TAG_PARAM;

#define sMEM_TAG_PARAM_SIZE sizeof(sMEM_TAG_PARAM)/sizeof(BYTE)

#pragma pack(pop)     /* set alignment to 1 byte boundary */

#define BOOT_FIRMWARE_START             0x1A000000
#define ERASE_TAG_SIZE                  0x200       // 512 bytes
#define STAY_IN_BL_FLAG                 0x10007FF0  // Stay In BootLoader Flag

#define APP_TAG_ADDR                    0x1A008000  // 32k
#define APP_BOOT_TAG_ADDR               0x1A007800

#define APP_FIRMWARE_START              0x1A008200  // 32k offset 512 bytes

#endif /* MEMORYMAP_H */

