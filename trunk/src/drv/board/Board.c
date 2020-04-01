// ==============================================================================
// FILE NAME: BOARD.C
// DESCRIPTION:
//
// 初始化與MCU有關的, GPIO, 周邊功能
//
//
// modification history
// --------------------
// 19/08/2013, Leo written
// --------------------
// ==============================================================================


#include "Board.h"
//#include "Board_EEPROM.h"
//#include "Board_EMC.h"
#include "Board_Gpio.h"
//#include "Board_Gpio_Table.h"
#include "Board_I2C.h"
#include "Board_Spi.h"
#include "Board_SystemTick.h"
#include "Board_Uart.h"
#include "Board_Timer.h"
//#include "Board_PMC.h"
#include "Board_Clock.h"
//#include "Board_RTC.h"
//#include "Board_MCPWM.h"
#include "Board_RITimer.h"
//#include "Board_ADC.h" //A70LH_Larry_0023
//#include "Board_SPIFI.h"

#include "utilLogAPI.h"
#include "utilCounterAPI.h"

// ==============================================================================
// Define Section
// ==============================================================================

// ==============================================================================
// Local variable Section
// ==============================================================================

// ==============================================================================
// Initial Tables
// ==============================================================================
#ifdef CORE_M4
static const sBOARD_TIMER_CFG m_sIR_Timer =
{
    eBOARD_TIMER_TIMER1,            // eTimerId
    eBOARD_TIMER_MODE_CAPTURE,      // eTimerMode
    100000,                         // dwPrescale
    TRUE,                           // cEnableInt
    0x01,                           // cPreemption
    0x01,                           // cSubPriority

    .uModeCfg.sCaptureCfg =         // Enable Capture funcion (Need --c99 compiler option)
    {
        eBOARD_TIMER_CHANNLE_1,     // eCaptureChannel
        TIMER_CAPSRC_BOTH_CAPN   // eCaptureMode
    },
};
#endif

#ifdef CORE_M0
static const sBOARD_TIMER_CFG m_sIR_Timer =
{
    eBOARD_TIMER_TIMER0,            // eTimerId
    eBOARD_TIMER_MODE_CAPTURE,      // eTimerMode
    100000,                         // dwPrescale
    TRUE,                           // cEnableInt
    0x01,                           // cPreemption
    0x01,                           // cSubPriority

    .uModeCfg.sCaptureCfg =         // Enable Capture funcion (Need --c99 compiler option)
    {
        eBOARD_TIMER_CHANNLE_0,     // eCaptureChannel
        TIMER_CAPSRC_FALLING_CAPN   // eCaptureMode
    },
};
#endif


// 使用Timer 1當作counter收集轉速
const sBOARD_TIMER_CFG m_sRpmCountCfg =
{
    eBOARD_TIMER_TIMER1,
    eBOARD_TIMER_MODE_COUNTER,
    0,
    FALSE,
    0x01,
    0x01,

    .uModeCfg.sCounterCfg =
    {
        eBOARD_TIMER_CHANNLE_0,
        eBOARD_CAP_MODE_RISE,
    },
};

// ==============================================================================
// FUNCTION NAME: BOARD_INIT
// DESCRIPTION:
//
// 上電後第一次初始化
// 系統頻率12MHz
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 19/08/2013, Leo written
// --------------------
// ==============================================================================
void Board_Init(void)
{
    Board_Gpio_Init();

    Board_Timer_RGU_Reset(m_sIR_Timer.eTimerId);
    Board_Timer_Init((sBOARD_TIMER_CFG *)&m_sIR_Timer);

    Board_Uart_Init(eBOARD_UART_UART0);
    Board_Uart_Init(eBOARD_UART_UART1);
    Board_Uart_Init(eBOARD_UART_UART2);
    Board_Uart_Init(eBOARD_UART_UART3);

    Board_I2C_Init(eBOARD_I2C_BUS0);
    Board_I2C_Init(eBOARD_I2C_BUS1);
    Board_I2C_Init(eBOARD_I2C_BUS2);

    Board_SSP_Init(eBOARD_SSP_SSP0);
    Board_SSP_Init(eBOARD_SSP_SSP1);
}

