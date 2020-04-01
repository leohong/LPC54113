// ===============================================================================
// FILE NAME: appIAPProc.c
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2014/07/01, Leo Create
// --------------------
// ===============================================================================
//LPC54113_JS_0048 modify//LPC54113_JS_0046 modify

#include "Release.h"
#include "Board.h"
#include "MemoryMap.h"
#include "appIapProcAPI.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"
#include "Board_uart.h"                                     //LPC54113_JS_0055 add
#include "Board_I2C.h"		//T100_Larry_0005
#include "Board_MCPWM.h"	//T100_Larry_0005

// 宣告一個固定位址的變數, 存放Jump form App Tag
DWORD g_dwStayInBootLoaderFlag __attribute__((at(STAY_IN_BL_FLAG)));

// Total ROM Size (Code + RO Data + RW Data)
// 引入Scatter file中section name
// extern unsigned int Image$$LR_IROM1$$Length;

// 利用linker將App Code Version放入固定位置
const sMEM_TAG_PARAM m_sAppMemParam __attribute__((at(APP_TAG_ADDR))) =
{
    (VER_MAJOR<<8)|(VER_MINOR),             // Code Fw Version
    APP_FIRMWARE_START,                     // Code Start Address
    0,//(DWORD)&(Image$$LR_IROM1$$Length),      // Code Size
    0,                                      // Code Checksum
};

// ==============================================================================
// FUNCTION NAME: appIapProc_BootLoader_Version_Get
// DESCRIPTION:
//
//
// Params:
// BYTE *pcVersion:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/02, Leo Create
// --------------------
// ==============================================================================
void appIapProc_BootLoader_Version_Get(BYTE *pcVersion)
{
    pcVersion[0] = VER_MINOR;
    pcVersion[1] = VER_MAJOR;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Run_BootCode_Set
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
// 2014/07/02, Leo Create
// --------------------
// ==============================================================================
#pragma O0
void appIapProc_Run_BootCode_Set(void)
{
    BOOT_CODE_ENTRY fpBootEntryPoint;

    Board_Goto_BL_DeInit();

    __disable_irq();
    memset((uint32_t *)NVIC->ICER, 0xFF, sizeof(NVIC->ICER));
    memset((uint32_t *)NVIC->ICPR, 0xFF, sizeof(NVIC->ICPR));

    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

    __set_MSP(*((DWORD*)(BOOT_FIRMWARE_START))); // Reload Stack point
    __set_CONTROL(0);

    // Appcode jump flag
    g_dwStayInBootLoaderFlag = 0x12345678;

    fpBootEntryPoint = (BOOT_CODE_ENTRY)*((DWORD*)(BOOT_FIRMWARE_START + 4));
    (fpBootEntryPoint)();
    //NVIC_SystemReset();
}
