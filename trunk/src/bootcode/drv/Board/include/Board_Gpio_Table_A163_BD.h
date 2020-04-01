#ifndef BOARD_GPIO_TABLE_A163_BD_H
#define BOARD_GPIO_TABLE_A163_BD_H
// ==============================================================================
// FILE NAME: BOARD_GPIO_TABLE.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 13/11/2013, Leo Create
// --------------------
// ==============================================================================


#include "CommonDef.h"
#include "Board_GPIO_Define.h"
#include "Board_GPIO_Pin_Name.h"

// Global Input Multiplexer Array (GIMA)
static const sGIMA_PIN_CFG m_asGimaConfig[] =
{
    { GIMA_FUNC_CAP, {GIMA_CAP_IN_TIME0, GIMA_CAP_CHANNEL0, DISABLE, DISABLE, DISABLE, DISABLE, LOW}},
};

#define GIMA_PIN_NUMBER sizeof(m_asGimaConfig)/sizeof(sGIMA_PIN_CFG)

// Global Input Multiplexer Array (GIMA)
static const sGIMA_MUX_CFG m_asGimaMuxCfgTable[] =
{
    {m_asGimaConfig, GIMA_PIN_NUMBER},
};

// GPIO pin interrupt
static const sGPIO_INT_CFG m_asGpio_INT_Table[] =
{
    {pIT66341_INT,      eBOARD_INT_PIN1}, 
    //{pRESET_N,          eBOARD_INT_PIN3},
    //{pPWR_ON_N,         eBOARD_INT_PIN4},
};

// GPIO pin interrupt
#define GPIO_INT_PIN_NUMBER sizeof(m_asGpio_INT_Table)/sizeof(sGPIO_INT_CFG)

// ==============================================================================
// FUNCTION NAME: m_asDefaultPinConfig
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/09/16, Chris Create
// --------------------
// ==============================================================================