// ==============================================================================
// FUNCTION NAME: Board_SystemOperationClock
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
// 2017/05/02, Larry Create
// --------------------
// ==============================================================================
void Board_SystemOperationClock(void)
{
    Board_Clock_Init(eBOARD_CLOCK_204MHZ);
}

// ==============================================================================
// FUNCTION NAME: BOARD_POWERON_REINIT
// DESCRIPTION:
//
// 開機後,系統頻率變為204MHz,對應周邊需重新初始化
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 28/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_PowerOn_ReInit(void)
{
    //Board_Clock_Init(eBOARD_CLOCK_204MHZ);
    //Board_RITimer_Init();
    //PMC_Evrt_DeInit();

    //utilDelayMs(10);

    //Board_EMC_Init();

    //utilDelayMs(10);

#ifdef ENABLE_IR
    Board_Timer_RGU_Reset(m_sIR_Timer.eTimerId);
    Board_Timer_Init((sBOARD_TIMER_CFG *)&m_sIR_Timer);
#else
    Board_Timer_RGU_Reset(m_sRpmCountCfg.eTimerId);
    Board_Timer_Init((sBOARD_TIMER_CFG *)&m_sRpmCountCfg);
#endif /* ENABLE_IR */

    //Chip_GPDMA_Init(LPC_GPDMA);

    Board_Uart_Init(eBOARD_UART_UART0);
    Board_Uart_Init(eBOARD_UART_UART1);
    Board_Uart_Init(eBOARD_UART_UART2);
    Board_Uart_Init(eBOARD_UART_UART3);

    Board_I2C_Init(eBOARD_I2C_BUS0);
    Board_I2C_Init(eBOARD_I2C_BUS1);
    //Board_I2C_Init(eBOARD_I2C_BUS2);

    Board_SSP_Init(eBOARD_SSP_SSP0);
    Board_SSP_Init(eBOARD_SSP_SSP1);

#ifdef CORE_M4
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "M4 System Clock %d-MHz\r\n", SystemCoreClock / 1000000L);
#endif // CORE_M4

#ifdef CORE_M0
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "M0 System Clock %d-MHz\r\n", SystemCoreClock / 1000000L);
#endif // CORE_M0
}

// ==============================================================================
// FUNCTION NAME: Board_CommMode_On_ReInit
// DESCRIPTION:
//
// 進入Communication Mode 後,
// 與Lan board通訊需開啟I2C bus 0
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
// 2014/01/11, Leo Create
// --------------------
// ==============================================================================
void Board_CommMode_On_ReInit(void)
{
    Board_SSP_Init(eBOARD_SSP_SSP1);


#ifdef CORE_M4
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "M4 System Clock %d-MHz\r\n", SystemCoreClock / 1000000L);
#endif // CORE_M4

#ifdef CORE_M0
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "M0 System Clock %d-MHz\r\n", SystemCoreClock / 1000000L);
#endif // CORE_M0
}

// ==============================================================================
// FUNCTION NAME: Board_CommMode_Off_ReInit
// DESCRIPTION:
//
// Communication Mode回到Standby Mode,
// 需關閉I2C bus 0
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/09/02, Leo Create
// --------------------
// ==============================================================================
void Board_CommMode_Off_ReInit(void)
{
     Board_SSP_DeInit(eBOARD_SSP_SSP1);

#ifdef CORE_M4
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "M4 System Clock %d-MHz\r\n", SystemCoreClock / 1000000L);
#endif // CORE_M4

#ifdef CORE_M0
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "M0 System Clock %d-MHz\r\n", SystemCoreClock / 1000000L);
#endif // CORE_M0
}

