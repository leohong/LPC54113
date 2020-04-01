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


#include "Release.h"
#include "Board.h"
#include "Board_GPIO.h"
#include "Board_Iap.h"
#include "Board_uart.h"
#include "utilCounterAPI.h"
#include "utilEventHandlerAPI.h"
#include "appIapProcAPI.h"
#include "utilLogAPI.h"
#include "utilHostAPI.h"
#include "MemoryMap.h"


// 宣告一個固定位址的變數, 存放Jump form App Tag
DWORD g_dwStayInBootLoaderFlag __attribute__((at(STAY_IN_BL_FLAG)));
#define JUMP_FROM_APP   ((0x12345678 == g_dwStayInBootLoaderFlag)? 1 : 0 )

static BYTE m_cIapEnable = FALSE;
volatile static BYTE m_cRunAppCode = FALSE;
volatile static sMEM_TAG_PARAM m_sAppMemParam;
volatile static sPAGE_DATA m_sPageData;
volatile static DWORD m_dwCounterMain;
volatile static DWORD m_dwLastCheckSum;
volatile static DWORD m_dwLastAddress;;

/* Structure that will hold the TCB of the task being created. */
StaticTask_t m_xIapMainTaskTCB;

/* Buffer that the task being created will use as its stack. */
StackType_t m_axIapMainStack[IAP_TASK_STACK_SZ];

//#define IAP_DEBUG