//A163 PinConfig
static const sPIN_CFG m_asDefaultPinConfig[] =
{
    //N                                 //P             //P         //D
    //E                                 //I             //I         //I
    //T                                 //N             //N         //R
    //                                  //              //          //E
    //N                                 //M             //F         //C
    //A                                 //O             //U         //T
    //M                                 //D             //N         //I
    //E                                 //E             //C         //O
    //                                  //N
    /* P0_0  */ {pGPIO_2,               GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P0_1  */ {pGPIO_3,               GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_0  */ {pP1_0,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_1  */ {pP1_1,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_2  */ {pP1_2,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_3  */ {pP1_3,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_4  */ {pP1_4,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_5  */ {pP1_5,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_6  */ {pP1_6,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_7  */ {pP1_7,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_8  */ {pP1_8,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_9  */ {pP1_9,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_10 */ {pP1_10,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_11 */ {pP1_11,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_12 */ {pP1_12,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_13 */ {pP1_13,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_14 */ {pP1_14,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P1_15 */ {pPWR_GOOD_N,           GPIO_OUT,       FUNC0,      eIO_MODE_OUTPUT,    LOW},
    /* P1_16 */ {pP1_16,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     HIGH},
    /* P1_17 */ {pP1_17,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     HIGH},
    /* P1_18 */ {pP1_18,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     HIGH},
    /* P1_19 */ {pP1_19,                GPIO_INACT,     FUNC2,      eIO_MODE_NONGPIO,   LOW}, // I2S
    /* P1_20 */ {pP1_20,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     HIGH},
    /* P2_0  */ {pP2_0,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P2_1  */ {pP2_1,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P2_2  */ {pP2_2,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P2_3  */ {pMCU_TXD3,             UART_TX,        FUNC2,      eIO_MODE_NONGPIO,   LOW},   // uart 3
    /* P2_4  */ {pMCU_RXD3,             UART_RX,        FUNC2,      eIO_MODE_NONGPIO,   LOW},   // uart 3
    /* P2_5  */ {pP2_5,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P2_6  */ {pP2_6,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P2_7  */ {pP2_7,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P2_8  */ {pP2_8,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P2_9  */ {pP2_9,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P2_10 */ {pP2_10,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P2_11 */ {pP2_11,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P2_12 */ {pP2_12,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P2_13 */ {pP2_13,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P3_0  */ {pP3_0,                 GPIO_INACT,     FUNC5,      eIO_MODE_NONGPIO,   LOW}, // I2S
    /* P3_1  */ {pREAR_IR_EN,           GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    HIGH}, // IR ENABLE?
    /* P3_2  */ {pP3_2,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P3_3  */ {pP3_3,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P3_4  */ {pP3_4,                 GPIO_INACT,     FUNC1,      eIO_MODE_INPUT,     LOW},
    /* P3_5  */ {pP3_5,                 GPIO_INACT,     FUNC1,      eIO_MODE_INPUT,     LOW},
    /* P3_6  */ {pP3_6,                 GPIO_INACT,     FUNC1,      eIO_MODE_INPUT,     LOW},
    /* P3_7  */ {pP3_7,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P3_8  */ {pP3_8,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P4_0  */ {pP4_0,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P4_1  */ {pP4_1,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P4_2  */ {pIT66341_INT,          GPIO_IN,        FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P4_3  */ {p12V_5V_PON,           GPIO_OUT,       FUNC0,      eIO_MODE_OUTPUT,    LOW},
    /* P4_4  */ {pOPS_5V_DET,           GPIO_IN,        FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P4_5  */ {pOPS_HDMI_HPD,         GPIO_OUT,       FUNC0,      eIO_MODE_OUTPUT,    LOW},
    /* P4_6  */ {pMCU_DP_DET,           GPIO_IN,        FUNC0,      eIO_MODE_INPUT,     HIGH},
    /* P4_7  */ {pP4_7,                 GPIO_INACT,     FUNC2,      eIO_MODE_NONGPIO,   LOW}, // I2S
    /* P4_8  */ {pP4_8,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P4_9  */ {pP4_9,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P4_10 */ {pP4_10,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P5_0  */ {pP5_0,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P5_1  */ {pP5_1,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P5_2  */ {pP5_2,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P5_3  */ {pP5_3,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P5_4  */ {pP5_4,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P5_5  */ {pP5_5,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P5_6  */ {pP5_6,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P5_7  */ {pP5_7,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_0  */ {pP6_0,                 GPIO_INACT,     FUNC0,      eIO_MODE_NONGPIO,   LOW}, // I2S
    /* P6_1  */ {pP6_1,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_2  */ {pP6_2,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_3  */ {pP6_3,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_4  */ {pP6_4,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_5  */ {pP6_5,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_6  */ {pP6_6,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_7  */ {pP6_7,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P6_8  */ {pP6_8,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* P6_9  */ {pP6_9,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_10 */ {pP6_10,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_11 */ {pP6_11,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P6_12 */ {pP6_12,                GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P7_0  */ {pNXP_SCL2_A,           GPIO_IN,        FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P7_1  */ {pNXP_SDA2_A,           GPIO_IN,        FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P7_2  */ {pOPS_PSON,             GPIO_OUT,       FUNC0,      eIO_MODE_OUTPUT,    HIGH},
    /* P7_3  */ {pIR_NXP4337,           GPIO_IN,        FUNC1,      eIO_MODE_NONGPIO,   LOW}, // IR 1
    /* P7_4  */ {pOPS_PSOK,             GPIO_IN,        FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P7_5  */ {pCARD_DET_N,           GPIO_IN,        FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P7_6  */ {pOPS_DET_N,            GPIO_IN,        FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P7_7  */ {pSYS_FAN_OUT,          GPIO_IN,        FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P8_0  */ {pP8_0,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P8_1  */ {pD3V3A_EN,             GPIO_OUT,       FUNC0,      eIO_MODE_OUTPUT,    LOW},
    /* P8_2  */ {pLED1_SINK,            GPIO_OUT,       FUNC0,      eIO_MODE_OUTPUT,    HIGH},
    /* P8_3  */ {pP8_3,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P8_4  */ {pP8_4,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P8_5  */ {pLED2_SINK,            GPIO_OUT,       FUNC0,      eIO_MODE_OUTPUT,    HIGH},
    /* P8_6  */ {pP8_6,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P8_7  */ {pP2RXHPD,              GPIO_OUT,       FUNC0,      eIO_MODE_OUTPUT,    LOW},
    /* P8_8  */ {pP8_8,                 GPIO_INACT,     FUNC0,      eIO_MODE_NONGPIO,   LOW}, // I2S
    /* P9_0  */ {pP9_0,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P9_1  */ {pP9_1,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P9_2  */ {pP9_2,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P9_3  */ {pP9_3,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* P9_4  */ {pIT6563_RSTN,          GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* P9_5  */ {pNXP_UART0_TXD,        UART_TX,        FUNC7,      eIO_MODE_NONGPIO,   LOW},   // uart 0
    /* P9_6  */ {pNXP_UART0_RXD,        UART_RX,        FUNC7,      eIO_MODE_NONGPIO,   LOW},   // uart 0
    /* PA_0  */ {pPA_0,                 GPIO_INACT,     FUNC0,      eIO_MODE_NONGPIO,   LOW}, // I2S
    /* PA_1  */ {pNXP_TXD2,             UART_TX,        FUNC3,      eIO_MODE_NONGPIO,   LOW},   // uart 2
    /* PA_2  */ {pNXP_RXD2,             UART_RX,        FUNC3,      eIO_MODE_NONGPIO,   LOW},   // uart 2
    /* PA_3  */ {pPA_3,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW},
    /* PA_4  */ {pPA_4,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PB_0  */ {pPWR_ON_N,             GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    HIGH},
    /* PB_1  */ {pRESET_N,              GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    HIGH},
    /* PB_2  */ {pPB_2,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PB_3  */ {pPB_3,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PB_4  */ {pPB_4,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PB_5  */ {pSLOT_PON,             GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PB_6  */ {pOPS_PON,              GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PC_0  */ {pPC_0,                 GPIO_INACT,     FUNC0,      eIO_MODE_INPUT,     LOW}, // NON GPIO
    /* PC_1  */ {pPV7_GPIO_0,           GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PC_2  */ {pPV7_GPIO_1,           GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PC_3  */ {pT66341_1V0_EN,        GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PC_4  */ {pPV7_GPIO_2,           GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PC_5  */ {pPV7_GPIO_3,           GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PC_6  */ {pIT6563_1V0_EN,        GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PC_7  */ {pIT6563_3V3_EN,        GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PC_8  */ {pUSB_PWR_EN,           GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PC_9  */ {pUSB_OCT,              GPIO_IN,        FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PC_10 */ {pIT66341_3V3_EN,       GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PC_11 */ {pAUDIO_PON,            GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PC_12 */ {pPC_12,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PC_13 */ {pMCU_TXD1,             UART_TX,        FUNC2,      eIO_MODE_NONGPIO,   LOW}, // uart 1
    /* PC_14 */ {pMCU_RXD1,             UART_RX,        FUNC2,      eIO_MODE_NONGPIO,   LOW}, // uart 1
    /* PD_0  */ {pPD_0,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_1  */ {pKEY_PWR,              GPIO_IN_UP,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_2  */ {pPD_2,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_3  */ {pSCDC_ENABLE,          GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PD_4  */ {pPD_4,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_5  */ {pPD_5,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_6  */ {pPD_6,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_7  */ {pPD_7,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_8  */ {pPD_8,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_9  */ {pPD_9,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_10 */ {pPD_10,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_11 */ {pPD_11,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_12 */ {pPD_12,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_13 */ {pIR_HDBASE_T,          GPIO_IN,        FUNC1,      eIO_MODE_NONGPIO,   LOW}, // IR 2
    /* PD_14 */ {pPD_14,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_15 */ {pPD_15,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PD_16 */ {pPD_16,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_0  */ {pPE_0,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_1  */ {pPE_1,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_2  */ {pPE_2,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_3  */ {pPE_3,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_4  */ {pPE_4,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_5  */ {pPE_5,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_6  */ {pPE_6,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_7  */ {pPE_7,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_8  */ {pPE_8,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_9  */ {pPE_9,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_10 */ {pPE_10,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_11 */ {pPE_11,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_12 */ {pPE_12,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_13 */ {pNXP_SDA1_A,           I2C_IO,         FUNC2,      eIO_MODE_NONGPIO,   LOW},
    /* PE_14 */ {pKEY_RIGHT,            GPIO_IN_UP,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PE_15 */ {pNXP_SCL1_A,           I2C_IO,         FUNC2,      eIO_MODE_NONGPIO,   LOW},
    /* PF_0  */ {pCARD_SPI_SCK,         SSP_IO,         FUNC0,      eIO_MODE_NONGPIO,   LOW},
    ///* PF_1  */ {pCARD_SPI_CS,          SSP_IO,         FUNC4,      eIO_MODE_OUTPUT,    HIGH},
    /* PF_1  */ {pCARD_SPI_CS,          SSP_IO,         FUNC2,      eIO_MODE_NONGPIO,   LOW},
    /* PF_2  */ {pCARD_SPI_MISO,        SSP_IO,         FUNC2,      eIO_MODE_NONGPIO,   LOW},
    /* PF_3  */ {pCARD_SPI_MOSI,        SSP_IO,         FUNC2,      eIO_MODE_NONGPIO,   LOW},
    /* PF_4  */ {pPV7_SPI_CLK,          SSP_IO,         FUNC0,      eIO_MODE_NONGPIO,   LOW},
    /* PF_5  */ {pPV7_SPI_CSn,          SSP_IO,         FUNC2,      eIO_MODE_NONGPIO,   LOW},
    /* PF_6  */ {pPV7_SPI_MISO,         SSP_IO,         FUNC2,      eIO_MODE_NONGPIO,   LOW},
    /* PF_7  */ {pPV7_SPI_MOSI,         SSP_IO,         FUNC2,      eIO_MODE_NONGPIO,   LOW},
    /* PF_8  */ {pPF_8,                 GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PF_9  */ {pIT66341_RSTN,         GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
    /* PF_10 */ {pPF_10,                GPIO_INACT,     FUNC4,      eIO_MODE_INPUT,     LOW},
    /* PF_11 */ {pEDID_SW,              GPIO_OUT,       FUNC4,      eIO_MODE_OUTPUT,    LOW},
};

#define DEFAULT_PIN_NUMBERS sizeof(m_asDefaultPinConfig)/sizeof(sPIN_CFG)


static const sPIN_CLK_CFG m_asCLKPinConfig[] =
{
    //N                             //P                 //P
    //E                             //I                 //I
    //T                             //N                 //N
    //                              //                  //
    //N                             //M                 //F
    //A                             //O                 //U
    //M                             //D                 //N
    //E                             //E                 //C
    //                              //N

    /* CLK0  */ {pCLK0,             GPIO_CLK,           FUNC0},
    /* CLK1  */ {pCLK1,             GPIO_CLK,           FUNC0},
    /* CLK2  */ {pCLK2,             GPIO_CLK,           FUNC0},
    /* CLK3  */ {pCLK3,             GPIO_CLK,           FUNC0},
};

#define DEFAULT_CLKPIN_NUMBERS sizeof(m_asCLKPinConfig)/sizeof(sPIN_CLK_CFG)


// ==============================================================================
// FUNCTION NAME: m_asProjPwrOnPwrSeq
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/09/16, Larry Create
// --------------------
// ==============================================================================

static const sPWRSEQ m_asPwrOnPwrSeq[] =
{
    {pD3V3A_EN,             HIGH,   2000}, // Delay 2s for power ready

    {pD3V3A_EN,             HIGH,   0},
    {p12V_5V_PON,           HIGH,   100},
    {pT66341_1V0_EN,        HIGH,   1},
    {pIT66341_3V3_EN,       HIGH,   0},
    {pIT6563_1V0_EN,        HIGH,   1},
    {pIT6563_3V3_EN,        HIGH,   12},

    {pIT6563_RSTN,          HIGH,   0},
    {pIT66341_RSTN,         HIGH,   0},

    {pBOARD_PIN_NUMBERS,    0,      0},
};

#endif /* BOARD_GPIO_TABLE_A163_BD_H */