// ==============================================================================
// FUNCTION NAME: BOARD_POWEROFF_REINIT
// DESCRIPTION:
//
// 關機後,系統頻率降回12MHz,
// Power State 回到Communication Mode
// 對應周邊重新初始化
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
// 2014/01/11, Leo Create
// --------------------
// ==============================================================================
void Board_PowerOff_ReInit(void)
{
    //Board_EMC_DeInit();

    // Disable UART
    Board_Uart_DeInit(eBOARD_UART_UART0);
    Board_Uart_DeInit(eBOARD_UART_UART1);
    Board_Uart_DeInit(eBOARD_UART_UART2);
    Board_Uart_DeInit(eBOARD_UART_UART3);

    // Disable I2C
    Board_I2C_DeInit(eBOARD_I2C_BUS0);
    Board_I2C_DeInit(eBOARD_I2C_BUS1);
    Board_I2C_DeInit(eBOARD_I2C_BUS2);

    // Disable SPI
    Board_SSP_DeInit(eBOARD_SSP_SSP0);
    Board_SSP_DeInit(eBOARD_SSP_SSP1);

    //Chip_GPDMA_DeInit(LPC_GPDMA);

    //Board_MCPWM_DeInit();
    Board_GpioInt_DeInit();

    //utilDelayMs(10);
    //Board_Clock_Init(eBOARD_CLOCK_12MHZ);
    //utilDelayMs(10);

    Board_Gpio_Init();
    // Reinit Timer1 for IR
    //Board_RITimer_Init();
    Board_Timer_RGU_Reset(m_sIR_Timer.eTimerId);
    Board_Timer_Init((sBOARD_TIMER_CFG *)&m_sIR_Timer);

    //Chip_GPDMA_Init(LPC_GPDMA);
    // Reconfig Uart
    Board_Uart_Init(eBOARD_UART_UART0);
    Board_Uart_Init(eBOARD_UART_UART1);
    Board_Uart_Init(eBOARD_UART_UART2);
    Board_Uart_Init(eBOARD_UART_UART3);

    // Reconfig I2C bus 0/1
    Board_I2C_Init(eBOARD_I2C_BUS0);
    Board_I2C_Init(eBOARD_I2C_BUS1);
    Board_I2C_Init(eBOARD_I2C_BUS2);

    Board_SSP_Init(eBOARD_SSP_SSP0);
    Board_SSP_Init(eBOARD_SSP_SSP1);

#ifdef CORE_M4
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "M4 System Clock %d-MHz\r\n", SystemCoreClock / 1000000L);
#endif // CORE_M4

#ifdef CORE_M0
    LOG_MSG(eDB_MSK_LIST_SYSCTRL, "M0 System Clock %d-MHz\r\n", SystemCoreClock / 1000000L);
#endif // CORE_M0
}

// ==============================================================================
// FUNCTION NAME: Board_Goto_BL_DeInit
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
// 2014/07/07, Leo Create
// --------------------
// ==============================================================================
void Board_Goto_BL_DeInit(void)
{
    //Board_RITimer_Init();
    //Board_EMC_DeInit();
    Board_Timer_DeInit((sBOARD_TIMER_CFG *)&m_sIR_Timer);

    Board_Uart_DeInit(eBOARD_UART_UART0);
    Board_Uart_DeInit(eBOARD_UART_UART1);
    Board_Uart_DeInit(eBOARD_UART_UART2);
    Board_Uart_DeInit(eBOARD_UART_UART3);

    Board_I2C_DeInit(eBOARD_I2C_BUS0);
    Board_I2C_DeInit(eBOARD_I2C_BUS1);
    Board_I2C_DeInit(eBOARD_I2C_BUS2);

    Board_SSP_DeInit(eBOARD_SSP_SSP0);
    Board_SSP_DeInit(eBOARD_SSP_SSP1);

    //Chip_GPDMA_DeInit(LPC_GPDMA);
    //Board_MCPWM_DeInit();
    Board_GpioInt_DeInit();
    //PMC_Evrt_DeInit();
}