// ==============================================================================
// FUNCTION NAME: appIapProc_IAP_Enable
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
eRESULT appIapProc_IAP_Enable(void)
{
    eRESULT eResult = rcERROR;

    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "Enable:%X %d\n", m_sAppMemParam.dwCodeStartupAddress, m_sAppMemParam.dwCodeSize);
    if((m_sAppMemParam.dwCodeStartupAddress != 0xFFFFFFFF) && (m_sAppMemParam.dwCodeSize != 0))
    {
#ifdef IAP_DEBUG
        eResult = rcSUCCESS;
#else
        eResult = Board_IAP_Initial();
        eResult |= Board_IAP_Erase(APP_TAG_ADDR, ERASE_TAG_SIZE);
        eResult |= Board_IAP_Erase(m_sAppMemParam.dwCodeStartupAddress, m_sAppMemParam.dwCodeSize);
#endif // IAP_DEBUG

        if(rcSUCCESS == eResult)
        {
            m_cIapEnable = TRUE;
        }
        else
        {
            m_cIapEnable = FALSE;
        }
    }
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "IAP_Enable:%d\n", eResult);

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_IAP_Enable_Get
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
BYTE appIapProc_IAP_Enable_Get(void)
{
    return m_cIapEnable;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_AppCode_Version_Get
// DESCRIPTION:
//
//
// Params:
// DWORD *pdwVersion:
//
// Returns:
//
//
// Modification History
// --------------------
// 2019/08/02, Leo Create
// --------------------
// ==============================================================================
void appIapProc_AppCode_Version_Get(DWORD *pdwVersion)
{
    *pdwVersion = m_sAppMemParam.dwCodeVersion;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_AppCode_Version_Set
// DESCRIPTION:
//
//
// Params:
// DWORD *pdwVersion:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/20, Leo Create
// --------------------
// ==============================================================================
void appIapProc_AppCode_Version_Set(DWORD *pdwVersion)
{
    m_sAppMemParam.dwCodeVersion = *pdwVersion;
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "Vers:%d\n", m_sAppMemParam.dwCodeVersion);
}

// ==============================================================================
// FUNCTION NAME: appIapProc_AppBinInfo_Set
// DESCRIPTION:
//
//
// Params:
// DWORD *pdwStartAdd:
// DWORD *pdwSize:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/02, Leo Create
// --------------------
// ==============================================================================
eRESULT appIapProc_AppBinInfo_Set(DWORD *pdwStartAdd, DWORD *pdwSize)
{
    eRESULT eResult = rcERROR;

    // 檢查Address Bank and Range
    if(((BANK_A_START + BANK_A_SIZE) > *pdwStartAdd) || ((BANK_B_START + BANK_B_SIZE) > *pdwStartAdd))
    {
        m_sAppMemParam.dwCodeStartupAddress = *pdwStartAdd;

        eResult = rcSUCCESS;
    }

    if(*pdwSize > BANK_A_SIZE)
    {
        return rcERROR;
    }

    m_sAppMemParam.dwCodeSize = *pdwSize;
    m_sAppMemParam.dwCodeChecksum = 0;

    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "Info:%X %d\n", m_sAppMemParam.dwCodeStartupAddress, m_sAppMemParam.dwCodeSize);

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Bin_Address
// DESCRIPTION:
//
//
// Params:
// DWORD *pdwAddress:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/02, Leo Create
// --------------------
// ==============================================================================
eRESULT appIapProc_Bin_Address(DWORD *pdwAddress)
{
    m_sPageData.dwAddress = *pdwAddress;

    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Bin_Address_Get
// DESCRIPTION:
//
//
// Params:
// DWORD *pdwAddress:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/02, Leo Create
// --------------------
// ==============================================================================
void appIapProc_Bin_Address_Get(DWORD *pdwAddress)
{
    *pdwAddress = m_sPageData.dwAddress;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Bin_Data
// DESCRIPTION:
//
//
// Params:
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
eRESULT appIapProc_Bin_Data(BYTE *pcData)
{
    DWORD dwCount;
    BYTE *pcBuffer = NULL;

    m_sPageData.dwAddress = 0;
    m_sPageData.cNotEmpty = TRUE;
    m_sPageData.dwChecksum = 0;

    pcBuffer = (BYTE*)m_sPageData.adwPageData;

    for(dwCount = 0; dwCount < BIN_PAGE_BYTE_SIZE; dwCount++)
    {
        pcBuffer[dwCount] = pcData[dwCount];
        m_sPageData.dwChecksum += pcData[dwCount];
    }

    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "D:%X, %X\n", m_sPageData.dwAddress, m_sPageData.dwChecksum);
    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_CheckSum_Get
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
// 2014/08/18, Leo Create
// --------------------
// ==============================================================================
void appIapProc_CheckSum_Get(DWORD *pdwCheckSum)
{
    *pdwCheckSum = (~m_sPageData.dwChecksum + 1);
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Page_Program
// DESCRIPTION:
//
//
// Params:
// DWORD *pdwAddress:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/08/18, Leo Create
// --------------------
// ==============================================================================
eRESULT appIapProc_Programing(DWORD *pdwAddress)
{
    eRESULT eResult = rcBUSY;
    DWORD dwDataCount = BIN_PAGE_BYTE_SIZE;
    BYTE *pcDataStart = (BYTE *)m_sPageData.adwPageData;

    if(*pdwAddress != 0xFFFFFFFF)
    {
        appIapProc_Bin_Address(pdwAddress);
    }

    if(APP_TAG_ADDR == (*pdwAddress))
    {
        memset(pcDataStart, 0xFF, ERASE_TAG_SIZE); // Erease the App Tag Data
        dwDataCount = BIN_PAGE_BYTE_SIZE - ERASE_TAG_SIZE;
        pcDataStart = &pcDataStart[ERASE_TAG_SIZE];
        m_sPageData.dwChecksum = 0;

        while(dwDataCount--)
        {
            m_sPageData.dwChecksum += *(pcDataStart++);
        }
    }

    // To avoid add the same checksum twice
    if((*pdwAddress != m_dwLastAddress)||(m_sPageData.dwChecksum != m_dwLastCheckSum))
    {
        m_dwLastAddress = *pdwAddress;
        m_dwLastCheckSum = m_sPageData.dwChecksum;
        m_sAppMemParam.dwCodeChecksum += m_sPageData.dwChecksum;
    }

    if((TRUE == m_cIapEnable)&&(TRUE == m_sPageData.cNotEmpty))
    {
#ifdef IAP_DEBUG
        eResult = rcSUCCESS;
#else
        eResult = Board_IAP_Write(m_sPageData.dwAddress, (DWORD)BIN_PAGE_BYTE_SIZE, (BYTE*)m_sPageData.adwPageData);
#endif // IAP_DEBUG
    }

    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "P:%X, %X\n", m_sPageData.dwAddress, ((~m_sPageData.dwChecksum)+1));
    return eResult;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Programing_Finish
// DESCRIPTION:
//
//
// Params:
// DWORD *pdwVersion:
// DWORD *pwdCompany:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/17, Leo Create
// --------------------
// ==============================================================================
eRESULT appIapProc_Programing_Finish(DWORD *pdwVersion, DWORD *pwdCompany)
{
    eRESULT eResult = rcERROR;
    BYTE acDummy[ERASE_TAG_SIZE];

    memset(acDummy, 0xFF, ERASE_TAG_SIZE);

    m_sAppMemParam.dwCodeVersion = *pdwVersion;
    //m_sAppMemParam.dwCompany = *pwdCompany;

    m_sAppMemParam.dwCodeStartupAddress = APP_FIRMWARE_START;
    m_sAppMemParam.dwCodeSize = m_sAppMemParam.dwCodeSize; // !!Skip  the App Tag sector!!
    m_sAppMemParam.dwCodeChecksum = (~m_sAppMemParam.dwCodeChecksum + 1);

    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "%X ", m_sAppMemParam.dwCodeVersion);
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "%X ", m_sAppMemParam.dwCodeStartupAddress);
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "%X ", m_sAppMemParam.dwCodeSize);
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "%X \n", m_sAppMemParam.dwCodeChecksum);

    memcpy(acDummy, (BYTE*)&m_sAppMemParam, sizeof(sMEM_TAG_PARAM)/sizeof(BYTE));

#ifdef IAP_DEBUG
    m_cIapEnable = FALSE;
    eResult = rcSUCCESS;
#else
    if(rcSUCCESS == Board_IAP_Write(APP_TAG_ADDR, ERASE_TAG_SIZE, acDummy))
    {
        m_cIapEnable = FALSE;
        eResult = rcSUCCESS;
    }
#endif // IAP_DEBUG
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "Done:%d\n", eResult);

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Run_App_Flag_Set
// DESCRIPTION:
//
//
// Params:
// BYTE cEnable:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/09/09, Leo Create
// --------------------
// ==============================================================================
void appIapProc_Run_App_Flag_Set(BYTE cEnable)
{
    m_cRunAppCode = cEnable;
    g_dwStayInBootLoaderFlag = 0x87654321;
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Run_AppCode_Set
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
void appIapProc_Run_AppCode_Set(void)
{
    APP_CODE_ENTRY fpAppEntryPoint;

    __disable_irq();
    memset((uint32_t *)NVIC->ICER, 0xFF, sizeof(NVIC->ICER));
    memset((uint32_t *)NVIC->ICPR, 0xFF, sizeof(NVIC->ICPR));

    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

    __set_MSP(*((DWORD*)(APP_FIRMWARE_START))); // Reload Stack point
    __set_CONTROL(0);

    if(0x12345678 == g_dwStayInBootLoaderFlag)
    {
        g_dwStayInBootLoaderFlag = 0;
    }

    fpAppEntryPoint = (APP_CODE_ENTRY)*((DWORD*)(APP_FIRMWARE_START + 4));
    (fpAppEntryPoint)();
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Key_Scan
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
// 2019/12/12, Leo Create
// --------------------
// ==============================================================================
void appIapProc_Key_Scan(void)
{
#if 0
    DWORD dwPressCount = 0;

    if(FALSE == JUMP_FROM_APP)
    {
        Board_Gpio_PinMux((sPIN_CFG*)m_asBootHotKeyConfig, LPC54113_BOOT_HOTKEY_NUMBER);

        //Scan Key
        m_dwCounterMain = (SystemCoreClock/100);

        while(m_dwCounterMain--)
        {
            if(LOW == Board_Gpio_Get(eCONNECTOR_BOARD_KEY_4))
            {
                dwPressCount++;
            }
            else
            {
                if(dwPressCount > 0)
                {
                    dwPressCount--;
                }
            }

            if(1000 < dwPressCount)
            {
                g_dwStayInBootLoaderFlag = 0x12345678;
                break;
            }
        }
    }
#endif // 0
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Verify
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
// 2014/07/31, Leo Create
// --------------------
// ==============================================================================
static DWORD appIapProc_Verify(void)
{
    DWORD dwAppVectorCheck = *((DWORD*)APP_FIRMWARE_START);
    DWORD dwStarAddr = 0;
    DWORD dwEndAddr = 0;
    DWORD dwSum = 0;
    //int m_iRomSize = (int)&Image$$LR_IROM1$$Length;

    m_cIapEnable = FALSE;
    m_cRunAppCode = FALSE;

    m_sAppMemParam = *((sMEM_TAG_PARAM*)APP_TAG_ADDR);

    appIapProc_Key_Scan();
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "Run\r\n");

    // 0xFFFFFFFF = dwAppCodeVersion,           Means the flash has been ereased.
    // 0xFFFFFFFF = dwAppVectorCheck,           Means there is no appcode.
    // 0x12345678 = g_dwStayInBootLoaderFlag,   Means Appcode jump to bootcode.
    if((0xFFFFFFFF == m_sAppMemParam.dwCodeVersion) || (0xFFFFFFFF == dwAppVectorCheck))
    {
        return eIAP_APP_CHK_FAIL;
    }
    else if(TRUE == JUMP_FROM_APP)
    {
        return eIAP_APP_UPG;
    }
    else
    {
        dwStarAddr = m_sAppMemParam.dwCodeStartupAddress;
        dwEndAddr   = m_sAppMemParam.dwCodeStartupAddress + m_sAppMemParam.dwCodeSize;

        while(dwStarAddr < dwEndAddr)
        {
            dwSum += *((BYTE *)dwStarAddr);
            dwStarAddr++;
        }

        LOG_MSG(eDB_MSK_LIST_SYSCTRL, "F:0x%08X\r\nC:0x%08X\r\n", m_sAppMemParam.dwCodeChecksum, dwSum);
        LOG_MSG(eDB_MSK_LIST_SYSCTRL, "0x%08X\r\n", m_sAppMemParam.dwCodeChecksum+dwSum);

        if((DWORD)(m_sAppMemParam.dwCodeChecksum + dwSum) == 0)
        {
            return eIAP_APP_CHK_PASS;
        }
        else
        {
            return eIAP_APP_RECHK_FAIL;
        }
    }
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Init
// DESCRIPTION:
//
//
// Params:
// void *pParameters:
//
// Returns:
//
//
// Modification History
// --------------------
// 2020/03/18, Leo Create
// --------------------
// ==============================================================================
void appIapProc_Init(void)
{
    if(appIapProc_Verify() != eIAP_APP_CHK_PASS)
    {
        Board_Init();
        utilLog_Init();
        utilHost_Init();

        __enable_irq();
        
        /* System Monitor thread */
        xTaskCreateStatic(appIapProc_Task,
                          "Iap Task",
                          IAP_TASK_STACK_SZ,
                          NULL,
                          IAP_TASK_PRIORITY,
                          m_axIapMainStack,
                          &m_xIapMainTaskTCB);

        vTaskStartScheduler();
    }
    else
    {
        appIapProc_Run_AppCode_Set();
    }
}

// ==============================================================================
// FUNCTION NAME: appIapProc_Task
// DESCRIPTION:
//
//
// Params:
// void *pParameters:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/02, Leo Create
// --------------------
// ==============================================================================
void appIapProc_Task(void *pParameters)
{
    m_dwLastCheckSum = 0;
    m_dwLastAddress = 123;

    while(1)
    {
        if(TRUE == m_cRunAppCode)
        {
            if(appIapProc_Verify() == eIAP_APP_CHK_PASS)
            {
                utilDelayMs(200);
                appIapProc_Run_AppCode_Set();
            }
            else
            {
                m_cRunAppCode = FALSE;
            }
        }
    }
}

